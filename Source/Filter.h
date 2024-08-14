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
        x1 = 0.0;
        x2 = 0.0;
        
        y1 = 0.0;
        y2 = 0.0;
    }
    
    void calculateCoefficients ()
    {
        c1 = -2 * getRealPart();
        c2 = magnitude * magnitude;
    }
    
    float processSample (double inputSample)
    {
        double outputSample;
        switch (type)
        {
            case ZERO:
            {
                outputSample = inputSample + c1 * x1 + c2 * x2;
            } break;
                
            case POLE:
            {
                outputSample = inputSample - c1 * y1 - c2 * y2;
            } break;
        }
        
        updatePastInputAndOutput(inputSample, outputSample);
        return outputSample;
    }
    
    void processBlock (juce::AudioBuffer<double>& buffer, int numSamples)
    {
        auto bufferData = buffer.getArrayOfWritePointers();
        
        for (int smp = 0; smp < numSamples; ++smp)
                bufferData[0][smp] = processSample(bufferData[0][smp]);
    }
    
    void updatePastInputAndOutput (double inputSample, double outputSample)
    {
        x2 = x1;
        x1 = inputSample;
        
        y2 = y1;
        y1 = outputSample;
    }
    
private:
    Type type;
    double magnitude, phase;
    
    double c1, c2;

    double x1, x2, y1, y2;
};


class PolesAndZerosCascade
{
public:
    PolesAndZerosCascade ()
    {
        activeZeros = 0;
        activePoles = 0;
    }
    ~PolesAndZerosCascade () {}
    
    void memoryReset ()
    {
        for (auto& zero : zeros)
            zero->memoryReset();
        
        for (auto& pole : poles)
            pole->memoryReset();
    }
    
    void addZero ()
    {
        if (activeZeros < MAX_ORDER)
        {
            zeros.push_back(std::make_unique<FilterElement>(FilterElement::ZERO));
            ++activeZeros;
        }
        else
        {
            DBG("It should not be possibile to add another zero!");
            jassertfalse;
        }
    }
    
    void removeZero ()
    {
        jassert(activeZeros == 0); // It should not be possibile to remove a zero if there is none
        
        zeros.pop_back();
        --activeZeros;
    }
    
    void removePole ()
    {
        jassert(activeZeros == 0); // It should not be possibile to remove a pole if there is none
        
        poles.pop_back();
        --activePoles;
    }
    
    void addPole ()
    {
        if (activePoles < MAX_ORDER)
        {
            poles.push_back(std::make_unique<FilterElement>(FilterElement::POLE));
            ++activePoles;
        }
        else
        {
            DBG("It should not be possibile to add another pole!");
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
    
    int activeZeros, activePoles;
};


