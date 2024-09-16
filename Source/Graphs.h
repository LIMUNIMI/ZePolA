#pragma once
#include <JuceHeader.h>

#define GRAPHS_BACKGROUND                   0xfffdfefe
#define GRID_COLOUR                         0xff252525
#define ZEROS_COLOUR                        0xff9b59b6
#define CONJ_ZEROS_COLOUR                   0x709b59b6
#define POLES_COLOUR                        0xffffbc2e
#define CONJ_POLES_COLOUR                   0x70ffbc2e
#define LINE_COLOUR                         0xff000000

#define GRAPHS_QUALITY                      2048
#define NUMBER_OF_REFERENCE_FREQUENCIES     8

class GraphicResponse : public juce::Component
{
public:
    GraphicResponse (const std::vector<double>& vls, const std::vector<double>& rf, const double sr)
    {
        updateValues(vls, rf, sr);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 8.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        drawResponse(g);
    }
    
    void updateValues (const std::vector<double>& vls, const std::vector<double>& rf, const double sr)
    {
        values = vls;
        referenceFrequencies = rf;
        sampleRate = sr;
        repaint();
    }
    
private:
    std::vector<double> values;
    std::vector<double> referenceFrequencies;
    
    double sampleRate;
    
    
    
    void drawResponse (juce::Graphics& g)
    {
        auto width = getWidth();
        auto height = getHeight();
    
        
        juce::Path responsePath;
        responsePath.startNewSubPath(0, height - static_cast<float>(values[0]) * height);
        g.drawVerticalLine(0, 0, height);
        
        long int valuesSize = values.size();
        float x;
        float y;
        int k = 1;
        
        for (int i = 1; i < valuesSize; ++ i)
        {
            g.setColour(juce::Colour(LINE_COLOUR));
            x = static_cast<float>(i) / valuesSize * width;
            y = height - static_cast<float>(values[i]) * height;
            responsePath.lineTo(x, y);
            
            if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
            {
                g.setColour(juce::Colour(GRID_COLOUR));
                g.drawVerticalLine((int)x, 0, (float)height);
                k ++;
            }
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
    
        g.setColour(juce::Colour(LINE_COLOUR));
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
    
        g.setColour(juce::Colour(LINE_COLOUR));
        g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, 1.0f);
        g.drawLine(centerX, bounds.getY(), centerX, bounds.getBottom(), 1.0f);
        
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

