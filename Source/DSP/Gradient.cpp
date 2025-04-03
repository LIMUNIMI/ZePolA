#include "Gradient.h"
#include <JuceHeader.h>

// =============================================================================
template <typename ValueType>
DualValue<ValueType>::DualValue(ValueType x, ValueType dx) : x(x), dx(dx)
{
}
template <typename ValueType>
DualValue<ValueType> DualValue<ValueType>::constant(ValueType c)
{
    return {c, static_cast<ValueType>(0)};
}
template <typename ValueType>
DualValue<ValueType> DualValue<ValueType>::variable(ValueType v)
{
    return {v, static_cast<ValueType>(1)};
}

// =============================================================================
template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator+(const DualValue& other) const
{
    return {x + other.x, dx + other.dx};
}
template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator+(const ValueType& other) const
{
    return {x + other, dx};
}
template <typename ValueType>
template <typename OtherType>
DualValue<ValueType>
DualValue<ValueType>::operator+(const OtherType& other) const
{
    return *this + static_cast<ValueType>(other);
}

template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator-(const DualValue& other) const
{
    return {x - other.x, dx - other.dx};
}
template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator-(const ValueType& other) const
{
    return {x - other, dx};
}
template <typename ValueType>
template <typename OtherType>
DualValue<ValueType>
DualValue<ValueType>::operator-(const OtherType& other) const
{
    return *this - static_cast<ValueType>(other);
}
template <typename ValueType>
DualValue<ValueType> DualValue<ValueType>::operator-() const
{
    return {-x, -dx};
}

template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator*(const DualValue& other) const
{
    return {x * other.x, dx * other.x + x * other.dx};
}
template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator*(const ValueType& other) const
{
    return {x * other, dx * other};
}
template <typename ValueType>
template <typename OtherType>
DualValue<ValueType>
DualValue<ValueType>::operator*(const OtherType& other) const
{
    return *this * static_cast<ValueType>(other);
}

template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator/(const DualValue& other) const
{
    return {x / other.x, (dx * other.x + x * other.dx) / (other.x * other.x)};
}
template <typename ValueType>
DualValue<ValueType>
DualValue<ValueType>::operator/(const ValueType& other) const
{
    return {x / other, dx / other};
}
template <typename ValueType>
template <typename OtherType>
DualValue<ValueType>
DualValue<ValueType>::operator/(const OtherType& other) const
{
    return *this / static_cast<ValueType>(other);
}

// =============================================================================
template <typename ValueType>
ValueType DualValue<ValueType>::getValue() const
{
    return x;
}
template <typename ValueType>
ValueType DualValue<ValueType>::getDerivative() const
{
    return dx;
}

// =============================================================================
template <typename ValueType>
DualValue<ValueType> exp(const DualValue<ValueType>& x)
{
    auto e = exp(x.getValue());
    return {e, e * x.getDerivative()};
}
template <typename ValueType>
DualValue<ValueType> pow(const DualValue<ValueType>& x, const ValueType& p)
{
    return {pow(x.getValue(), p),
            p * pow(x.getValue(), p - static_cast<ValueType>(1))
                * x.getDerivative()};
}
template <typename FloatType>
DualValue<std::complex<FloatType>>
pow(const DualValue<std::complex<FloatType>>& z, const FloatType& p)
{
    return {pow(z.getValue(), p),
            p * pow(z.getValue(), p - static_cast<FloatType>(1))
                * z.getDerivative()};
}
template <typename ValueType>
DualValue<ValueType> real(const DualValue<ValueType>& x)
{
    return x;
}
template <typename FloatType>
DualValue<FloatType> real(const DualValue<std::complex<FloatType>>& z)
{
    return {real(z.getValue()), real(z.getDerivative())};
}
template <typename ValueType>
DualValue<ValueType> imag(const DualValue<ValueType>& x)
{
    return DualValue<ValueType>::constant(static_cast<ValueType>(0));
}
template <typename FloatType>
DualValue<FloatType> imag(const DualValue<std::complex<FloatType>>& z)
{
    return {imag(z.getValue()), imag(z.getDerivative())};
}
template <typename ValueType>
DualValue<ValueType> std::conj(const DualValue<ValueType>& x)
{
    return x;
}
template <typename FloatType>
DualValue<std::complex<FloatType>>
std::conj(const DualValue<std::complex<FloatType>>& z)
{
    return {std::conj(z.getValue()), std::conj(z.getDerivative())};
}
template <typename ValueType>
DualValue<ValueType> std::norm(const DualValue<ValueType>& x)
{
    return x * x;
}
template <typename FloatType>
DualValue<FloatType> std::norm(const DualValue<std::complex<FloatType>>& z)
{
    return real(z * std::conj(z));
}

// =============================================================================
GradientAscent::GradientAscent(double eta, double th, double theta)
    : eta(eta), th(th), theta(theta), theta_c(1.0 - theta)
{
}
double GradientAscent::operator()(
    double x, std::function<DualValue<double>(DualValue<double>)> f,
    int max_iters)
{
    auto x_i  = DualValue<double>::variable(x);
    double mu = 0.0, ms = 0.0, step, g;
    for (int i = 0; i < max_iters; ++i)
    {
        g  = f(x_i).getDerivative();
        mu = theta * mu + theta_c * g;
        ms = theta * ms + theta_c * (g * g);
        if (ms < th) break;
        step = eta * g * abs(mu) / sqrt(ms);
        x_i  = x_i + step;
        if (abs(step) < th) break;
    }
    return x_i.getValue();
}

// =============================================================================
DifferentiableDTFT::DifferentiableDTFT(const FilterElementCascade& fec)
{
    auto n = fec.size();
    // std::vector<double> angs;
    for (auto i = 0; i < n; ++i)
        if (fec[i].getActive())
        {
            elements.push_back(
                std::polar<double>(fec[i].getMagnitude(), fec[i].getAngle()));
            isPole.push_back(fec[i].getType());
            gains.push_back(fec[i].getGain());
            angles.push_back(fec[i].getAngle());
        }
    jassert(elements.size() <= n);
    n = elements.size();
    jassert(isPole.size() == n);
    jassert(angles.size() == n);
    jassert(gains.size() == n);

    std::sort(angles.begin(), angles.begin() + angles.size());
    angles.push_back(juce::MathConstants<double>::pi);
    angles.insert(angles.begin(), 0.0);
    jassert(angles.size() == n + 2);
}

// =============================================================================
namespace DualValueCasts
{
/** Cast a variable to the same type of a DualValue variable */
template <typename InputType, typename ValueType>
static ValueType asValueType(const InputType& f, const DualValue<ValueType>&)
{
    return static_cast<ValueType>(f);
};
/** Cast a variable to the same type of another variable */
template <typename InputType, typename ValueType>
static ValueType asValueType(const InputType& f, const ValueType&)
{
    return static_cast<ValueType>(f);
};
/** Cast a variable to a DualValue constant */
template <typename InputType, typename ValueType>
static DualValue<ValueType> asDualValue(const InputType& f,
                                        const DualValue<ValueType>& v)
{
    return DualValue<ValueType>::constant(asValueType(f, v));
};
/** Cast a variable to the same type of another variable */
template <typename InputType, typename ValueType>
static ValueType asDualValue(const InputType& f, const ValueType& v)
{
    return asValueType(f, v);
};
/** Cast a DualValue to a complex DualValue on the imaginary axis */
template <typename FloatType>
static DualValue<std::complex<FloatType>>
asComplexImag(const DualValue<FloatType>& f)
{
    return {
        std::complex<FloatType>(static_cast<FloatType>(0), f.getValue()),
        std::complex<FloatType>(static_cast<FloatType>(0), f.getDerivative()),
    };
};
/** Cast a variable to a complex variable on the imaginary axis */
template <typename FloatType>
static std::complex<FloatType> asComplexImag(const FloatType& f)
{
    return {static_cast<FloatType>(0), f};
};
};  // namespace DualValueCasts

// =============================================================================
template <typename ZeroType, typename ValueType>
static ValueType zeroConjZT(const ZeroType& zero, const ValueType& z_pm1)
{
    auto one = DualValueCasts::asValueType(1, z_pm1);
    return (z_pm1 * zero - one) * (z_pm1 * std::conj(zero) - one);
}

template <typename ValueType>
ValueType DifferentiableDTFT::forward(const ValueType& w)
{
    auto z_pm1 = exp(DualValueCasts::asComplexImag(-w));
    auto h     = DualValueCasts::asDualValue(1, z_pm1);
    auto n     = elements.size();
    for (auto i = 0; i < n; ++i)
    {
        auto h_i = zeroConjZT(elements[i], z_pm1);
        h        = (isPole[i]) ? h / h_i : h * h_i;
        h        = h * gains[i];
    }
    return std::norm(h);
}
std::array<double, 2> DifferentiableDTFT::peakFrequency()
{
    double w = 0.0, h2 = 0.0;
    GradientAscent gd;
    for (auto a : angles)
    {
        auto w_a
            = gd(a, std::bind(&DifferentiableDTFT::forward<DualValue<double>>,
                              this, std::placeholders::_1));
        auto h2_w_a = forward(w_a);
        jassert(h2_w_a >= 0.0);
        if (h2_w_a > h2)
        {
            h2 = h2_w_a;
            w  = w_a;
        }
    }
    return {w, h2};
}
