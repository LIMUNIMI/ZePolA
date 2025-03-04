#include "FilterDesign.h"
#include "../Macros.h"

// =============================================================================
juce::String FilterParameters::typeToString(FilterParameters::FilterType t)
{
    switch (t)
    {
    case FilterParameters::FilterType::Butterworth: return "Butterworth";
    case FilterParameters::FilterType::ChebyshevI: return "Chebyshev I";
    case FilterParameters::FilterType::ChebyshevII: return "Chebyshev II";
    case FilterParameters::FilterType::Elliptic: return "Elliptic";
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
    case FilterParameters::FilterShape::HighPass: return "HighPass";
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
    , passbandRippleDb(1.0)
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
FilterParameters::ZPK::ZPK() { }
void FilterParameters::ZPK::reset()
{
    poles.clear();
    zeros.clear();
    gain = 1.0;
}
size_t FilterParameters::ZPK::nElements() const
{
    return poles.size() + zeros.size();
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
FilterFactory::FilterFactory() { }
void FilterFactory::build(FilterParameters& params)
{
    sanitizeParams(params);
    params.zpk.reset();
    ButterworthFilterFactory butterFF;
    ChebyshevIFilterFactory chebIFF;
    switch (params.type)
    {
    case (FilterParameters::FilterType::Butterworth):
        butterFF.build(params);
        break;
    case (FilterParameters::FilterType::ChebyshevI):
        chebIFF.build(params);
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
    if (params.passbandRippleDb <= 0) params.passbandRippleDb = 1.0;
    params.cutoff = std::clamp(params.cutoff, 0.0, params.sr * 0.5);
}

// =============================================================================
AnalogFilterFactory::AnalogFilterFactory() { }
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
    case (FilterParameters::FilterShape::HighPass):
        applyHighPassParamsToPrototype(params);
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
    // Gain compensation
    params.zpk.gain *= pow(w, static_cast<float>(params.zpk.relativeDegree()));
}
void AnalogFilterFactory::applyHighPassParamsToPrototype(
    FilterParameters& params)
{
    auto w     = params.warpedFrequency();
    double k_z = 1.0, k_p = 1.0, a;
    for (auto& z : params.zpk.zeros)
    {
        a = abs(z);
        z = std::polar(w / a, std::arg(z));
        k_z *= a * a;  // Square to account for conjugates
    }
    for (auto& p : params.zpk.poles)
    {
        a = abs(p);
        p = std::polar(w / a, std::arg(p));
        k_p *= a * a;  // Square to account for conjugates
    }
    // Lowpass implies zeros at Nyquist => inverting makes them zeros at origin
    auto rdeg = params.zpk.relativeDegree();
    jassert(rdeg % 2 == 0);
    // Half the amount because they will be doubled by their conjugates
    rdeg /= 2;
    for (auto i = 0; i < rdeg; i++)
        params.zpk.zeros.push_back(std::complex<double>(0.0, 0.0));
    // Gain compensation
    params.zpk.gain *= k_z / k_p;
}
void AnalogFilterFactory::bilinearTransform(FilterParameters::ZPK& zpk)
{
    double k_z = 1.0, k_p = 1.0, a;

    // Bilinear transform
    std::complex<double> tmp;
    for (auto& z : zpk.zeros)
    {
        // (4.0 - z) * (4.0 - z*) Also accounts for conjugate
        a = abs(z);
        k_z *= a * a - 16.0 * z.real() + 16.0;
        z = (4.0 + z) / (4.0 - z);
    }
    for (auto& p : zpk.poles)
    {
        // (4.0 - p) * (4.0 - p*) Also accounts for conjugate
        a = abs(p);
        k_p *= a * a - 16.0 * p.real() + 16.0;
        p = (4.0 + p) / (4.0 - p);
    }

    // Add zeros at Nyquist
    auto rdeg = zpk.relativeDegree();
    jassert(rdeg % 2 == 0);
    // Half the amount because they will be doubled by their conjugates
    rdeg /= 2;
    for (auto i = 0; i < rdeg; i++)
        zpk.zeros.push_back(std::complex<double>(-1.0, 0.0));

    // Gain compensation
    zpk.gain *= k_z / k_p;
}

// =============================================================================
ButterworthFilterFactory::ButterworthFilterFactory() { }
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

// =============================================================================
ChebyshevIFilterFactory::ChebyshevIFilterFactory() { }
void ChebyshevIFilterFactory::buildAnalogPrototype(FilterParameters& params)
{
    // Ripple factor
    double eps = sqrt(pow(10.0, 0.1 * params.passbandRippleDb) - 1.0);
    double mu  = std::asinh(1.0 / eps) / params.order;

    // This only generates positive imaginary-part poles (conjugates will be
    // added later)
    for (auto m = 1 - params.order; m < 0; m += 2)
        params.zpk.poles.push_back(-std::sinh(std::complex(
            mu, juce::MathConstants<double>::pi * m / (2 * params.order))));
    double a;
    params.zpk.gain = 1.0;
    for (const auto& p : params.zpk.poles)
    {
        a = abs(p);
        params.zpk.gain *= a * a;
    }
    if (params.order % 2 == 0) params.zpk.gain /= sqrt(1 + eps * eps);
    ONLY_ON_DEBUG(else jassertfalse;)
}
