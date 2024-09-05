#pragma once
#include <JuceHeader.h>

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
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        drawSpectrum(g);
    }
    
    void updateMagnitudes (const std::vector<std::complex<double>>& spectrum)
    {
        magnitudes.resize(spectrum.size());
        for (int i = 0; i < spectrum.size(); ++ i)
        {
            magnitudes[i] = std::abs(spectrum[i]);
        }
        repaint();
    }
    
private:
    std::vector<double> magnitudes;
    
    void drawSpectrum(juce::Graphics& g)
    {
        if (magnitudes.empty())
            return;
        
        auto width = getWidth();
        auto height = getHeight();
    
        for (int i = 1; i < magnitudes.size(); ++ i)
        {
            float x1 = static_cast<float>(i - 1) / magnitudes.size() * width;
            float y1 = height - static_cast<float>(magnitudes[i - 1]) * height;
            
            float x2 = static_cast<float>(i) / magnitudes.size() * width;
            float y2 = height - static_cast<float>(magnitudes[i]) * height;
            
            g.drawLine(x1, y1, x2, y2, 1.0f);
        }
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
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        drawSpectrum(g);
    }
    
    void updatePhases (const std::vector<std::complex<double>>& spectrum)
    {
        phases.resize(spectrum.size());
        for (int i = 0; i < spectrum.size(); ++ i)
        {
            phases[i] = std::arg(spectrum[i]);
        }
        repaint();
    }
    
private:
    std::vector<double> phases;
    
    void drawSpectrum(juce::Graphics& g)
    {
        if (phases.empty())
            return;
        
        auto width = getWidth();
        auto height = getHeight();
    
        for (int i = 1; i < phases.size(); ++ i)
        {
            float x1 = static_cast<float>(i - 1) / phases.size() * width;
            float y1 = height - static_cast<float>(phases[i - 1]) * height;
            
            float x2 = static_cast<float>(i) / phases.size() * width;
            float y2 = height - static_cast<float>(phases[i]) * height;
            
            g.drawLine(x1, y1, x2, y2, 1.0f);
        }
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
        g.fillAll(juce::Colours::black);
        drawPlane(g);
        drawPolesAndZeros(g);
    }
    
    void updateElements(const std::vector<std::shared_ptr<FilterElement>>& elements)
    {
        zeros.clear();
        poles.clear();
        
        for (auto& element : elements)
        {
            if (element->getType())
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
        g.drawLine(0, height / 2, width, height / 2, 2.0f);
        g.drawLine(width / 2, 0, width / 2, height, 2.0f);
        
        // Circumference paint
        g.setColour(juce::Colours::yellow);
        float radius = std::min(width, height) / 2.0f;
        g.drawEllipse((width / 2) - radius, (height / 2) - radius, radius * 2, radius * 2, 1.5f);
    }
    
    void drawPolesAndZeros(juce::Graphics& g)
    {
        auto width = getWidth();
        auto height = getHeight();
        float radius = 5.0f;
        
        // Zeros are "O"
        g.setColour(juce::Colours::blue);
        for (const auto& zero : zeros)
        {
            float x = (std::real(zero) * (width / 2)) + (width / 2);
            float y = (-(std::imag(zero)) * (height / 2)) + (height / 2);
            
            g.drawEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
        }
        
        // Poles are "X"
        g.setColour(juce::Colours::red);
        for (const auto& pole : poles)
        {
            float x = (std::real(pole) * (width / 2)) + (width / 2);
            float y = (-(std::imag(pole)) * (height / 2)) + (height / 2);
            
            g.drawLine(x - radius, y - radius, x + radius, y + radius, 2.0f); // Disegna "X"
            g.drawLine(x + radius, y - radius, x - radius, y + radius, 2.0f);
        }
    }
};
