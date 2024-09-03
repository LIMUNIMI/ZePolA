#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include <vector>
#include <memory>

// -----------------------------------------------------------------------------
/* The class "FilterElement" represents a complex number in magnitude-phase
 form. A ZERO is a value on the complex plane for which the transfer function of
 a digital filter becomes zero. A POLE is a value on the complex plane for which
 the transfer function of a digital filter becomes infinite. In general, the
 transfer function of an IIR filter is represented as a rational polynomial: the
 zeros are the roots of the numerator (FIR part of the filter) while the poles
 are the roots of the denominator (IIR part of the filter).
 The approach for creating the FilterElement class is to break down the chain of
 poles and zeros of a digital filter of order M into individual filters with
 either a single zero or a single pole in cascade and to represent the smallest
 component of a digital filter.
 Each FilterElement consists of the following parameters: magnitude (modulus),
 phase, coeff1, and coeff2 for the coefficients of the difference equation and
 for calculating the output sample, along with two double variables for the
 filter memory: the previous sample and the sample from two instances ago (input
 samples if it is a ZERO and output samples if it is a POLE).
*/
class FilterElement
{
public:
    
    enum Type { ZERO, POLE };

    /* The constructor of FilterElement requires the specification of the
     element type (ZERO or POLE), while magnitude and phase have default values:
     0.0 for both.
    */
    FilterElement (Type t, double mg = 0.0, double ph = 0.0) : type(t), magnitude(mg), phase(ph)
    {
        calculateCoefficients();
    }
    
    ~FilterElement () {}
    
    /* The method setMagnitude sets the magnitude of the complex number to the
     value of newValue and calls the calculateCoefficients method to compute
     the new coefficients for the difference equation.
    */
    void setMagnitude (double newValue)
    {
        magnitude = newValue;
        calculateCoefficients();
    }

    /* The method setPhase sets the phase of the complex number to the value of
     newValue and calls the calculateCoefficients method to compute the new
     coefficients for the difference equation.
    */
    void setPhase (double newValue)
    {
        phase = newValue;
        calculateCoefficients();
    }

    /* The method getMagnitude returns the magnitude of the complex number
     represented by FilterElement.
    */
    double getMagnitude ()
    {
        return magnitude;
    }

    /* The method getPhase returns the phase of the complex number represented
    by FilterElement.
    */
    double getPhase ()
    {
        return phase;
    }

    /* The method getRealPart returns the real part of the complex number
    represented by FilterElement.
    */
    double getRealPart ()
    {
        return getMagnitude() * cos(getPhase() * MathConstants<double>::pi);
    }

    // The method getType returns the type of the FilterElement.
    enum Type getType ()
    {
        return type;
    }

    /* The memoryReset method resets the memory of the FilterElement (whether it
    is a ZERO or a POLE) to zero.
    */
    void memoryReset ()
    {
        memory1 = 0.0;
        memory2 = 0.0;
    }

    /* Calculates the coefficients of the FilterElement with the current
    pole/zero and its conjugate.
    */
    void calculateCoefficients ()
    {
        coeff1 = -2 * getRealPart();
        coeff2 = magnitude * magnitude;
    }

    /* The processSample method is called for each sample in the incoming buffer
     and processes an input sample (the current instant) by calculating the
     output sample. The processing differs depending on whether it is a ZERO
     element or a POLE element. The ZERO element calculates only the FIR
     (non-recursive) part of the difference equation: the output sample from the
     ZERO element is calculated as the sum of the input sample, the input sample
     from one instant ago (multiplied by the coefficient coeff1), and the input
     sample from two instants ago (multiplied by the coefficient coeff2). One
     instant ago and two instants ago refer to the previous sample and the
     sample before that, respectively. The POLE element calculates only the IIR
     (recursive) part of the difference equation: the output sample from the
     POLE element is calculated as the difference between the input sample, the
     output sample from one instant ago (multiplied by the coefficient coeff1),
     and the output sample from two instants ago (multiplied by the coefficient
     coeff2). The memory of the FilterElement is then updated to have the
     necessary information available for the calculation of the next sample
     during the subsequent call to the processSample method.
    */
    float processSample (double inputSample)
    {
        double outputSample;
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
    
    /* The processBlock method takes the entire buffer of audio samples as input
     and calls the processSample method for each sample in the buffer.
    */
    void processBlock (juce::AudioBuffer<double>& buffer, int numSamples)
    {
        auto bufferData = buffer.getArrayOfWritePointers();
        
        for (int smp = 0; smp < numSamples; ++smp)
                bufferData[0][smp] = processSample(bufferData[0][smp]);
    }

    /* The updateMemory method updates the filter's memory. memory1 stores the
     previous sample, which is an input sample if it is a ZERO or an output
     sample if it is a POLE. memory2 stores the sample from two instances ago,
     which is an input sample if it is a ZERO or an output sample if it is a 
     POLE.
    */
    void updateMemory (double inputSample, double outputSample)
    {
        memory2 = memory1;
        switch (type)
        {
            case ZERO:
            {
                memory1 = inputSample;
            } break;
                
            case POLE:
            {
                memory1 = outputSample;
            }
        }
    }
    
private:
    Type type;
    
    double magnitude;
    double phase;
    
    double coeff1;
    double coeff2;

    double memory1;
    double memory2;
};

// -----------------------------------------------------------------------------
/* The "PolesAndZerosCascade" class represents a digital filter as a chain of 
 poles and zeros (simple filters). The approach of the class is to conceive an
 IIR digital filter as a chain of simple digital filters, each composed of
 either a single zero (FIR filter) or a single pole (IIR filter).
 A PolesAndZerosCascade is composed of the following parameters: a std::vector
 of pointers to objects of type FilterElement (poles or zeros) to dynamically
 allocate the filter components and arbitrarily decide how many poles and how
 many zeros to associate with the filter, and a boolean to manage the filter's
 bypass. The list of FilterElement pointers follows a LIFO (last in - first out)
 logic. The most recently added element to the filter is the first to be removed
 upon the user's request.
 The private method countElementsOfType counts the elements of the specified 
 type (POLE or ZERO) in the call.
 */
class PolesAndZerosCascade
{
public:

    /* The constructor of PolesAndZerosCascade has default values for the number
     of zeros and poles that the filter should have at the time of creation.
    */
    PolesAndZerosCascade (int nZeros = 1, int nPoles = 1)
    {
        for (int i = 0; i < nZeros; ++ i)
            addElement(FilterElement::ZERO);
        
        for (int i = 0; i < nPoles; ++ i)
            addElement(FilterElement::POLE);
    }
    
    ~PolesAndZerosCascade () {}
    
    /* The memoryReset method resets the memory of the entire filter by calling
     the reset method for all the elements that make up the filter's cascade.
     */
    void memoryReset ()
    {
        for (auto& node : elements)
            node->memoryReset();
    }
    
    /* The setUnsetBypass method bypasses the entire filter depending on the
     value of newValue chosen by the user as a parameter of the plugin.
    */
    void setUnsetBypass (bool newValue)
    {
        active = !newValue;
        if (!active)
            memoryReset();
    }
    
    /* The addElement method adds an element of the specified type to the
     cascade of zero or pole elements. The addition occurs at the end of the
     list of pointed elements.
    */
    void addElement (FilterElement::Type type)
    {
        if (countElementsOfType(type) < MAX_ORDER)
            elements.push_back(std::make_unique<FilterElement>(type));
        else
        {
            DBG(" You are trying to add a zero or a pole even if you have reached the maximum number of poles or zeros!");
            jassertfalse; // You are trying to add a zero or a pole even if you have reached the maximum number of poles or zeros!
        }
    }
    
    /* The removeElement method removes an element of the specified type from
     the cascade of zero or pole elements. The most recently added element that
     matches the specified type is the one that gets removed.
    */
    void removeElement (FilterElement::Type type)
    {
        auto iterator = std::find_if(elements.rbegin(), elements.rend(),
                                   [type](const std::unique_ptr<FilterElement>& element) { return element->getType() == type; });
        // find_if ritorna un iteratore all'ultimo elemento che corrisponde al criterio di ricerca (type da eliminare)
        if (iterator != elements.rend())
            elements.erase(std::next(iterator).base());
        else
        {
            DBG("No element of the specified type found!");
            jassertfalse;
        }
    }

    /* The castBuffer method in the PolesAndZerosCascade class is a template
     function that converts and copies audio data between two buffers of
     potentially different data types.
     */
    template <typename TargetType, typename SourceType>
    void castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples)
    {
        auto dst = destination.getArrayOfWritePointers();
        auto src = source.getArrayOfReadPointers();

        for (int ch = 0; ch < numChannels; ++ch)
            for (int smp = 0; smp < numSamples; ++smp)
                dst[ch][smp] = static_cast<TargetType>(src[ch][smp]);
    }
    
    /* The processBlock method receives a reference to the audio buffer and is
     responsible for processing the audio. The general mechanism involves
     calling the individual processBlock method for each element in the filter
     chain. Specifically, the method calculates numSamples (the number of
     samples to be processed) and referenceRMS (the RMS volume to normalize the
     output from the filter), and creates a copy of the input buffer in double
     format to increase precision in the filter calculation. The processBlock of
     each element in the cascade is then called. Finally, the buffer is re-cast
     to float, and the output RMS volume is calculated. To balance the output
     volume with the input volume to the filter, the pre-processing and
     post-processing RMS volumes are calculated. Lastly, a gain is applied to
     all samples in the buffer, calculated as the ratio between the input and
     output volumes.
    */
    void processBlock (juce::AudioBuffer<float>& buffer)
    {
        if (!active) return;
        
        const auto numSamples = buffer.getNumSamples();
        
        double const referenceRMS = buffer.getRMSLevel(0, 0, numSamples);
        
        AudioBuffer<double> doubleBuffer(1, numSamples);
        
        castBuffer(doubleBuffer, buffer, 1, numSamples);
        
        for (auto& element : elements)
            element->processBlock(doubleBuffer, numSamples);
            
        castBuffer(buffer, doubleBuffer, 1, numSamples);
    
        double const gain = referenceRMS / buffer.getRMSLevel(0, 0, numSamples);
        
        buffer.applyGain(0, 0, numSamples, static_cast<float>(gain));
    }
    
    /* The parameterChanged method allows the adjustment of bypass values as
     well as the magnitude and phase of each active zero and pole in the cascade.
     The parameter parameterID contains a string that identifies the parameter
     to be changed, and newValue contains the new value to be assigned.
    */
    void parameterChanged (const String& parameterID, float newValue)
    {
        if (parameterID == "BYPASS")
        {
            setUnsetBypass(newValue > 0.5f);
            return;
        }
        
        juce::String type = parameterID.dropLastCharacters(2);
        FilterElement::Type elementType = (type == "Z") ? FilterElement::ZERO : FilterElement::POLE;
        juce::String elementParameter = parameterID.substring(1, 2);
        const int elementNr = parameterID.getLastCharacters(1).getIntValue();

        int i = 0;
        
        for (auto& element : elements)
        {
            if (element->getType() == elementType)
            {
                ++ i;
                if (i == elementNr)
                    (elementParameter == "M") ? element->setMagnitude(newValue) : element->setPhase(newValue);
            }
        }
    }
    
private:
    std::vector<std::unique_ptr<FilterElement>> elements;
    
    bool active = true;
    
    int countElementsOfType(FilterElement::Type type)
    {
        return static_cast<int>(std::count_if(elements.begin(), elements.end(),
                                     [type](const std::unique_ptr<FilterElement>& element) { return element->getType() == type; }));
    }
};


