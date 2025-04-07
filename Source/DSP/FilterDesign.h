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
        double gain;

        // =====================================================================
        void reset();

        // =====================================================================
        /** Number of element conjugate pairs (poles + zeros) / 2 */
        size_t nElements() const;
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

        // This should not be used as a type, but only to get how many types
        // there are
        N_FILTER_TYPES
    };
    /** Encode filter type as a string */
    static juce::String typeToString(FilterType);

    // =========================================================================
    enum FilterShape
    {
        LowPass = 0,
        HighPass,

        // This should not be used as a shape, but only to get how many
        // shapes there are
        N_FILTER_SHAPES
    };
    /** Encode filter shape as a string */
    static juce::String shapeToString(FilterShape);

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
    FilterShape shape;
    int order;
    double cutoff;
    double passbandRippleDb;
    double stopbandRippleDb;
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
/** Class for digital filters derived from analog filters */
class AnalogFilterFactory : public FilterFactory
{
public:
    // =========================================================================
    AnalogFilterFactory();

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
