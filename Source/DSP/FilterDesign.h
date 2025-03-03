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
