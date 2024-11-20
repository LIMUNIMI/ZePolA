#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

// -----------------------------------------------------------------------------
/*
 The FilterElement class represent a second order digital filter. The filter can
 be a 2-nd order FIR filter, and therefore have only one zero (and its conjugate),
 or a 2-nd order IIR filter. In this case the filter has only one pole (and its
 conjugate) and no zeros and the frequency response has 1 in the numerator.
*/
class FilterElement
{
    public:
    
    enum Type { ZERO, POLE };
    
    FilterElement (Type t, double mg = MAGNITUDE_DEFAULT, double ph = PHASE_DEFAULT) : type(t), magnitude(mg), phase(ph)
    {
        calculateCoefficients();
    }
    
    ~FilterElement () {}
    
    // Returns the magnitude of the zero/pole of the digital filter
    inline double getMagnitude() const { return magnitude; }
    
    // Returns the phase of the zero/pole of the digital filter
    inline double getPhase() const { return phase; }
    
    // Returns the type of the filter (ZERO: FIR, POLE: IIR)
    inline Type getType() const { return type; }
    
    // Returns the current gain of the digital filter
    inline double getGain() const { return gain; }
    
    // Returns true if the filter is active or false if it is not
    inline bool isActive() const { return active; }
    
    // Returns the real part of the digital filter
    double getRealPart ()
    {
        return getMagnitude() * cos(getPhase() * MathConstants<double>::pi);
    }
    
    // Sets the magnitude of the zero/pole of the digital filter
    void setMagnitude (double newValue)
    {
        if (type)
            newValue = jmin(newValue, POLE_MAX_MAGNITUDE);
        
        magnitude = newValue;
        calculateCoefficients();
    }
    
    // Sets the phase of the zero/pole of the digital filter
    void setPhase (double newValue)
    {
        phase = newValue;
        calculateCoefficients();
    }
    
    // Sets the active statuts of the digital filter
    void setUnsetActive (bool newValue)
    {
        active = newValue;
        if (!active)
            memoryReset();
    }
    
    // Sets the type of the digital filter (ZERO: FIR, POLE: IIR)
    void setType (FilterElement::Type newType)
    {
        type = newType;
        memoryReset();
    }
    
    // Sets the gain of the digital filter
    void setGain (double newValue)
    {
        auto linearGain = Decibels::decibelsToGain(newValue, FILTER_ELEMENT_GAIN_FLOOR - 1.0);
        gain = linearGain;
    }
    
    // Resets the memory of the digital filter to the initial state
    void memoryReset ()
    {
        memory1 = 0.0;
        memory2 = 0.0;
    }
    
    // Calculates the frequency response coefficients
    void calculateCoefficients ()
    {
        coeff1 = -2 * getRealPart();
        coeff2 = magnitude * magnitude;
    }
    
    // Calculates the output sample given the input sample
    float processSample (double inputSample)
    {
        double outputSample;
        inputSample *= gain;
        
        switch (type)
        {
            case ZERO:
            {
                outputSample = inputSample + coeff1 * memory1 + coeff2 * memory2;
            } break;
                
            case POLE:
            {
                outputSample = inputSample - coeff1 * memory1 - coeff2 * memory2;
            } break;
        }
        
        updateMemory(inputSample, outputSample);
        return outputSample;
    }
    
    // Calls the processSample method on each sample in the buffer
    void processBlock (double* bufferData, int numSamples)
    {
        for (int smp = 0; smp < numSamples; ++smp)
            bufferData[smp] = processSample(bufferData[smp]);
    }
    
    // Returns the value of the frequency response at the given phi
    std::complex<double> getFrequencyResponseAtPhi (const double phi)
    {
        std::complex<double> z1 = std::polar(1.0, -2 * MathConstants<double>::pi * phi);
        std::complex<double> z2 = std::polar(1.0, -4 * MathConstants<double>::pi * phi);
        
        auto H = 1.0 + coeff1 * z1  + coeff2 * z2;
        
        if (type == POLE)
            H = 1.0 / H;
        
        return std::polar(gain * std::abs(H), std::arg(H));
    }
    
    private:
    Type type;
    
    double magnitude;
    double phase;
    
    double coeff1;
    double coeff2;
    
    double gain = 1.0;
    
    double memory1;
    double memory2;
    
    bool active = false;
    
    // Updates the memory of the digital filter
    void updateMemory (double inputSample, double outputSample)
    {
        memory2 = memory1;
        memory1 = (type == ZERO) ? inputSample : outputSample;
    }
};

// -----------------------------------------------------------------------------
/*
 The PolesAndZerosCascade class represent a digital filter as a chain of 2-nd
 order only FIR or only IIR filters (FilterElement).
 A PolesAndZerosCascade is basically an std::vector<FilterElement>.
*/
class PolesAndZerosCascade
{
    public:
    PolesAndZerosCascade (PolesAndZerosCascade* cascade = nullptr, int nElements = NUMBER_OF_FILTER_ELEMENTS)
    {
        if (cascade == nullptr)
            for (int i = 0; i < nElements; ++ i)
                addElement(FilterElement::ZERO);
        else
        {
            for (int i = 0; i < cascade->elements.size(); ++ i)
            {
                addElement(cascade->getElementType(i + 1));
                elements[i].setMagnitude(cascade->getElementMagnitude(i + 1));
                elements[i].setPhase(cascade->getElementPhase(i + 1));
                elements[i].setUnsetActive(cascade->getElementActiveStatus(i + 1));
            }
        }
    }
    
    ~PolesAndZerosCascade () {}
    
    // Returns the value of the frequency response at the given phi. It is
    // calculated as the multiplication between the spectrum of each 2-nd order
    // filter.
    std::complex<double> getFrequencyResponseAtPhi (const double phi)
    {
        std::complex<double> spectrum(1.0, 0.0);
        
        for (auto& element : elements)
            if (element.isActive())
                spectrum *= element.getFrequencyResponseAtPhi(phi);
        
        return spectrum;
    }
    
    // Returns the cascade
    inline PolesAndZerosCascade* getCascade ()
    {
        return this;
    }
    
    // Returns the cascade as an std::vector of FilterElement
    std::vector<FilterElement>& getElementsChain ()
    {
        return elements;
    }
    
    // Returns the magnitude of the elementNr element in the cascade
    inline double getElementMagnitude(int elementNr) const
    {
        return elements[elementNr - 1].getMagnitude();
    }
    
    // Returns the phase of the elementNr element in the cascade
    inline double getElementPhase(int elementNr) const
    {
        return elements[elementNr - 1].getPhase();
    }
    
    // Returns the type (ZERO: FIR, POLE: IIR) of the elementNr element in the cascade
    inline FilterElement::Type getElementType(int elementNr) const
    {
        return elements[elementNr - 1].getType();
    }
    
    // Returns the active status of the elementNr element in the cascade
    inline bool getElementActiveStatus(int elementNr) const
    {
        return elements[elementNr - 1].isActive();
    }
    
    // Returns the gain of the elementNr element in the cascade
    inline double getElementGain(int elementNr) const
    {
        return elements[elementNr - 1].getGain();
    }
    
    // Returns the elementNr element in the cascade
    inline FilterElement getElement(int elementNr) const
    {
        return elements[elementNr - 1];
    }
    
    // Sets the magnitude of the elementNr element in the cascade
    void setElementMagnitude (const int elementNr, double newValue)
    {
        elements[elementNr - 1].setMagnitude(newValue);
    }

    // Sets the phase of the elementNr element in the cascade
    void setElementPhase (const int elementNr, double newValue)
    {
        elements[elementNr - 1].setPhase(newValue);
    }

    // Sets the active status of the elementNr element in the cascade
    void setUnsetElementActive (const int elementNr, bool newValue)
    {
        elements[elementNr - 1].setUnsetActive(newValue);
    }
    
    // Sets the type (ZERO: FIR, POLE: IIR) of the elementNr element in the cascade
    void setElementType (const int elementNr, bool isZero)
    {
        elements[elementNr - 1].setType(isZero ? FilterElement::ZERO : FilterElement::POLE);
    }
    
    void setElementGain (const int elementNr, double newValue)
    {
        elements[elementNr - 1].setGain(newValue);
    }
    
    // Resets the memory of each element in the cascade
    void memoryReset ()
    {
        for (auto& element : elements)
            element.memoryReset();
    }
    
    // Adds a new element in the cascade
    void addElement (FilterElement::Type type)
    {
        elements.push_back(FilterElement(type));
    }

    // Process a set of samples by calling the process block method of each element
    void processBlock (double* bufferData, const int numSamples)
    {
        for (auto& element : elements)
            if (element.isActive())
                element.processBlock(bufferData, numSamples);
    }
    
    private:
    std::vector<FilterElement> elements;
};
