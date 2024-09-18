#pragma once
#include <JuceHeader.h>

class StereoWrapper
{
public:
    StereoWrapper(PolesAndZerosCascade& filter) : filter(filter) {}
    
    void processStereoBlock(juce::AudioBuffer<float>& buffer)
    {
        if (buffer.getNumChannels() < 2)
            return;
        
        juce::AudioBuffer<float> leftChannelBuffer(buffer.getWritePointer(0), buffer.getNumSamples());
        juce::AudioBuffer<float> rightChannelBuffer(buffer.getWritePointer(1), buffer.getNumSamples());
    
        filter.processBlock(leftChannelBuffer);
        
        filter.processBlock(rightChannelBuffer);
    }
    
private:
    PolesAndZerosCascade& filter;
};

