#pragma once
#include "Parameters.h"
#include <JuceHeader.h>

// =============================================================================
/**
 * @brief Second order digital filter
 *
 * The filter can be a 2-nd order FIR filter, and therefore have only one zero
 * (and its conjugate), or a 2-nd order IIR filter, and therefore have only one
 * pole (and its conjugate)
 */
class FilterElement
{
public:
    // =========================================================================
    /**
     * @brief Filter element type
     *
     *  - ZERO: 2-zero filter
     *  - POLE: 2-pole filter
     */
    enum Type
    {
        ZERO,
        POLE
    };

    // =========================================================================
    /**
     * @brief Construct a new Filter Element
     *
     * By default, it is a 2-zero filter with both zeros in 0
     */
    FilterElement();
    // Copy constructor
    FilterElement(const FilterElement&);

    // =========================================================================
    // Returns the magnitude of the zero/pole of the digital filter
    double getMagnitude() const;
    // Returns the normalized phase of the zero/pole of the digital filter
    double getPhase() const;
    // Returns the angle (non-normalized) of the zero/pole of the digital filter
    double getAngle() const;
    // Returns the real part of the zero/pole of the digital filter
    double getRealPart() const;
    // Returns the imaginary part of the zero/pole of the digital filter
    double getImagPart() const;
    // Returns the type of the digital filter (pole/zero)
    Type getType() const;
    // Returns the input gain (linear) of the digital filter
    double getGain() const;
    // Returns the input gain (in decibel) of the digital filter
    double getGainDb() const;
    // Returns true if the filter is active (or false if it is not)
    bool getActive() const;
    // Returns an array with the current gain and coefficients of the filter
    std::array<double, 3> getCoefficients() const;

    // =========================================================================
    // Resets the memory of the digital filter to the initial state
    void resetMemory();
    // Sets the magnitude of the zero/pole of the digital filter
    void setMagnitude(double);
    // Sets the normalized phase of the zero/pole of the digital filter
    void setPhase(double);
    // Sets the type of the digital filter (pole/zero)
    void setType(Type);
    // Sets the input gain (linear) of the digital filter
    void setGain(double);
    // Sets the input gain (in decibel) of the digital filter
    void setGainDb(double);
    // Sets the active state of the digital filter
    void setActive(bool b = true);
    // Sets the digital filter to be inactive
    void setInactive();

    // =========================================================================
    /**
     * @brief Computes the output samples for an input array of samples
     *
     * @param outputs Output buffer
     * @param inputs Input buffer (can be the same as output buffer)
     * @param n Buffer size. Both pointers should safe to be accessed between 0
     * and n - 1
     */
    void processBlock(double* outputs, double* inputs, int n);
    /**
     * @brief Computes the DTFT of the digital filter
     *
     * @param Ω Digital frequency, where 0 is the DC and 2π is the sample rate
     * @return DTFT at digital frequency Ω
     */
    std::complex<double> dtft(double) const;

private:
    // =========================================================================
    // Computes the digital filter time-domain coefficients
    void computeCoefficients();
    // Pushes a sample in the memory queue
    void pushSample(double);

    // =========================================================================
    // Zero sample processing function
    double processSampleZero(double);
    // Pole sample processing function
    double processSamplePole(double);

    // =========================================================================
    // Minimum input gain allowed
    static const double gain_floor_db;
    // Maximum magnitude allowed for poles
    static const double pole_magnitude_ceil;

    // =========================================================================
    double magnitude, phase, gain, coeffs[2], memory[2];
    bool active;
    Type type;

    double (FilterElement::*processSampleFunc)(double);

    // =========================================================================
    JUCE_LEAK_DETECTOR(FilterElement);
};

// =============================================================================
/**
 * @brief A filter made by 2-pole or 2-zero filters in series
 *
 * The components of this filter are implemented by the FilterElement class
 */
class FilterElementCascade
{
public:
    // =========================================================================
    // Construct a new Filter Element Cascade with n elements
    FilterElementCascade(int n);
    // Copy constructor
    FilterElementCascade(const FilterElementCascade&);

    // =========================================================================
    // Adds a new element at the end of the cascade
    void addElement();
    // Adds a copy of the provided element at the end of the cascade
    void addElement(const FilterElement&);

    // =========================================================================
    // Resets the memory of the digital filter to the initial state
    void resetMemory();

    // =========================================================================
    // Number of elements in the chain
    size_t size() const;
    // Access element of the chain
    FilterElement& operator[](size_t);
    // Returns the cascade as a vector of FilterElement
    [[deprecated("Use accessors instead.")]] std::vector<FilterElement>&
    getElementsChain();

    // =========================================================================
    /**
     * @brief Computes the DTFT of the digital filter
     *
     * @param Ω Digital frequency, where 0 is the DC and 2π is the sample rate
     * @return DTFT at digital frequency Ω
     */
    std::complex<double> dtft(double) const;

    inline std::vector<double> getCoefficients() const
    {
        std::vector<double> coefficients;
        for (auto& element : elements)
        {
            auto coeffs = element.getCoefficients();
            for (auto& c : coeffs) coefficients.push_back(c);
        }

        return coefficients;
    }

    // =========================================================================
    /**
     * @brief Computes the output samples for an input array of samples
     *
     * @param outputs Output buffer
     * @param inputs Input buffer (can be the same as output buffer)
     * @param n Buffer size. Both pointers should safe to be accessed between 0
     * and n - 1
     */
    void processBlock(double* outputs, double* inputs, int n);

private:
    // =========================================================================
    std::vector<FilterElement> elements;

    // =========================================================================
    JUCE_LEAK_DETECTOR(FilterElement);
};
