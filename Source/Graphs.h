#pragma once
#include <JuceHeader.h>

#define GRAPHS_BACKGROUND       0xff2a2a2a

class FrequencyResponse : public juce::Component
{
public:
    FrequencyResponse (const std::vector<std::complex<double>>& spectrum)
    {
        updateMagnitudes(spectrum);
    }
    ~FrequencyResponse () {}
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour (GRAPHS_BACKGROUND));
        g.setColour(juce::Colours::white);
        drawFrequencyResponse(g);
    }
    
    void updateMagnitudes (const std::vector<std::complex<double>>& spectrum)
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
            double magnitude = std::abs(spectrum[i]);
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
        g.fillAll(juce::Colour (GRAPHS_BACKGROUND));
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
            double phase = std::arg(spectrum[i]);
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
        g.fillAll(juce::Colour (GRAPHS_BACKGROUND));
        drawPlane(g);
        drawPolesAndZeros(g);
    }
    
    void updateElements(const std::vector<std::shared_ptr<FilterElement>>& elements)
    {
        zeros.clear();
        poles.clear();
        
        for (auto& element : elements)
        {
            if (element->getType() and element->isActive())
                poles.push_back(std::polar(element->getMagnitude(), element->getPhase()));
            else
                zeros.push_back(std::polar(element->getMagnitude(), element->getPhase()));
        }
        repaint();
    }
    
private:
    std::vector<std::complex<double>> zeros;
    std::vector<std::complex<double>> poles;
    
    void drawPlane(juce::Graphics& g)
    {
        auto width = getWidth();
        auto height = getHeight();
        
        // Axis paint
        g.setColour(juce::Colours::white);
        g.drawLine(0, height / 2, width, height / 2, 0.7f);
        g.drawLine(width / 2, 0, width / 2, height, 0.7f);
        
        // Circumference paint
        g.setColour(juce::Colours::white);
        float radius = std::min(width, height) / 2.0f;
        g.drawEllipse((width / 2) - radius, (height / 2) - radius, radius * 2, radius * 2, 1.2f);
    }
    
    void drawPolesAndZeros(juce::Graphics& g)
    {
        auto width = getWidth();
        auto height = getHeight();
        float radius = 5.0f;
        
        // Zeros are "O"
        g.setColour(juce::Colour (0xff3498db));
        for (const auto& zero : zeros)
        {
            float x = (std::real(zero) * (width / 2)) + (width / 2);
            float y = (-(std::imag(zero)) * (height / 2)) + (height / 2);
            
            g.drawEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
        }
        
        // Poles are "X"
        g.setColour(juce::Colour (0xfff1c40f));
        for (const auto& pole : poles)
        {
            float x = (std::real(pole) * (width / 2)) + (width / 2);
            float y = (-(std::imag(pole)) * (height / 2)) + (height / 2);
            
            g.drawLine(x - radius, y - radius, x + radius, y + radius, 2.0f);
            g.drawLine(x + radius, y - radius, x - radius, y + radius, 2.0f);
        }
    }
};
