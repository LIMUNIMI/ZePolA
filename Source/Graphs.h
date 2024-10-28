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
#define PLANE_PADDING                       0.07f
#define PLANE_LINE_THICKNESS                1.0f
#define CIRCLE_LINE_THICKNESS               1.5f
#define REFERENCE_ANGLES                    {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 360}

#define GRAPHS_QUALITY                      2048
#define NUMBER_OF_REFERENCE_FREQUENCIES     8

#define ELEMENT_WIDTH                       16
#define ELEMENT_HEIGHT                      16

class GraphicResponse : public juce::Component
{
    public:
    GraphicResponse (double* vls, double* rf, const double sr, bool yType)
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
    
    void updateValues (double* vls, double* rf, const double sr, bool yType)
    {
        std::copy(vls, vls + GRAPHS_QUALITY, values);
        std::copy(rf, rf + NUMBER_OF_REFERENCE_FREQUENCIES, referenceFrequencies);
        sampleRate = sr;
        ampDb = yType;
        repaint();
    }
    
    protected:
    double values[GRAPHS_QUALITY];
    double referenceFrequencies[NUMBER_OF_REFERENCE_FREQUENCIES];
    
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
            
            float x;
            float y;
            int k = 1;
            
            for (int i = 1; i < GRAPHS_QUALITY; ++i)
            {
                x = static_cast<float>(i) / GRAPHS_QUALITY * width;
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
            std::vector<float> linearLevels = { 0.1f, 0.5f, 1.0f, 1.5f, 2.0f };
            g.setColour(juce::Colour(GRID_COLOUR));
            
            for (float level : linearLevels)
            {
                float yPos = juce::jmap<float>(level, 0.0f, 2.0f, height, 0.0f);
                
                if (level != 2.0f)
                    g.drawHorizontalLine(static_cast<int>(yPos), 0, width);
                
                float yOffset = 0.0f;
                if (level == 2.0f)
                    yOffset = 8.0f;
                g.setColour(juce::Colour(TEXT_COLOUR));
                g.drawText(juce::String(level), 5, yPos - 10 + yOffset, 40, 20, juce::Justification::left);
                g.setColour(juce::Colour(GRID_COLOUR));
            }
            
            responsePath.startNewSubPath(0, height - juce::jmap<float>(values[0], 0.0f, 2.0f, 0.0f, height));
            
            float x;
            float y;
            int k = 1;
            
            for (int i = 1; i < GRAPHS_QUALITY; ++i)
            {
                g.setColour(juce::Colour(LINE_COLOUR));
                x = static_cast<float>(i) / GRAPHS_QUALITY * width;
                
                y = height - juce::jmap<float>(values[i], 0.0f, 2.0f, 0.0f, height);
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
        
        float x;
        y = 0;
        int k = 1;
        
        for (int i = 1; i < GRAPHS_QUALITY; ++ i)
        {
            g.setColour(juce::Colour(LINE_COLOUR));
            x = static_cast<float>(i) / GRAPHS_QUALITY * width;
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
    GaussianPlane () {}
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 6.0f;
        auto componentBounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(componentBounds, cornerSize);
        
        bounds = componentBounds.reduced(componentBounds.getWidth() * PLANE_PADDING, componentBounds.getHeight() * PLANE_PADDING);
        
        g.setColour(juce::Colour(LINE_COLOUR));
        drawPlane(g);
    }
    
    juce::Rectangle<float> getBounds ()
    {
        return bounds;
    }
    
    float getCentreX ()
    {
        return getBounds().getCentreX() + getX();
    }
    
    float getCentreY ()
    {
        return getBounds().getCentreY() + getY();
    }
    
    float getWidth ()
    {
        return getBounds().getWidth();
    }
    
    float getHeight ()
    {
        return getBounds().getHeight();
    }
    
    private:
    
    juce::Rectangle<float> bounds;
    
    void drawPlane(juce::Graphics& g)
    {
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();
        
        g.setColour(juce::Colour(LINE_COLOUR));
        g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, PLANE_LINE_THICKNESS);
        g.drawLine(centerX, bounds.getY(), centerX, bounds.getBottom(), PLANE_LINE_THICKNESS);
        
        float radius = std::min(width, height) / 2.0f;
        g.drawEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2, CIRCLE_LINE_THICKNESS);
        
        g.setColour(juce::Colour(PLANE_GRID_COLOUR));
        for (float r = radius * 0.25f; r < radius; r += radius * 0.25f)
            g.drawEllipse(centerX - r, centerY - r, r * 2, r * 2, 0.5f);
        
        for (auto angle : REFERENCE_ANGLES)
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
};

class DraggableElement : public juce::Component
{
    public:
    
    void mouseDown (const MouseEvent& e) override
    {
        dragger.startDraggingComponent(this, e);
    }
    
    void mouseDrag (const MouseEvent& e) override
    {
        Point<int> circleCentre(static_cast<int>(gaussianPlane->getCentreX()), static_cast<int>(gaussianPlane->getCentreY()));
        int radius = static_cast<int>(std::min(gaussianPlane->getWidth(), gaussianPlane->getHeight()) / 2.0f);
        
        DBG(circleCentre.getX() << " " << circleCentre.getY());
        
        // Calcola la nuova posizione del componente basata sul trascinamento
        auto newPosition = getBounds().withPosition(getX() + e.getDistanceFromDragStartX(),
                                                    getY() + e.getDistanceFromDragStartY());

        // Calcola la distanza dal centro del cerchio alla nuova posizione
        auto componentCenter = newPosition.getCentre();
        auto distanceFromCircleCenter = componentCenter.getDistanceFrom(circleCentre);

        // Controlla se la distanza è entro il raggio
        if (distanceFromCircleCenter <= radius)
        {
            dragger.dragComponent(this, e, nullptr); // Muovi il componente liberamente
        }
        else
        {
            // Limita la posizione sul perimetro del cerchio
            auto angle = std::atan2(componentCenter.y - circleCentre.y, componentCenter.x - circleCentre.x);
            auto constrainedX = circleCentre.x + std::cos(angle) * radius;
            auto constrainedY = circleCentre.y + std::sin(angle) * radius;
            setBounds(newPosition.withPosition(constrainedX - getWidth() / 2, constrainedY - getHeight() / 2));
        }
    }
    
    DraggableElement(FilterElement e)
    {
        element = std::polar(e.getMagnitude(), MathConstants<double>::pi * e.getPhase());
        type = e.getType();
    }
    
    void updateElement(FilterElement e, GaussianPlane *gp)
    {
        element = std::polar(e.getMagnitude(), MathConstants<double>::pi * e.getPhase());
        type = e.getType();
        gaussianPlane = gp;
        
        calculatePosition();
        
        this->setBounds(x, y, getWidth(), getHeight());
        
        repaint();
    }
    
    void paint (juce::Graphics& g) override
    {
        float radius = 5.0f;
        
        switch (type)
        {
            case FilterElement::ZERO:
            {
                g.setColour(juce::Colour(ZEROS_COLOUR));
                g.drawEllipse(3, 3, radius * 2.0f, radius * 2.0f, 2.0f);
            } break;
                
            case FilterElement::POLE:
            {
                g.setColour(juce::Colour(POLES_COLOUR));;
                int centerX = getWidth() / 2;
                int centerY = getWidth() / 2;
                
                float radius = 5.0f;
                g.drawLine(centerX - radius, centerY - radius, centerX + radius, centerY + radius, 2.0f);
                g.drawLine(centerX + radius, centerY - radius, centerX - radius, centerY + radius, 2.0f);
            } break;
        }
    }
    
    private:
    std::complex<double> element;
    FilterElement::Type type;
    GaussianPlane *gaussianPlane;

    float x;
    float y;
    
    ComponentDragger dragger;
    
    void calculatePosition ()
    {
        x = (std::real(element) * (gaussianPlane->getWidth() / 2)) + gaussianPlane->getCentreX() - getWidth() * 0.5;
        y = (-(std::imag(element)) * (gaussianPlane->getHeight() / 2)) + gaussianPlane->getCentreY() - getHeight() * 0.5;
    }
};


