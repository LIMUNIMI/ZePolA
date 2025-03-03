#include "FilterDesign.h"
#include "../Macros.h"

// =============================================================================
juce::String FilterParameters::typeToString(FilterParameters::FilterType t)
{
    switch (t)
    {
    case FilterParameters::FilterType::Butterworth: return "Butterworth";
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter type. Defaulting to 'UNKNOWN'");
        return "UNKNOWN";
    }
}
juce::String FilterParameters::shapeToString(FilterParameters::FilterShape s)
{
    switch (s)
    {
    case FilterParameters::FilterShape::LowPass: return "LowPass";
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter shape. Defaulting to 'UNKNOWN'");
        return "UNKNOWN";
    }
}

// =============================================================================
FilterParameters::FilterParameters(double f)
    : sr(f)
    , type(FilterParameters::FilterType::Butterworth)
    , shape(FilterParameters::FilterShape::LowPass)
    , order(2)
    , cutoff(0.5 * f)
{
}
void FilterParameters::computeZPK()
{
    FilterFactory factory;
    factory.build(*this);
}
double FilterParameters::normalizedFrequency() const
{
    return 2.0 * cutoff / sr;
}
double FilterParameters::warpedFrequency() const
{
    return 4.0
           * tan(juce::MathConstants<double>::halfPi * normalizedFrequency());
}

// =============================================================================
FilterParameters::ZPK::ZPK() {}
void FilterParameters::ZPK::reset()
{
    poles.clear();
    zeros.clear();
    gain = 1.0;
}
int FilterParameters::ZPK::relativeDegree() const
{
    // Double the degree to account for conjugates
    return 2
           * (static_cast<int>(poles.size()) - static_cast<int>(zeros.size()));
}
size_t FilterParameters::ZPK::degree() const
{
    // Double the degree to account for conjugates
    return 2 * std::max(poles.size(), zeros.size());
}

// =============================================================================
FilterFactory::FilterFactory() {}
void FilterFactory::build(FilterParameters& params)
{
    sanitizeParams(params);
    params.zpk.reset();
    ButterworthFilterFactory butterFF;
    switch (params.type)
    {
    case (FilterParameters::FilterType::Butterworth):
        butterFF.build(params);
        break;
    default:
        UNHANDLED_SWITCH_CASE("Unhandled case for filter type. Doing nothing");
        break;
    }
}
void FilterFactory::sanitizeParams(FilterParameters& params)
{
    if (params.type >= FilterParameters::FilterType::N_FILTER_TYPES)
        params.type = FilterParameters::FilterType::Butterworth;
    if (params.shape >= FilterParameters::FilterShape::N_FILTER_SHAPES)
        params.shape = FilterParameters::FilterShape::LowPass;
    if (!params.sr) params.sr = 1.0;
    if (params.order < 2) params.order = 2;
    if (params.order % 2) params.order++;
    params.cutoff = std::clamp(params.cutoff, 0.0, params.sr * 0.5);
}

// =============================================================================
AnalogFilterFactory::AnalogFilterFactory() {}
void AnalogFilterFactory::build(FilterParameters& params)
{
    buildAnalogPrototype(params);
    applyParamsToPrototype(params);
    bilinearTransform(params.zpk);
}
void AnalogFilterFactory::applyParamsToPrototype(FilterParameters& params)
{
    switch (params.shape)
    {
    case (FilterParameters::FilterShape::LowPass):
        applyLowPassParamsToPrototype(params);
        break;
    default:
        UNHANDLED_SWITCH_CASE(
            "Unhandled case for filter shape. Resetting filter");
        params.zpk.reset();
        break;
    }
}
void AnalogFilterFactory::applyLowPassParamsToPrototype(
    FilterParameters& params)
{
    auto w = params.warpedFrequency();
    for (auto& z : params.zpk.zeros) z *= w;
    for (auto& p : params.zpk.poles) p *= w;
    params.zpk.gain *= pow(w, static_cast<float>(params.zpk.relativeDegree()));
}
void AnalogFilterFactory::bilinearTransform(FilterParameters::ZPK& zpk)
{
    std::complex<double> k_z(1.0, 0.0), k_p(1.0, 0.0);

    // Bilinear transform
    std::complex<double> tmp;
    for (auto& z : zpk.zeros)
    {
        tmp = 4.0 - z;
        // Also account for conjugate
        k_z *= tmp * (4.0 - std::conj(z));
        z = (4.0 + z) / tmp;
    }
    for (auto& p : zpk.poles)
    {
        tmp = 4.0 - p;
        // Also account for conjugate
        k_p *= tmp * (4.0 - std::conj(p));
        p = (4.0 + p) / tmp;
    }

    // Add zeros at Nyquist
    auto rdeg = zpk.relativeDegree();
    jassert(rdeg % 2 == 0);
    // Half the amount because they will be doubled by their conjugates
    rdeg /= 2;
    for (auto i = 0; i < rdeg; i++)
        zpk.zeros.push_back(std::complex<double>(-1.0, 0.0));

    // Gain compensation
    zpk.gain *= (k_z / k_p).real();
}

// =============================================================================
ButterworthFilterFactory::ButterworthFilterFactory() {}
void ButterworthFilterFactory::buildAnalogPrototype(FilterParameters& params)
{
    // This loop would be used to generate all poles
    // for (auto m = 1 - params.order; m < params.order; m += 2)

    // This only generates positive imaginary-part poles (conjugates will be
    // added later)
    for (auto m = 1; m < params.order; m += 2)
        params.zpk.poles.push_back(-exp(std::complex(
            0.0, juce::MathConstants<double>::pi * m / (-2 * params.order))));
}
