#pragma once
#include <JuceHeader.h>

#define GRAPHS_BACKGROUND                   0xffb3b6b7
#define ZEROS_COLOUR                        0xff9b59b6
#define CONJ_ZEROS_COLOUR                   0x709b59b6
#define POLES_COLOUR                        0xffffbc2e
#define CONJ_POLES_COLOUR                   0x70ffbc2e

class GraphicResponse : public juce::Component
{
public:
    GraphicResponse (const std::vector<double>& vls)
    {
        updateValues(vls);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 8.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        g.setColour(juce::Colours::white);
        drawResponse(g);
    }
    
    void updateValues (const std::vector<double>& vls)
    {
        values = vls;
        repaint();
    }
    
private:
    std::vector<double> values;
    
    void drawResponse (juce::Graphics& g)
    {
        auto width = getWidth();
        auto height = getHeight();
        
        juce::Path responsePath;
        responsePath.startNewSubPath(0, height - static_cast<float>(values[0]) * height);
        
        long int valuesSize = values.size();
        for (int i = 1; i < valuesSize; ++ i)
        {
            float x = static_cast<float>(i) / valuesSize * width;
            float y = height - static_cast<float>(values[i]) * height;
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
