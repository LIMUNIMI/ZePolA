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
    PolesAndZerosCascade (int nZeros = 1, int nPoles = 1)
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
    
    void setUnsetBypass (bool newValue)
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
        
        const auto numSamples = buffer.getNumSamples(); // Calcolo del numero dei campioni su cui il filtro lavora, per non dover chiamare il metodo più volte si preferisce assegnare una variabile passata per copia a tutti i FilterElement
        
        double const referenceRMS = buffer.getRMSLevel(0, 0, numSamples); // Calcolo RMS del buffer dry prima del filtro
        
        AudioBuffer<double> doubleBuffer(1, numSamples); // Creazione del buffer double che utilizzerà il filtro
        
        castBuffer(doubleBuffer, buffer, 1, numSamples); // Cast del buffer<float> in buffer<double>
        
        for (auto& element : elements) // Loop che chiama la processBlock di ciascuno degli elementi (FilterElement) attivi
            element->processBlock(doubleBuffer, numSamples);
            
        castBuffer(buffer, doubleBuffer, 1, numSamples); // Cast del buffer<double> in buffer<float>
    
        double const gain = referenceRMS / buffer.getRMSLevel(0, 0, numSamples); // Calcolo del gain da applicare al buffer wet in uscita dal filtro mediante il rapporto tra il referenceRMS e il valore RMS del buffer wet
        
        buffer.applyGain(0, 0, numSamples, static_cast<float>(gain)); // Applicazione del gain al buffer in uscita dal filtro
    }
    
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


