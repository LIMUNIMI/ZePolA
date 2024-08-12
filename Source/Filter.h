#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include <cmath>
#include <vector>
#include <memory>

class FilterElement
{
public:
    
    enum Type { ZERO, POLE };
    
    FilterElement (Type t) : type(t)
    {
        magnitude = COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT;
        phase = COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT;
        
        b0 = 1.0f;
        b1 = 0.0f;
        b2 = 0.0f;
        
        a1 = 0.0f;
        a2 = 0.0f;
        
        calculateCoefficients();
    }
    
    ~FilterElement () {}
    
    void prepareToPlay ()
    {
        x1[0] = 0.0f; x1[1] = 0.0f;
        x2[0] = 0.0f; x2[1] = 0.0f;
        
        y1[0] = 0.0f; y1[1] = 0.0f;
        y2[0] = 0.0f; y2[1] = 0.0f;
        
        calculateCoefficients();
    }
    
    void releaseResources ()
    {
        x1[0] = 0.0f; x1[1] = 0.0f;
        x2[0] = 0.0f; x2[1] = 0.0f;
        
        y1[0] = 0.0f; y1[1] = 0.0f;
        y2[0] = 0.0f; y2[1] = 0.0f;
    }
    
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
    
    float getRealPart ()
    {
        return getMagnitude() * cos(getPhase() * M_PI);
    }
    
    float getImaginaryPart ()
    {
        return getMagnitude() * sin(getPhase() * M_PI);
    }

    enum Type getType ()
    {
        return type;
    }
    
    void calculateCoefficients ()
    {
        if (type == ZERO)
        {
            b1 = -2 * getRealPart();
            b2 = magnitude * magnitude;
        }
        else
        {
            a1 = -2 * getRealPart();
            a2 = magnitude * magnitude;
        }
    }
    
    float processSample (double inputSample, int numCh)
    {
        double outputSample;
        if (type == ZERO)
        {
            outputSample = b0 * inputSample + b1 * x1[numCh] + b2 * x2[numCh];
        } 
        else
        {
            outputSample = - a1 * y1[numCh] - a2 * y2[numCh];
        }
        
        updatePastInputAndOutput(inputSample, outputSample, numCh);
        return static_cast<float>(outputSample);
    }
    
    void processBlock (juce::AudioBuffer<float>& buffer, int numChannels, int numSamples)
    {
        auto bufferData = buffer.getArrayOfWritePointers();
        
        for (int smp = 0; smp < numSamples; ++smp)
            for (int ch = 0; ch < numChannels; ++ch)
                bufferData[ch][smp] = processSample(static_cast<double>(bufferData[ch][smp]), ch);
    }
    
    void updatePastInputAndOutput (double inputSample, double outputSample, int numCh)
    {
        x2[numCh] = x1[numCh];
        x1[numCh] = inputSample;
        
        y2[numCh] = y1[numCh];
        y1[numCh] = outputSample;
    }
    
private:
    Type type;
    double magnitude, phase;
    
    double b0, b1, b2, a1, a2;

    double x1[MAX_NUM_CHANNELS], x2[MAX_NUM_CHANNELS], y1[MAX_NUM_CHANNELS], y2[MAX_NUM_CHANNELS];
};


class PoleAndZeroCascade
{
public:
    PoleAndZeroCascade ()
    {
        activeZeros = 0;
        activePoles = 0;
    }
    ~PoleAndZeroCascade () {}
    
    void prepareToPlay ()
    {
        for (auto& zero : zeros)
            zero->prepareToPlay();
        
        for (auto& pole : poles)
            pole->prepareToPlay();
    }
    
    void releaseResources ()
    {
        for (auto& zero : zeros)
            zero->releaseResources();
        
        for (auto& pole : poles)
            pole->releaseResources();
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
            DBG("Non deve permettere di aggiungere un altro zero!");
            jassertfalse;
        }
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
            DBG("Non deve permettere di aggiungere un altro polo!");
            jassertfalse;
        }
    }
    
    void processBlock (juce::AudioBuffer<float>& buffer)
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();
        
        for (auto& zero : zeros)
            zero->processBlock(buffer, numChannels, numSamples);
        
        for (auto& pole : poles)
            pole->processBlock(buffer, numChannels, numSamples);
    }
    
private:
    std::vector<std::unique_ptr<FilterElement>> zeros;
    std::vector<std::unique_ptr<FilterElement>> poles;
    
    int activeZeros, activePoles;
};


