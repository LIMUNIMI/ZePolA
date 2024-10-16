#pragma once
#include <JuceHeader.h>

#define GRAPHS_BACKGROUND                   0x45979a9a
#define GRID_COLOUR                         0x28979a9a
#define TEXT_COLOUR                         0xff797d7f
#define ZEROS_COLOUR                        0xff9b59b6
#define CONJ_ZEROS_COLOUR                   0x709b59b6
#define POLES_COLOUR                        0xffffbc2e
#define CONJ_POLES_COLOUR                   0x70ffbc2e
#define LINE_COLOUR                         0xff000000
#define PLANE_GRID_COLOUR                   0x67383838

#define GRAPHS_QUALITY                      2048
#define NUMBER_OF_REFERENCE_FREQUENCIES     8

class GraphicResponse : public juce::Component
{
    public:
    GraphicResponse (const std::vector<double>& vls, const std::vector<double>& rf, const double sr, bool yType)
    {
        updateValues(vls, rf, sr, yType);
    }
    
    void paint (juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 6.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        drawResponse(g, ampDb);
    }
    
    void updateValues (const std::vector<double>& vls, const std::vector<double>& rf, const double sr, bool yType)
    {
        values = vls;
        referenceFrequencies = rf;
        sampleRate = sr;
        ampDb = yType;
        repaint();
    }
    
    protected:
    std::vector<double> values;
    std::vector<double> referenceFrequencies;
    
    double sampleRate;
    
    virtual void drawResponse (juce::Graphics& g, bool ampDb) = 0;
    
    juce::String formatFrequency (const double frequency)
    {
        if (frequency >= 10000)
        {
            return juce::String(juce::roundToInt(frequency / 1000)) + "k";
        }
        return juce::String(juce::roundToInt(frequency));
    }
    
    private:
    bool ampDb = false;
};

class FrequencyResponse : public GraphicResponse
{
    public:
    using GraphicResponse::GraphicResponse;
    
    void drawResponse(juce::Graphics& g, bool linLog) override
    {
        auto width = getWidth();
        auto height = getHeight();
        juce::Path responsePath;
        
        g.setFont(juce::Font("Gill Sans", 10.0f, juce::Font::plain));
        g.setColour(juce::Colour(LINE_COLOUR));
        
        if (!linLog)
        {
            const auto minValue = 1e-12;
            
            const float minDB = -60.0f;
            const float maxDB = 60.0f;
            
            responsePath.startNewSubPath(0, juce::jmap<float>(
                                                              20.0f * std::log10(juce::jmax(values[0], minValue)),
                                                              minDB, maxDB, height, 0.0f));
            
            std::vector<float> dbLevels = { -50.0f, -30.0f, -20.0f, -10.0f, -5.0f, 0.0f, 5.0f, 10.0f, 20.0f, 30.0f, 50.0f };
            g.setColour(juce::Colour(GRID_COLOUR));
            
            for (float dbLevel : dbLevels)
            {
                float yPos = juce::jmap<float>(dbLevel, minDB, maxDB, height, 0.0f);
                g.drawHorizontalLine(static_cast<int>(yPos), 0, width);
                
                g.setColour(juce::Colour(TEXT_COLOUR));
                g.drawText(juce::String(dbLevel) + " dB", 5, yPos - 10, 40, 20, juce::Justification::left);
                g.setColour(juce::Colour(GRID_COLOUR));
            }
            
            long int valuesSize = values.size();
            float x;
            float y;
            int k = 1;
            
            for (int i = 1; i < valuesSize; ++i)
            {
                x = static_cast<float>(i) / valuesSize * width;
                if (isnan(x))
                {
                    x = 0.0;
                }
                y = juce::jmap<float>(
                                      20.0f * std::log10(juce::jmax(values[i], minValue)),
                                      minDB, maxDB, height, 0.0f);
                
                responsePath.lineTo(x, y);
                
                if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
                {
                    g.setColour(juce::Colour(GRID_COLOUR));
                    g.drawVerticalLine(static_cast<int>(x), 0, height);
                    
                    g.setColour(juce::Colour(TEXT_COLOUR));
                    g.drawText(formatFrequency(referenceFrequencies[k] * sampleRate), x - 10, height * 0.5f, 20, 20, juce::Justification::centred);
                    ++k;
                }
            }
            
            g.setColour(juce::Colour(LINE_COLOUR));
            g.strokePath(responsePath, juce::PathStrokeType(1.5f));
            
            g.setColour(juce::Colour(TEXT_COLOUR));
            g.drawText(formatFrequency(sampleRate * 0.5), width - 20, height * 0.5f, 20, 20, juce::Justification::centred);
        }
        else
        {
            std::vector<float> linearLevels = { 0.1f, 0.25f, 0.5f, 0.75f, 1.0f };
            g.setColour(juce::Colour(GRID_COLOUR));
            
            for (float level : linearLevels)
            {
                float yPos = juce::jmap<float>(level, 0.0f, 1.0f, height, 0.0f);
                
                if (level != 1.0f)
                    g.drawHorizontalLine(static_cast<int>(yPos), 0, width);
                
                float yOffset = 0.0f;
                if (level == 1.0f)
                    yOffset = 8.0f;
                g.setColour(juce::Colour(TEXT_COLOUR));
                g.drawText(juce::String(level), 5, yPos - 10 + yOffset, 40, 20, juce::Justification::left);
                g.setColour(juce::Colour(GRID_COLOUR));
            }
            
            responsePath.startNewSubPath(0, height - static_cast<float>(values[0]) * height);
            
            long int valuesSize = values.size();
            float x;
            float y;
            int k = 1;
            
            for (int i = 1; i < valuesSize; ++i)
            {
                g.setColour(juce::Colour(LINE_COLOUR));
                x = static_cast<float>(i) / valuesSize * width;
                y = height - static_cast<float>(values[i]) * height;
                responsePath.lineTo(x, y);
                
                if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
                {
                    g.setColour(juce::Colour(GRID_COLOUR));
                    g.drawVerticalLine(x, 0, height);
                    g.setColour(juce::Colour(TEXT_COLOUR));
                    g.drawText(formatFrequency(referenceFrequencies[k] * sampleRate), x - 10, height * 0.5, 20, 20, juce::Justification::centred);
                    ++k;
                }
            }
            
            g.setColour(juce::Colour(LINE_COLOUR));
            g.strokePath(responsePath, juce::PathStrokeType(1.5f));
            
            g.setColour(juce::Colour(TEXT_COLOUR));
            g.drawText(formatFrequency(sampleRate * 0.5), width - 20, height * 0.5, 20, 20, juce::Justification::centred);
        }
    }
};

class PhaseResponse : public GraphicResponse
{
    public:
    using GraphicResponse::GraphicResponse;
    
    void drawResponse (juce::Graphics& g, bool linLog) override
    {
        auto width = getWidth();
        auto height = getHeight();
        
        juce::Path responsePath;
        g.setColour(juce::Colour(LINE_COLOUR));
        responsePath.startNewSubPath(0, height - static_cast<float>(values[0]) * height);
        
        juce::String piString(CharPointer_UTF8("π"));
        std::vector<juce::String> referencePhases = {piString, piString + " / 2", "0", "-" + piString + " / 2", "-" + piString};
        
        g.setFont(juce::Font("Gill Sans", 10.0f, juce::Font::plain));
        
        int i = 0;
        float y = 0;
        float offsetY = 0;
        for (auto phase : referencePhases)
        {
            if (i != 0 && i != 4)
            {
                offsetY = -10;
                g.setColour(juce::Colour(GRID_COLOUR));
                g.drawHorizontalLine(y, 0, width);
            }
            else if (i == 0)
            {
                offsetY = 0;
            }
            else
            {
                offsetY = -20;
            }
            
            g.setColour(juce::Colour(TEXT_COLOUR));
            g.drawText(phase, 6, y + offsetY, 20, 20, juce::Justification::centredLeft);
            
            y += 0.25 * height;
            ++ i;
        }
        
        g.setColour(juce::Colour(TEXT_COLOUR));
        
        long int valuesSize = values.size();
        float x;
        y = 0;
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
                g.drawVerticalLine(x, 0, height);
                g.setColour(juce::Colour(TEXT_COLOUR));
                g.drawText(formatFrequency(referenceFrequencies[k] * sampleRate), x - 10, height * 0.5, 20, 20, juce::Justification::centred);
                ++ k;
            }
        }
        g.strokePath(responsePath, juce::PathStrokeType(1.5f));
        
        g.setColour(juce::Colour(TEXT_COLOUR));
        g.drawText(formatFrequency(sampleRate * 0.5), x - 20, height * 0.5, 20, 20, juce::Justification::centred);
    }
};

class GaussianPlane : public juce::Component
{
    public:
    GaussianPlane (const std::vector<FilterElement>& elements)
    {
        updateElements(elements);
    }
    ~GaussianPlane () {}
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 6.0f;
        auto bounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(bounds, cornerSize);
        
        auto margin = bounds.reduced(bounds.getWidth() * 0.07f, bounds.getHeight() * 0.07f);
        
        g.setColour(juce::Colour(LINE_COLOUR));
        drawPlane(g, margin);
        drawPolesAndZeros(g, margin);
    }
    
    void updateElements(const std::vector<FilterElement>& elements)
    {
        zeros.clear();
        poles.clear();
        
        for (auto& element : elements)
        {
            if (!element.isActive())
                continue;
            if (element.getType())
                poles.push_back(std::polar(element.getMagnitude(), MathConstants<double>::pi * element.getPhase()));
            else
                zeros.push_back(std::polar(element.getMagnitude(), MathConstants<double>::pi * element.getPhase()));
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
        
        g.setColour(juce::Colour(PLANE_GRID_COLOUR));
        for (float r = radius * 0.25f; r < radius; r += radius * 0.25f)
        {
            g.drawEllipse(centerX - r, centerY - r, r * 2, r * 2, 0.5f);
        }
        
        
        for (int angle = 0; angle < 360; angle += 30)
        {
            float rad = juce::MathConstants<float>::pi * angle / 180.0f;
            float x = std::cos(rad) * radius;
            float y = std::sin(rad) * radius;
            g.drawLine(centerX, centerY, centerX + x, centerY - y, 0.5f);
        }
        
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
            
            // DRAW CONJUGATE
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
            // DRAW CONJUGATE
            g.setColour(juce::Colour (CONJ_POLES_COLOUR));
            y = ((std::imag(pole)) * (height / 2)) + centerY;
            g.drawLine(x - radius, y - radius, x + radius, y + radius, 2.0f);
            g.drawLine(x + radius, y - radius, x - radius, y + radius, 2.0f);
        }
    }
};



