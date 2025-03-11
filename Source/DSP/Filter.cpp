#include "Filter.h"
#include "../Macros.h"
#include <JuceHeader.h>

// =============================================================================
const double FilterElement::gain_floor_db       = -128.0;
const double FilterElement::pole_magnitude_ceil = 0.99999;

// =============================================================================
const std::string FilterElement::typeToString(FilterElement::Type t)
{
    switch (t)
    {
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter element type. Defaulting to 'UNKNOWN'");
        return "UNKNOWN";
    case FilterElement::Type::ZERO: return "ZERO";
    case FilterElement::Type::POLE: return "POLE";
    }
}
const std::string FilterElement::typeToString(float t)
{
    return FilterElement::typeToString(FilterElement::floatToType(t));
}
float FilterElement::typeToFloat(FilterElement::Type t)
{
    return static_cast<float>(static_cast<int>(t));
}
FilterElement::Type FilterElement::floatToType(float f)
{
    switch (juce::roundToInt(f))
    {
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter element type. Defaulting to 'ZERO'");
    case FilterElement::Type::ZERO: return FilterElement::Type::ZERO;
    case FilterElement::Type::POLE: return FilterElement::Type::POLE;
    }
}

// =============================================================================
FilterElement::FilterElement()
    : type(FilterElement::Type::ZERO)
    , magnitude(0.0)
    , phase(0.0)
    , gain(1.0)
    , active(false)
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
FilterElement::Type FilterElement::getType() const { return type; }
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
    switch (type)
    {
    default:
        UNHANDLED_SWITCH_CASE("Unhandled case for filter element type. "
                              "Returning pass-through filter coefficients");
        return {static_cast<double>(false), 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0};
        break;
    case FilterElement::Type::ZERO:
        return {static_cast<double>(active),
                1.0,
                0.0,
                0.0,
                1.0,
                coeffs[0],
                coeffs[1],
                gain};
        break;
    case FilterElement::Type::POLE:
        return {static_cast<double>(active),
                1.0,
                coeffs[0],
                coeffs[1],
                1.0,
                0.0,
                0.0,
                gain};
        break;
    }
}

// =============================================================================
void FilterElement::setMagnitude(double m)
{
    switch (type)
    {
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter element type. Defaulting to 'ZERO'");
    case FilterElement::Type::ZERO: break;  // nothing to do, ok
    case FilterElement::Type::POLE:
        if (m > pole_magnitude_ceil) m = pole_magnitude_ceil;
        break;
    }
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
void FilterElement::setType(FilterElement::Type t)
{
    if (type != t)
    {
        type = t;
        computeCoefficients();
        resetMemory();
    }
    switch (type)
    {
    case FilterElement::Type::POLE:
        processSampleFunc = &FilterElement::processSamplePole;
        break;
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter element type. Defaulting to 'ZERO'");
    case FilterElement::Type::ZERO:
        processSampleFunc = &FilterElement::processSampleZero;
        break;
    }
    setMagnitude(getMagnitude());
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

// =============================================================================
void FilterElement::resetMemory() { memory[0] = memory[1] = 0.0; }
void FilterElement::pushSample(double x)
{
    memory[1] = memory[0];
    memory[0] = x;
}
void FilterElement::computeCoefficients()
{
    coeffs[0] = -2 * getRealPart();
    coeffs[1] = magnitude * magnitude;
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
std::complex<double> FilterElement::dtft(double omega) const
{
    std::complex<double> z_inv = std::exp(std::complex<double>(0.0, -omega));

    std::complex<double> h
        = 1.0 + coeffs[0] * z_inv + coeffs[1] * z_inv * z_inv;
    switch (type)
    {
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter element type. Defaulting to 'ZERO'");
    case FilterElement::Type::ZERO: h *= gain; break;
    case FilterElement::Type::POLE: h = gain / h; break;
    }

    return h;
}
static double _wgl(double x)
{
    static const double c = -2.0 * exp(-0.5);
    return (x < 0.5) ? exp(-2.0 * x * x) : c * (x - 1.0);
}
double FilterElement::rmsg() const
{
    double g;
    switch (type)
    {
    default:
        UNHANDLED_SWITCH_CASE("Unhandled case for filter element type. "
                              "Returning the default value: 1.0");
        g = 1.0;
        break;
    case FilterElement::Type::ZERO:
        // 1 + 4Re^2{z} + |z|^4
        g = 1.0 + coeffs[0] * coeffs[0] + coeffs[1] * coeffs[1];
        break;
    case FilterElement::Type::POLE:
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
        // g        = abs(1.0 - coeffs[1]) * one_mp2 * one_mp2;
        // Approx v2
        // W_GL(|p|) * |1 - p^2|^2
        g = _wgl(sqrt(coeffs[1])) * one_mp2 * one_mp2;
        break;
    }
    }
    return std::sqrt(g);
}
double FilterElement::rmsgDb() const
{
    return juce::Decibels::gainToDecibels(rmsg(), gain_floor_db);
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
