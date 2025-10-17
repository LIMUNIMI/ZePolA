/*
  ==============================================================================

    FilterDesign.h

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

#pragma once
#include "Filter.h"
#include <JuceHeader.h>
#include <complex>

// =============================================================================
/** Parameters for filter design */
class FilterParameters
{
public:
    // =========================================================================
    /** Poles, zeros, gain parameters of filters */
    class ZPK
    {
    public:
        // =====================================================================
        ZPK();

        // =====================================================================
        std::vector<std::complex<double>> zeros, poles;
        std::vector<bool> single_zeros, single_poles;
        double gain;

        // =====================================================================
        void pushZero(std::complex<double>, bool is_single = false);
        void pushPole(std::complex<double>, bool is_single = false);
        void reset();

        // =====================================================================
        /** Number of elements */
        size_t nElements() const;
        /** Poles degree */
        int poleDegree() const;
        /** Zeros degree */
        int zeroDegree() const;
        /** Relative degree (poles - zeros) */
        int relativeDegree() const;
        /** Degree max(poles, zeros) */
        size_t degree() const;

    private:
        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZPK)
    };

    // =========================================================================
    enum FilterType
    {
        Butterworth = 0,
        ChebyshevI,
        ChebyshevII,
        Elliptic,
        Biquad,

        // This should not be used as a type, but only to get how many types
        // there are
        N_FILTER_TYPES
    };
    /** Encode filter type as a string */
    static juce::String typeToString(FilterType);

    // =========================================================================
    enum AnalogFilterShape
    {
        AnalogLowPass = 0,
        AnalogHighPass,

        // This should not be used as a shape, but only to get how many
        // shapes there are
        N_ANALOG_FILTER_SHAPES
    };
    /** Encode filter shape as a string */
    static juce::String shapeToString(AnalogFilterShape);

    // =========================================================================
    enum BiquadFilterShape
    {
        BiquadLowPass = 0,
        BiquadHighPass,
        BiquadNotch,
        BiquadAllPass,
        BiquadPeaking,
        BiquadLowShelf1,
        BiquadHighShelf1,
        BiquadLowShelf2,
        BiquadHighShelf2,
        BiquadBandPass1,
        BiquadBandPass2,

        // This should not be used as a shape, but only to get how many
        // shapes there are
        N_BIQUAD_FILTER_SHAPES
    };
    /** Encode filter shape as a string */
    static juce::String shapeToString(BiquadFilterShape);

    // =========================================================================
    FilterParameters(double sr = 1.0);

    // =========================================================================
    /**  */
    void computeZPK();
    /** Get the normalized frequency between 0 (DC) and 1 (Nyquist) */
    double normalizedFrequency() const;
    /** Get the warped frequency for the conversion from analog filter prototype
     */
    double warpedFrequency() const;

    // =========================================================================
    double sr;
    FilterType type;
    AnalogFilterShape analogFShape;
    BiquadFilterShape biquadFShape;
    int order;
    double cutoff;
    double cutoff2;
    double passbandRippleDb;
    double stopbandRippleDb;
    double quality;
    double gain_db;
    ZPK zpk;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterParameters)
};

// =============================================================================
/** Factory class for filters */
class FilterFactory
{
public:
    // =========================================================================
    FilterFactory();
    virtual ~FilterFactory();

    // =========================================================================
    /** Compute a filter's zeros, poles and gain. Zeros and poles should be
     * paired with their conjugates */
    virtual void build(FilterParameters&);
    /** Clean filter parameters */
    virtual void sanitizeParams(FilterParameters&);

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterFactory)
};

// =============================================================================
/** Abstract base class for digital filters derived from analog filters */
class AnalogFilterFactory : public FilterFactory
{
public:
    // =========================================================================
    AnalogFilterFactory();
    virtual ~AnalogFilterFactory();

    // =========================================================================
    /** Build an analog filter prototype low-pass filter */
    virtual void buildAnalogPrototype(FilterParameters&) = 0;
    /** Apply cutoff frequency and type to the filter prototype */
    void applyParamsToPrototype(FilterParameters&);
    /** Apply lowpass cutoff frequency and type to the filter prototype */
    void applyLowPassParamsToPrototype(FilterParameters&);
    /** Apply hipass cutoff frequency and type to the filter prototype */
    void applyHighPassParamsToPrototype(FilterParameters&);
    /** Apply a bilinear transform to convert an analog filter into a digital
     * one */
    void bilinearTransform(FilterParameters::ZPK&);
    virtual void build(FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogFilterFactory)
};

// =============================================================================
/** Factory class for Butterworth filters */
class ButterworthFilterFactory : public AnalogFilterFactory
{
public:
    // =========================================================================
    ButterworthFilterFactory();
    virtual ~ButterworthFilterFactory();

    // =========================================================================
    virtual void buildAnalogPrototype(FilterParameters&) override;

private:
    // =========================================================================
    // JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButterworthFilterFactory)
    JUCE_DECLARE_NON_COPYABLE(ButterworthFilterFactory)
    JUCE_HEAVYWEIGHT_LEAK_DETECTOR(ButterworthFilterFactory)
};

// =============================================================================
/** Factory class for Chebyshev type I filters */
class ChebyshevIFilterFactory : public AnalogFilterFactory
{
public:
    // =========================================================================
    ChebyshevIFilterFactory();
    virtual ~ChebyshevIFilterFactory();

    // =========================================================================
    virtual void buildAnalogPrototype(FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChebyshevIFilterFactory)
};

// =============================================================================
/** Factory class for Chebyshev type II filters */
class ChebyshevIIFilterFactory : public AnalogFilterFactory
{
public:
    // =========================================================================
    ChebyshevIIFilterFactory();
    virtual ~ChebyshevIIFilterFactory();

    // =========================================================================
    virtual void buildAnalogPrototype(FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChebyshevIIFilterFactory)
};

// =============================================================================
/** Factory class for Elliptic filters */
class EllipticFilterFactory : public AnalogFilterFactory
{
public:
    // =========================================================================
    EllipticFilterFactory();

    // =========================================================================
    virtual void buildAnalogPrototype(FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EllipticFilterFactory)
};

// =============================================================================
/** Abstract base class for digital biquad filters */
class BiquadFilterFactory : public FilterFactory
{
public:
    // =========================================================================
    BiquadFilterFactory();

    // =========================================================================
    static std::unique_ptr<BiquadFilterFactory>
    buildFactory(const FilterParameters&);

    // =========================================================================
    /** Compute the biquad filter coefficients */
    virtual std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                                      double alpha, double gain,
                                                      const FilterParameters&);
    /** Solve quadratic equation */
    template <typename FloatType>
    static std::array<std::complex<FloatType>, 2>
    solveQuadratic(std::complex<FloatType> a, std::complex<FloatType> b,
                   std::complex<FloatType> c);
    /** Solve quadratic equation with real coefficients */
    template <typename FloatType>
    static std::array<std::complex<FloatType>, 2>
    solveRealQuadratic(FloatType a, FloatType b, FloatType c);
    virtual void build(FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BiquadFilterFactory)
};

// =============================================================================
/** Factory class for lowpass biquad filters */
class LowPassBiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    LowPassBiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowPassBiquadFilterFactory)
};

// =============================================================================
/** Factory class for highpass biquad filters */
class HighPassBiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    HighPassBiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighPassBiquadFilterFactory)
};

// =============================================================================
/** Factory class for notch biquad filters */
class NotchBiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    NotchBiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotchBiquadFilterFactory)
};

// =============================================================================
/** Factory class for allpass biquad filters */
class AllPassBiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    AllPassBiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AllPassBiquadFilterFactory)
};

// =============================================================================
/** Factory class for peak (bell) biquad filters */
class PeakingBiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    PeakingBiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PeakingBiquadFilterFactory)
};

// =============================================================================
/** Factory class for low-shelf biquad filters */
class LowShelf1BiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    LowShelf1BiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowShelf1BiquadFilterFactory)
};

// =============================================================================
/** Factory class for high-shelf biquad filters */
class HighShelf1BiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    HighShelf1BiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighShelf1BiquadFilterFactory)
};

// =============================================================================
/** Factory class for resonant low-shelf biquad filters */
class LowShelf2BiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    LowShelf2BiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowShelf2BiquadFilterFactory)
};

// =============================================================================
/** Factory class for resonant high-shelf biquad filters */
class HighShelf2BiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    HighShelf2BiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighShelf2BiquadFilterFactory)
};

// =============================================================================
/** Factory class for bandpass biquad filters */
class BandPass1BiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    BandPass1BiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandPass1BiquadFilterFactory)
};

// =============================================================================
/** Factory class for resonant bandpass biquad filters */
class BandPass2BiquadFilterFactory : public BiquadFilterFactory
{
public:
    // =========================================================================
    BandPass2BiquadFilterFactory();

    // =========================================================================
    std::array<double, 7> computeBiquadCoeffs(double sn, double cs,
                                              double alpha, double gain,
                                              const FilterParameters&) override;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandPass2BiquadFilterFactory)
};
