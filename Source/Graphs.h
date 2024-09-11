#pragma once
#include <JuceHeader.h>

#define GRAPHS_BACKGROUND                   0xffb3b6b7
#define ZEROS_COLOUR                        0xff9b59b6
#define CONJ_ZEROS_COLOUR                   0x709b59b6
#define POLES_COLOUR                        0xffffbc2e
#define CONJ_POLES_COLOUR                   0x70ffbc2e

class FrequencyResponse : public juce::Component
{
public:
    FrequencyResponse (const std::vector<std::complex<double>>& spectrum, const float currentGain)
    {
        updateMagnitudes(spectrum, currentGain);
    }
    ~FrequencyResponse () {}
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 8.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        g.setColour(juce::Colours::white);
        drawFrequencyResponse(g);
    }
    
    void updateMagnitudes (const std::vector<std::complex<double>>& spectrum, const float currentGain)
    {
        bool shouldRepaint = false;
        
        if (magnitudes.size() != spectrum.size())
        {
            shouldRepaint = true;
            magnitudes.resize(spectrum.size());
        }
        
        long int spectrumSize = spectrum.size();
        for (int i = 0; i < spectrumSize; ++ i)
        {
            double magnitude = currentGain * std::abs(spectrum[i]);
            if (magnitudes[i] != magnitude)
            {
                magnitudes[i] = magnitude;
                shouldRepaint = true;
            }
        }

        if (shouldRepaint)
            repaint();
    }
    
private:
    std::vector<double> magnitudes;
    
    void drawFrequencyResponse(juce::Graphics& g)
    {
        if (magnitudes.empty())
            return;
        
        auto width = getWidth();
        auto height = getHeight();
        
        juce::Path responsePath;
        responsePath.startNewSubPath(0, height - static_cast<float>(magnitudes[0]) * height);
        
        long int magnitudesSize = magnitudes.size();
        for (int i = 1; i < magnitudesSize; ++ i)
        {
            float x = static_cast<float>(i) / magnitudes.size() * width;
            float y = height - static_cast<float>(magnitudes[i]) * height;
            responsePath.lineTo(x, y);
        }
        
        g.strokePath(responsePath, juce::PathStrokeType(1.0f));
    }
};

class PhaseResponse : public juce::Component
{
public:
    PhaseResponse (const std::vector<std::complex<double>>& spectrum)
    {
        updatePhases(spectrum);
    }
    
    ~PhaseResponse () {}
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 8.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        g.setColour(juce::Colours::white);
        drawPhaseResponse(g);
    }
    
    void updatePhases(const std::vector<std::complex<double>>& spectrum)
    {
        bool shouldRepaint = false;
        
        if (phases.size() != spectrum.size())
        {
            shouldRepaint = true;
            phases.resize(spectrum.size());
        }
        
        long int spectrumSize = spectrum.size();
        for (int i = 0; i < spectrumSize; ++ i)
        {
            double phase = (MathConstants<double>::pi + std::arg(spectrum[i])) / (MathConstants<double>::twoPi);
            if (phases[i] != phase)
            {
                phases[i] = phase;
                shouldRepaint = true;
            }
        }
        
        if (shouldRepaint)
            repaint();
    }
    
private:
    std::vector<double> phases;
    
    void drawPhaseResponse(juce::Graphics& g)
    {
        if (phases.empty())
            return;
        
        auto width = getWidth();
        auto height = getHeight();
        
        juce::Path responsePath;
        responsePath.startNewSubPath(0, height - static_cast<float>(phases[0]) * height);
    
        long int phasesSize = phases.size();
        for (int i = 1; i < phasesSize; ++ i)
        {
            float x = static_cast<float>(i) / phases.size() * width;
            float y = height - static_cast<float>(phases[i]) * height;
            responsePath.lineTo(x, y);
        }
        
        g.strokePath(responsePath, juce::PathStrokeType(1.0f));
    }
};

class GaussianPlane : public juce::Component
{
public:
    GaussianPlane (const std::vector<std::shared_ptr<FilterElement>>& elements)
    {
        updateElements(elements);
    }
    ~GaussianPlane () {}
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 8.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        
        auto margin = bounds.reduced(bounds.getWidth() * 0.07f, bounds.getHeight() * 0.07f);
    
        g.setColour(juce::Colours::white);
        drawPlane(g, margin);
        drawPolesAndZeros(g, margin);
    }
    
    void updateElements(const std::vector<std::shared_ptr<FilterElement>>& elements)
    {
        zeros.clear();
        poles.clear();
        
        for (auto& element : elements)
        {
            if (!element->isActive())
                continue;
            if (element->getType())
                poles.push_back(std::polar(element->getMagnitude(), MathConstants<double>::pi * element->getPhase()));
            else
                zeros.push_back(std::polar(element->getMagnitude(), MathConstants<double>::pi * element->getPhase()));
        }
        repaint();
    }
    
private:
    std::vector<std::complex<double>> zeros;
    std::vector<std::complex<double>> poles;
    
    void drawPlane(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();
    
        g.setColour(juce::Colours::white);
        g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 1.0f); // Asse reale
        g.drawLine(centerX, bounds.getY(), centerX, bounds.getBottom(), 1.0f); // Asse immaginario
        
        g.setColour(juce::Colours::white);
        float radius = std::min(width, height) / 2.0f;
        g.drawEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2, 1.5f);

        g.setFont(12.0f);
        g.setColour(juce::Colour(0xff383838));
        
        g.setFont(juce::Font("Gill Sans", 12.0f, juce::Font::plain));
        g.drawText("Re", centerX + radius + 2, centerY - 10, 20, 20, juce::Justification::centred);
        g.drawText("Im", centerX - 8, centerY - radius - 20, 20, 20, juce::Justification::centred);
    }
    
    void drawPolesAndZeros(juce::Graphics& g, juce::Rectangle<float> bounds)
    {
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();
        float radius = 5.0f;
        
        // Zeros are "O"
        for (const auto& zero : zeros)
        {
            g.setColour(juce::Colour (ZEROS_COLOUR));
            float x = (std::real(zero) * (width / 2)) + centerX;
            float y = (-(std::imag(zero)) * (height / 2)) + centerY;
            
            g.drawEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
            
//          DRAW CONJUGATE
            y = ((std::imag(zero)) * (height / 2)) + centerY;
            g.setColour(juce::Colour (CONJ_ZEROS_COLOUR));
            g.drawEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
        }
        
        // Poles are "X"
        for (const auto& pole : poles)
        {
            g.setColour(juce::Colour (POLES_COLOUR));
            float x = (std::real(pole) * (width / 2)) + centerX;
            float y = (-(std::imag(pole)) * (height / 2)) + centerY;
            
            g.drawLine(x - radius, y - radius, x + radius, y + radius, 2.0f);
            g.drawLine(x + radius, y - radius, x - radius, y + radius, 2.0f);
//            DRAW CONJUGATE
            g.setColour(juce::Colour (CONJ_POLES_COLOUR));
            y = ((std::imag(pole)) * (height / 2)) + centerY;
            g.drawLine(x - radius, y - radius, x + radius, y + radius, 2.0f);
            g.drawLine(x + radius, y - radius, x - radius, y + radius, 2.0f);
        }
    }
};
