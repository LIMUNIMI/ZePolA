#pragma once
#include <JuceHeader.h>
#include "Parameters.h"

class Complex
{
public:
    Complex ()
    {
        magnitude.setCurrentAndTargetValue(COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT);
        phase.setCurrentAndTargetValue(COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT);
    }
    
    ~Complex () {}
    
    void setMagnitude (float newValue)
    {
        magnitude.setTargetValue(newValue);
    }
    
    void setPhase (float newValue)
    {
        phase.setTargetValue(newValue);
    }
    
    float getMagnitude ()
    {
        return magnitude.getCurrentValue();
    }
    
    float getPhase ()
    {
        return phase.getCurrentValue();
    }

private:
    SmoothedValue<float, ValueSmoothingTypes::Linear> magnitude;
    SmoothedValue<float, ValueSmoothingTypes::Linear> phase;
};

class BiquadFilter
{
public:
    BiquadFilter ()
    {
        zero.setMagnitude(0.0f);
        zero.setPhase(0.0f);
        pole.setMagnitude(0.0f);
        pole.setPhase(0.0f);
    }
    
    ~BiquadFilter () {}
    
    void prepareToPlay (double sampleRate)
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
    
    void calculateCoefficients ()
    {
        // Calcolo dei coefficienti b0, b1, b2, a0, a1
        
        
    }

    double processSample (double inputSample, int numCh)
    {
        double outputSample = b0 * inputSample + b1 * x1[numCh] + b2 * x2[numCh] - a1 * y1[numCh] - a2 * y2[numCh];
        
        updatePastInputAndOutput(inputSample, outputSample, numCh);
        return outputSample;
    }
    
    void updateParameters (const String& paramID, float newValue, double sampleRate)
    {
        if (paramID == "ZM") {
            zero.setMagnitude(newValue);
            calculateCoefficients();
            return;
        }
        
        if (paramID == "ZP") {
            zero.setPhase(newValue);
            calculateCoefficients();
            return;
        }
        
        if (paramID == "PM") {
            pole.setMagnitude(newValue);
            calculateCoefficients();
            return;
        }
        
        if (paramID == "PP") {
            pole.setPhase(newValue);
            calculateCoefficients();
            return;
        }
    }

private:
    Complex zero;
    Complex pole;
    
    double b0, b1, b2, a1, a2;
    
    double x1[MAX_NUM_CHANNELS], x2[MAX_NUM_CHANNELS], y1[MAX_NUM_CHANNELS], y2[MAX_NUM_CHANNELS];
    
    void updatePastInputAndOutput (double inputSample, double outputSample, int numCh)
    {
        x2[numCh] = x1[numCh];
        x1[numCh] = inputSample;
        
        y2[numCh] = y1[numCh];
        y1[numCh] = outputSample;
    }
};


