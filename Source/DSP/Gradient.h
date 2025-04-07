/*
  ==============================================================================

    Gradient.h

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
#include <cmath>
#include <complex>
#include <functional>
#include <math.h>

// =============================================================================
/** Class for derivative forward propagation */
template <typename ValueType>
class DualValue
{
public:
    // =========================================================================
    DualValue(ValueType x, ValueType dx);

    // =========================================================================
    /** Instantiate a constant value */
    static DualValue<ValueType> constant(ValueType);
    /** Instantiate an input variable */
    static DualValue<ValueType> variable(ValueType);

    // =========================================================================
    DualValue operator+(const DualValue&) const;
    DualValue operator+(const ValueType&) const;
    template <typename OtherType>
    DualValue operator+(const OtherType&) const;
    DualValue operator-(const DualValue&) const;
    DualValue operator-(const ValueType&) const;
    template <typename OtherType>
    DualValue operator-(const OtherType&) const;
    DualValue operator-() const;
    DualValue operator*(const DualValue&) const;
    DualValue operator*(const ValueType&) const;
    template <typename OtherType>
    DualValue operator*(const OtherType&) const;
    DualValue operator/(const DualValue&) const;
    DualValue operator/(const ValueType&) const;
    template <typename OtherType>
    DualValue operator/(const OtherType&) const;

    // =========================================================================
    /** Get the function value */
    ValueType getValue() const;
    /** Get the function derivative */
    ValueType getDerivative() const;

private:
    // =========================================================================
    ValueType x, dx;
};

// =============================================================================
template <typename ValueType>
DualValue<ValueType> exp(const DualValue<ValueType>&);
template <typename ValueType>
DualValue<ValueType> pow(const DualValue<ValueType>&, const ValueType&);
template <typename FloatType>
DualValue<std::complex<FloatType>>
pow(const DualValue<std::complex<FloatType>>&, const FloatType&);
template <typename ValueType>
DualValue<ValueType> real(const DualValue<ValueType>&);
template <typename FloatType>
DualValue<FloatType> real(const DualValue<std::complex<FloatType>>&);
template <typename ValueType>
DualValue<ValueType> imag(const DualValue<ValueType>&);
template <typename FloatType>
DualValue<FloatType> imag(const DualValue<std::complex<FloatType>>&);
namespace std
{
template <typename ValueType>
DualValue<ValueType> conj(const DualValue<ValueType>&);
template <typename FloatType>
DualValue<std::complex<FloatType>>
conj(const DualValue<std::complex<FloatType>>&);
template <typename ValueType>
DualValue<ValueType> norm(const DualValue<ValueType>&);
template <typename FloatType>
DualValue<FloatType> norm(const DualValue<std::complex<FloatType>>&);
};  // namespace std

// =============================================================================
class GradientAscent
{
public:
    // =========================================================================
    GradientAscent(double eta = 5e-3, double th = 1e-4, double theta = 0.5);

    // =========================================================================
    /** Run the gradient ascent and return the minimal argument */
    double operator()(double,
                      std::function<DualValue<double>(DualValue<double>)>,
                      int max_iters = 1 << 7);

private:
    // =========================================================================
    double eta, th, theta, theta_c;
};

// =============================================================================
/** Wrapper for optimizing over a FilterElementCascade& */
class DifferentiableDTFT
{
public:
    // =========================================================================
    DifferentiableDTFT(const FilterElementCascade&);

    // =========================================================================
    /** Compute the DTFT square magnitude */
    template <typename ValueType>
    ValueType forward(const ValueType&);
    /**
     * Find the square magnitude peak value and frequency of the DTFT
     *
     * @return std::array<double, 2> Frequency, square magnitude
     */
    std::array<double, 2> peakFrequency();

private:
    // =========================================================================
    std::vector<std::array<double, 2>> coeffs;
    std::vector<double> gains, angles;
    std::vector<bool> isPole;
};
