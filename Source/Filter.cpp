#include "Filter.h"
#include "Macros.h"

// =============================================================================
const double FilterElement::gain_floor_db       = -128.0;
const double FilterElement::pole_magnitude_ceil = 0.99999;

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
FilterElement::FilterElement(FilterElement& other)
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
    return juce::Decibels::gainToDecibels(getGain(), gain_floor_db);
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
std::array<double, 3> FilterElement::getCoefficients() const
{
    return {gain, coeffs[0], coeffs[1]};
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
void FilterElement::processBlock(double* outputs, double* inputs, int n)
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
