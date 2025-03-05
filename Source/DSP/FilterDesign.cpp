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
    , cutoff(0.25 * f)
    , passbandRippleDb(3.0)
    , stopbandRippleDb(20.0)
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
    switch (params.type)
    {
    case (FilterParameters::FilterType::Butterworth):
    {
        ButterworthFilterFactory ff;
        ff.build(params);
    }
    break;
    case (FilterParameters::FilterType::ChebyshevI):
    {
        ChebyshevIFilterFactory ff;
        ff.build(params);
    }
    break;
    case (FilterParameters::FilterType::ChebyshevII):
    {
        ChebyshevIIFilterFactory ff;
        ff.build(params);
    }
    break;
    case (FilterParameters::FilterType::Elliptic):
    {
        EllipticFilterFactory ff;
        ff.build(params);
    }
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
    if (params.passbandRippleDb <= std::numeric_limits<double>::epsilon())
        params.passbandRippleDb = 3.0;
    if (params.stopbandRippleDb <= std::numeric_limits<double>::epsilon())
        params.stopbandRippleDb = 20.0;
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

    // Gain compensation
    double a;
    for (const auto& p : params.zpk.poles)
    {
        a = abs(p);
        params.zpk.gain *= a * a;  // Square to account for conjugates
    }
    if (params.order % 2 == 0) params.zpk.gain /= sqrt(1 + eps * eps);
    ONLY_ON_DEBUG(else jassertfalse;)
}

// =============================================================================
ChebyshevIIFilterFactory::ChebyshevIIFilterFactory() { }
void ChebyshevIIFilterFactory::buildAnalogPrototype(FilterParameters& params)
{
    // Ripple factor
    double eps = sqrt(pow(10.0, 0.1 * params.stopbandRippleDb) - 1.0);
    double mu  = std::asinh(eps) / params.order;
    // eps     = 1.0 / eps;

    jassert(params.order % 2 == 0);
    // This only generates positive imaginary-part poles and zeros (conjugates
    // will be added later)
    for (auto m = 1; m < params.order; m += 2)
    {
        params.zpk.zeros.push_back(-conj(
            (std::complex(0.0, 1.0)
             / sin(m * juce::MathConstants<double>::halfPi / params.order))));
        auto p = -exp(std::complex(0.0, juce::MathConstants<double>::pi * m
                                            / (2 * params.order)));
        p = std::complex(std::sinh(mu) * p.real(), std::cosh(mu) * p.imag());
        p = 1.0 / p;
        params.zpk.poles.push_back(p);
    }

    // Gain compensation
    double k_p = 1.0, k_z = 1.0, a;
    for (const auto& p : params.zpk.poles)
    {
        a = abs(p);
        k_p *= a * a;  // Square to account for conjugates
    }
    for (const auto& z : params.zpk.zeros)
    {
        a = abs(z);
        k_z *= a * a;  // Square to account for conjugates
    }
    params.zpk.gain *= k_p / k_z;
}

// =============================================================================
// Utilities for elliptic filters
/** Maximum iterations for convergence in _arc_jac_sn() */
#define _ARC_JAC_SN_CONV_LIMIT 11
/** Approximation degree for function _ellipdeg() */
#define _ELLIPTIC_MAX_DEG 7
// Functions from cephes library
extern "C"
{
    /** Complete elliptic integral */
    extern double ellpk(double);
    /** Jacobian elliptic functions */
    extern int ellpj(double u, double m, double* sn, double* cn, double* dn,
                     double* ph);
}
static double _ellipdeg(int n, double m)
{
    double num = 0.0, den = 1.0;
    double k1  = ellpk(1.0 - m);
    double k1p = ellpk(m);
    double q1  = exp(-juce::MathConstants<double>::pi * k1p / k1);
    double q   = pow(q1, 1.0 / static_cast<double>(n));
    for (auto i = 0; i <= _ELLIPTIC_MAX_DEG; ++i)
    {
        auto j = static_cast<double>(i + 1);
        num += pow(q, i * j);
        den += 2.0 * pow(q, j * j);
    }
    return 16.0 * q * pow(num / den, 4.0);
}
/** Helper function for _arc_jac_sn() */
template <typename ValueType>
ValueType _arc_jac_sn_complement(ValueType x)
{
    return sqrt((1.0 - x) * (1.0 + x));
}
/** Inverse Jacobian elliptic sn */
static std::complex<double> _arc_jac_sn(std::complex<double> w, double m)
{
    double k = sqrt(m);
    if (k > 1.0) return std::nan("_arc_jac_sn(): k > 1.0");
    if (k == 1.0) return std::atanh(w);
    std::vector<double> ks({k});
    {
        int i = 0;
        for (; ks.back() > std::numeric_limits<double>::epsilon()
               && i < _ARC_JAC_SN_CONV_LIMIT;
             ++i)
        {
            double k_  = ks.back();
            double k_p = _arc_jac_sn_complement(k_);
            ks.push_back((1.0 - k_p) / (1.0 + k_p));
        }
        if (i >= _ARC_JAC_SN_CONV_LIMIT)
            return std::nan("_arc_jac_sn(): convergence error");
    }

    double K = juce::MathConstants<double>::halfPi;
    for (auto it = ks.begin() + 1; it != ks.end(); ++it) K *= (1.0 + *it);

    std::vector<std::complex<double>> wns({w});
    {
        auto n = ks.size() - 1;
        for (auto i = 0; i < n; ++i)
            wns.push_back(
                2.0 * wns.back()
                / ((1.0 + ks[i + 1])
                   * (1.0 + _arc_jac_sn_complement(ks[i] * wns.back()))));
    }
    std::complex<double> u
        = std::asin(wns.back()) / juce::MathConstants<double>::halfPi;
    return K * u;
}
/** Real inverse Jacobian sc, with complementary modulus */
double _arc_jac_sc1(double w, double m)
{
    auto z = _arc_jac_sn(std::complex<double>(0.0, w), m);
    return (abs(z.real()) > std::numeric_limits<double>::epsilon())
               ? std::nan("_arc_jac_sc1(): z is not imaginary")
               : z.imag();
}

// =============================================================================
EllipticFilterFactory::EllipticFilterFactory() { }
void EllipticFilterFactory::buildAnalogPrototype(FilterParameters& params)
{
    jassert(params.order >= 2);
    jassert(params.order % 2 == 0);

    double eps_2 = pow(10.0, 0.1 * params.passbandRippleDb) - 1.0;
    double eps   = sqrt(eps_2);
    double ck_2  = eps_2 / (pow(10.0, 0.1 * params.stopbandRippleDb) - 1.0);

    double val  = ellpk(1.0 - ck_2);
    double m    = _ellipdeg(params.order, ck_2);
    double capk = ellpk(1.0 - m);

    int jj = params.order / 2;
    std::vector<double> s(jj), c(jj), d(jj), phi(jj);
    for (auto i = 0, j = 1; j < params.order; j += 2, i++)
        ellpj(j * capk / params.order, m, &s[i], &c[i], &d[i], &phi[i]);

    double m_sqrt = sqrt(m), k_z = 1.0, a;
    for (auto& s_i : s)
        if (abs(s_i) > std::numeric_limits<double>::epsilon())
        {
            a = 1.0 / (m_sqrt * s_i);
            params.zpk.zeros.push_back(std::complex(0.0, a));
            a = abs(a);
            k_z *= a * a;
        }

    double r  = _arc_jac_sc1(1.0 / eps, ck_2);
    double v0 = capk * r / (params.order * val);

    double sv, cv, dv, phiv, k_p = 1.0;
    ellpj(v0, 1.0 - m, &sv, &cv, &dv, &phiv);
    for (auto i = 0; i < jj; ++i)
    {
        params.zpk.poles.push_back(
            std::complex(c[i] * d[i] * sv * cv, -s[i] * dv)
            / (pow(d[i] * sv, 2.0) - 1));
        a = abs(params.zpk.poles.back());
        k_p *= a * a;
    }
    params.zpk.gain = sqrt(1.0 + eps_2) * k_p / k_z;
}
