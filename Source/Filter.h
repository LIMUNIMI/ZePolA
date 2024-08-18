#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include <vector>
#include <memory>

class FilterElement
{
public:
    
    enum Type { ZERO, POLE };
    
    FilterElement (Type t, double mg = 0.0, double ph = 0.0) : type(t)
    {
        magnitude = mg;
        phase = ph;
        
        calculateCoefficients();
    }
    
    ~FilterElement () {}
    
    void setMagnitude (double newValue)
    {
        magnitude = newValue;
        calculateCoefficients();
    }
    
    void setPhase (double newValue)
    {
        phase = newValue;
        calculateCoefficients();
    }
    
    double getMagnitude ()
    {
        return magnitude;
    }
    
    double getPhase ()
    {
        return phase;
    }
    
    double getRealPart ()
    {
        return getMagnitude() * cos(getPhase() * MathConstants<double>::pi);
    }

    enum Type getType ()
    {
        return type;
    }
    
    void memoryReset ()
    {
        memory1 = 0.0;
        memory2 = 0.0;
    }
    
    void calculateCoefficients ()
    {
        coeff1 = -2 * getRealPart();
        coeff2 = magnitude * magnitude;
    }
    
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
    
    void processBlock (juce::AudioBuffer<double>& buffer, int numSamples)
    {
        auto bufferData = buffer.getArrayOfWritePointers();
        
        for (int smp = 0; smp < numSamples; ++smp)
                bufferData[0][smp] = processSample(bufferData[0][smp]);
    }
    
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


class PolesAndZerosCascade
{
public:
    PolesAndZerosCascade (int nZeros = 0, int nPoles = 0)
    {
        for (int i = 0; i < nZeros; ++ i)
            addElement(FilterElement::ZERO);
        
        for (int i = 0; i < nPoles; ++ i)
            addElement(FilterElement::POLE);
    }
    
    ~PolesAndZerosCascade () {}
    
    void memoryReset ()
    {
        for (auto& node : elements)
            node->memoryReset();
    }
    
    void setUnsetMute (bool newValue)
    {
        active = !newValue;
        if (!active)
            memoryReset();
    }
    
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
    
    void removeElement (FilterElement::Type type)
    {
        // ----------------------------------------------------------------------------
        // ALTERNATIVA CHE ELIMINA IL PRIMO ELEMENTO CORRISPONDENTE AL TIPO SPECIFICATO
        // ----------------------------------------------------------------------------
//        auto iterator = std::find_if(elements.begin(), elements.end(),
//                                   [type](const std::unique_ptr<FilterElement>& element) { return element->getType() == type; });
//        // find_if ritorna un iteratore al primo elemento che corrisponde al criterio di ricerca (type da eliminare)
//        if (iterator != elements.end())
//            elements.erase(iterator);
//        else
//        {
//            DBG("No element of the specified type found!");
//            jassertfalse;
//        }
        
        // ----------------------------------------------------------------------------
        // ALTERNATIVA CHE ELIMINA L'ULTIMO ELEMENTO CORRISPONDENTE AL TIPO SPECIFICATO
        // ----------------------------------------------------------------------------
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
    
    template <typename TargetType, typename SourceType>
    void castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples)
    {
        auto dst = destination.getArrayOfWritePointers();
        auto src = source.getArrayOfReadPointers();

        for (int ch = 0; ch < numChannels; ++ch)
            for (int smp = 0; smp < numSamples; ++smp)
                dst[ch][smp] = static_cast<TargetType>(src[ch][smp]);
    }
    
    void processBlock (juce::AudioBuffer<float>& buffer)
    {
        if (!active) return;
        
        const auto numSamples = buffer.getNumSamples();
        
        AudioBuffer<double> doubleBuffer(1, numSamples);
        
        castBuffer(doubleBuffer, buffer, 1, numSamples);
        
        for (auto& element : elements)
            element->processBlock(doubleBuffer, numSamples);
            
        castBuffer(buffer, doubleBuffer, 1, numSamples);
    }
    
private:
    std::vector<std::unique_ptr<FilterElement>> elements;
    
    bool active = true;
    
    int countElementsOfType(FilterElement::Type type)
    {
        int counter = 0;
        
        for (auto& element : elements)
            if (element->getType() == type)
                ++counter;
        
        return counter;
    }
};


