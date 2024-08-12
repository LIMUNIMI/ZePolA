#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include <cmath>
#include <vector>

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
        double outputSample = b0 * inputSample + b1 * x1[numCh] + b2 * x2[numCh] - a1 * y1[numCh] - a2 * y2[numCh];
//        IN ALTERNATIVA
//        if (type == ZERO)
//        {
//            double outputSample = b0 * inputSample + b1 * x1[numCh] + b2 * x2[numCh];
//        } 
//        else
//        {
//            double outputSample = - a1 * y1[numCh] - a2 * y2[numCh];
//        }
        
        updatePastInputAndOutput(inputSample, outputSample, numCh);
        return static_cast<float>(outputSample);
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
        for (int i = 0; i < activeZeros; ++i)
            zerosPointers[i]->prepareToPlay();
        
        for (int i = 0; i < activePoles; ++i)
            polesPointers[i]->prepareToPlay();
    }
    
    void releaseResources ()
    {
        for (int i = 0; i < activeZeros; ++i)
            zerosPointers[i]->releaseResources();
        
        for (int i = 0; i < activePoles; ++i)
            polesPointers[i]->releaseResources();
    }
    
    void addZero ()
    {
        ++ activeZeros;
    }
    
    void addPole ()
    {
        ++ activePoles;
    }
    
    float processSample (double inputSample, int numCh)
    {
        std::vector<double> FIR(activeZeros);
        std::vector<double> IIR(activePoles);
        
        for (int i = 0; i < activeZeros; ++i)
        {
            FIR[i] = zerosPointers[i]->processSample(inputSample);
            inputSample = FIR[i];
        }
        
        for (int i = 0; i < activePoles; ++i)
        {
            IIR[i] = polesPointers[i]->processSample(inputSample);
            inputSample = IIR[i];
        }
        auto outputSample = inputSample;
        return static_cast<float>(outputSample);
    }
    
private:
    
    FilterElement z1(FilterElement::ZERO);
    FilterElement z2(FilterElement::ZERO);
    FilterElement z3(FilterElement::ZERO);
    FilterElement z4(FilterElement::ZERO);
    FilterElement z5(FilterElement::ZERO);
    FilterElement z6(FilterElement::ZERO);
    
    FilterElement p1(FilterElement::POLE);
    FilterElement p2(FilterElement::POLE);
    FilterElement p3(FilterElement::POLE);
    FilterElement p4(FilterElement::POLE);
    FilterElement p5(FilterElement::POLE);
    FilterElement p6(FilterElement::POLE);
    
    FilterElement* zerosPointers[] = { &z1, &z2, &z3, &z4, &z5, &z6 };
    FilterElement* polesPointers[] = { &p1, &p2, &p3, &p4, &p5, &p6 };
    
    int activeZeros, activePoles;
};


