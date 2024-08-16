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
    double magnitude, phase;
    
    double coeff1, coeff2;

    double memory1, memory2;
};


class PolesAndZerosCascade
{
public:
    PolesAndZerosCascade () {}
    
    ~PolesAndZerosCascade () {}
    
    void memoryReset ()
    {
        for (auto& zero : zeros)
            zero->memoryReset();
        
        for (auto& pole : poles)
            pole->memoryReset();
    }
    
    void setUnsetMute (bool newValue)
    {
        active = !newValue;
        if (!active)
            memoryReset();
    }
    
    void addZero ()
    {
        if (zeros.size() < MAX_ORDER)
            zeros.push_back(std::make_unique<FilterElement>(FilterElement::ZERO));
        else
        {
            DBG("It should not be possibile to add another zero!");
            jassertfalse;
        }
    }
    
    void removeZero ()
    {
        jassert(zeros.size() == 0); // It should not be possibile to remove a zero if there is none
        
        zeros.pop_back();
    }
    
    void addPole ()
    {
        if (poles.size() < MAX_ORDER)
            poles.push_back(std::make_unique<FilterElement>(FilterElement::POLE));
        else
        {
            DBG("It should not be possibile to add another pole!");
            jassertfalse;
        }
    }
    
    void removePole ()
    {
        jassert(poles.size() == 0); // It should not be possibile to remove a pole if there is none
        
        poles.pop_back();
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
        
        for (auto& zero : zeros)
            zero->processBlock(doubleBuffer, numSamples);
        
        for (auto& pole : poles)
            pole->processBlock(doubleBuffer, numSamples);
        
        castBuffer(buffer, doubleBuffer, 1, numSamples);
    }
    
private:
    std::vector<std::unique_ptr<FilterElement>> zeros;
    std::vector<std::unique_ptr<FilterElement>> poles;
    
    bool active = true;
};


