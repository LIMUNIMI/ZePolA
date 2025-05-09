/*
  ==============================================================================

    Filter.cpp

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

#include "Filter.h"
#include "../Macros.h"
#include <JuceHeader.h>

// =============================================================================
const double FilterElement::gain_floor_db       = -128.0;
const double FilterElement::pole_magnitude_ceil = 0.99999;
const double FilterElement::inv_magnitude_floor = 1e-6;

// =============================================================================
const std::string FilterElement::typeToString(bool t)
{
    return (t) ? "POLE" : "ZERO";
}
const std::string FilterElement::typeToString(float t)
{
    return FilterElement::typeToString(t > 0.5f);
}

// =============================================================================
FilterElement::FilterElement()
    : type(false)
    , magnitude(0.0)
    , phase(0.0)
    , gain(1.0)
    , active(false)
    , inverted(false)
    , single(false)
    , processSampleFunc(&FilterElement::processSampleZero)
{
    resetMemory();
    computeCoefficients();
}
FilterElement::FilterElement(const FilterElement& other)
    : type(other.type)
    , magnitude(other.magnitude)
    , phase(other.phase)
    , gain(other.gain)
    , active(other.active)
    , processSampleFunc(other.processSampleFunc)
{
    resetMemory();
    computeCoefficients();
}

// =============================================================================
double FilterElement::getMagnitude() const { return magnitude; }
double FilterElement::getPhase() const { return phase; }
double FilterElement::getAngle() const
{
    return getPhase() * MathConstants<double>::pi;
}
bool FilterElement::getType() const { return type; }
bool FilterElement::getSingle() const { return single; }
bool FilterElement::getInverted() const { return inverted; }
double FilterElement::getGain() const { return gain; }
double FilterElement::getGainDb() const
{
    return juce::Decibels::gainToDecibels(gain, gain_floor_db);
}
bool FilterElement::getActive() const { return active; }
double FilterElement::getRealPart() const
{
    return getMagnitude() * std::cos(getAngle());
}
double FilterElement::getImagPart() const
{
    return getMagnitude() * std::sin(getAngle());
}
std::array<double, 8> FilterElement::getCoefficients() const
{
    if (type)
    {
        // POLE
        return {static_cast<double>(active),
                1.0,
                coeffs[0],
                coeffs[1],
                1.0,
                0.0,
                0.0,
                gain};
    }
    else
    {
        // ZERO
        return {static_cast<double>(active),
                1.0,
                0.0,
                0.0,
                1.0,
                coeffs[0],
                coeffs[1],
                gain};
    }
}
std::array<double, 2> FilterElement::getCoefficientsRaw() const
{
    return {coeffs[0], coeffs[1]};
}

// =============================================================================
void FilterElement::setMagnitude(double m)
{
    if (inverted && m < inv_magnitude_floor) m = inv_magnitude_floor;
    if (type && m > pole_magnitude_ceil) m = pole_magnitude_ceil;
    magnitude = m;
    computeCoefficients();
}
void FilterElement::setPhase(double p)
{
    // Periodicity (1 is π rad/sample)
    phase = std::fmod(p, 2.0);
    // Conjugate symmetry
    if (phase > 1.0) phase = 2.0 - phase;
    computeCoefficients();
}
void FilterElement::setType(bool t)
{
    if (type != t)
    {
        type = t;
        setMagnitude(getMagnitude());
        resetMemory();
        processSampleFunc = (type) ? &FilterElement::processSamplePole
                                   : &FilterElement::processSampleZero;
    }
}
void FilterElement::setGain(double g) { gain = g; }
void FilterElement::setGainDb(double g)
{
    setGain(juce::Decibels::decibelsToGain(g, gain_floor_db));
}
void FilterElement::setActive(bool a)
{
    if (a && a != active) resetMemory();
    active = a;
}
void FilterElement::setInactive() { setActive(false); }
void FilterElement::setInverted(bool i)
{
    if (i != inverted)
    {
        inverted = i;
        resetMemory();
        setMagnitude(getMagnitude());
    }
}
void FilterElement::setSingle(bool s)
{
    if (s != single)
    {
        single = s;
        if (single) resetMemory();
        setPhase(getPhase());
    }
}

// =============================================================================
void FilterElement::resetMemory() { memory[0] = memory[1] = 0.0; }
void FilterElement::pushSample(double x)
{
    memory[1] = memory[0];
    memory[0] = x;
}
void FilterElement::computeCoefficients()
{
    double m = (inverted) ? 1.0 / magnitude : magnitude;
    if (single)
    {
        coeffs[0] = (getPhase() > 0.5) ? m : -m;
        coeffs[1] = 0.0;
    }
    else
    {
        coeffs[0] = -2.0 * m * std::cos(getAngle());
        coeffs[1] = m * m;
    }
}

// =============================================================================
double FilterElement::processSampleZero(double x)
{
    double y = x + coeffs[0] * memory[0] + coeffs[1] * memory[1];
    pushSample(x);
    return y;
}
double FilterElement::processSamplePole(double x)
{
    double y = x - coeffs[0] * memory[0] - coeffs[1] * memory[1];
    pushSample(y);
    return y;
}
void FilterElement::processBlock(double* outputs, const double* inputs, int n)
{
    for (int i = 0; i < n; ++i)
        outputs[i] = (this->*processSampleFunc)(gain * inputs[i]);
}

// =============================================================================
std::complex<double> FilterElement::_dtft_withGain(double omega, double g) const
{
    std::complex<double> z_inv = std::exp(std::complex<double>(0.0, -omega));
    std::complex<double> h     = 1.0 + (coeffs[0] + coeffs[1] * z_inv) * z_inv;
    return (type) ? g / h : g * h;
}
std::complex<double> FilterElement::dtft(double omega) const
{
    return _dtft_withGain(omega, gain);
}
// static double _wgl(double x)
// {
//     static const double c = -2.0 * exp(-0.5);
//     return (x < 0.5) ? exp(-2.0 * x * x) : c * (x - 1.0);
// }
double FilterElement::rmsg() const
{
    double g;
    if (type)
    {
        // double r = getRealPart();
        // double i = getImagPart();
        double a     = getAngle();
        auto one_mp2 = abs(1.0 - std::polar(coeffs[1], a + a));
        // Approx v0
        // |1 - |p|^2| * (Re^2{p} - 2|Re{z}| + Im^2{p} + 1)
        // g = abs(1.0 - coeffs[1]) * (r * r + coeffs[0] + i * i + 1.0);
        // Approx v1
        // |1 - |p|^2| * |1 - p^2|^2
        g = abs(1.0 - coeffs[1]) * one_mp2 * one_mp2;
        // Approx v2
        // W_GL(|p|) * |1 - p^2|^2
        // g = _wgl(sqrt(coeffs[1])) * one_mp2 * one_mp2;
    }
    else
    {
        // 1 + 4Re^2{z} + |z|^4
        g = 1.0 + coeffs[0] * coeffs[0] + coeffs[1] * coeffs[1];
    }
    return std::sqrt(g);
}
double FilterElement::rmsgDb() const
{
    return juce::Decibels::gainToDecibels(rmsg(), gain_floor_db);
}
double FilterElement::peak() const
{
    if (magnitude < std::numeric_limits<double>::epsilon()) return 1.0;
    // cos(phi) = (|z|^2 + 1) * Re{z} / (2 * |z|^2|)
    double phi = std::acos((pow(magnitude, 2.0) + 1.0) * getRealPart()
                           / (2.0 * pow(magnitude, 2.0)));
    // DTFT evaluated at derivative zeros
    std::array<double, 4> d_zeros = {0.0, juce::MathConstants<double>::pi, phi,
                                     phi + juce::MathConstants<double>::pi};
    for (auto it = d_zeros.begin(); it != d_zeros.end(); ++it)
        *it = abs(_dtft_withGain(*it, 1.0));
    ONLY_ON_DEBUG({
        DBG("Peak dB: max({");
        for (auto g : d_zeros)
            DBG("  " << juce::Decibels::gainToDecibels(g, gain_floor_db));
        DBG("})");
    });
    return *std::max_element(d_zeros.begin(), d_zeros.end());
}
double FilterElement::peakDb() const
{
    return juce::Decibels::gainToDecibels(peak(), gain_floor_db);
}

// =============================================================================
FilterElementCascade::FilterElementCascade(int n)
{
    for (int i = 0; i < n; ++i) addElement();
}
FilterElementCascade::FilterElementCascade(const FilterElementCascade& other)
{
    for (auto e : other.elements) addElement(e);
}

// =========================================================================
void FilterElementCascade::addElement() { elements.push_back(FilterElement()); }
void FilterElementCascade::addElement(const FilterElement& other)
{
    elements.push_back(FilterElement(other));
}

// =========================================================================
void FilterElementCascade::resetMemory()
{
    for (auto e : elements) e.resetMemory();
}

// =========================================================================
std::vector<FilterElement>::iterator FilterElementCascade::begin()
{
    return elements.begin();
}
std::vector<FilterElement>::iterator FilterElementCascade::end()
{
    return elements.end();
}
size_t FilterElementCascade::size() const { return elements.size(); }
bool FilterElementCascade::empty() const { return elements.empty(); }
FilterElement& FilterElementCascade::operator[](size_t i)
{
    return elements[i];
}
const FilterElement& FilterElementCascade::operator[](size_t i) const
{
    return elements[i];
}

// =========================================================================
std::complex<double> FilterElementCascade::dtft(double omega) const
{
    std::complex<double> h(1.0, 0.0);
    for (auto& e : elements)
        if (e.getActive()) h *= e.dtft(omega);
    return h;
}
std::vector<std::array<double, 8>> FilterElementCascade::getCoefficients() const
{
    std::vector<std::array<double, 8>> coeffs;
    for (auto& e : elements) coeffs.push_back(e.getCoefficients());
    return coeffs;
}

// =========================================================================
void FilterElementCascade::processBlock(double* outputs, const double* inputs,
                                        int n)
{
    for (auto& e : elements)
        if (e.getActive()) e.processBlock(outputs, inputs, n);
}
