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
#define PLANE_PADDING                       1.0 / 13.0
#define PLANE_LINE_THICKNESS                1.0f
#define CIRCLE_LINE_THICKNESS               1.5f
#define REFERENCE_ANGLES                    {0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330, 360}

#define GRAPHS_QUALITY                      2048
#define NUMBER_OF_REFERENCE_FREQUENCIES     8

#define ELEMENT_WIDTH                       16
#define ELEMENT_HEIGHT                      16
#define ELEMENT_LINE_THICKNESS              2.0f

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
    
    void drawGridAndReferenceFrequencies(juce::Graphics& g, bool linLog, const juce::Rectangle<float>& bounds)
    {
        std::vector<float> levels = linLog ? std::vector<float>{0.1f, 0.5f, 1.0f, 1.5f, 2.0f}
        : std::vector<float>{-50.0f, -30.0f, -20.0f, -10.0f, -5.0f, 0.0f, 5.0f, 10.0f, 20.0f, 30.0f, 50.0f};
        g.setFont(juce::Font("Gill Sans", 10.0f, juce::Font::plain));
        
        for (auto level : levels)
        {
            float yPos = juce::jmap<float>(level, linLog ? 0.0f : -60.0f, linLog ? 2.0f : 60.0f, bounds.getBottom(), bounds.getY());
            g.setColour(juce::Colour(GRID_COLOUR));
            g.drawHorizontalLine(static_cast<int>(yPos), bounds.getX(), bounds.getRight());
            
            g.setColour(juce::Colour(TEXT_COLOUR));
            g.drawText(linLog ? juce::String(level) : juce::String(level) + " dB", 5, yPos - 10, 40, 20, juce::Justification::left);
        }
    }
    
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
        auto bounds = getLocalBounds().toFloat();
        juce::Path responsePath;
        
        drawGridAndReferenceFrequencies(g, linLog, bounds);
        
        responsePath.startNewSubPath(bounds.getX(), calculateYValue(linLog, values[0], bounds));
        
        for (int i = 1; i < GRAPHS_QUALITY; ++i)
        {
            float x = bounds.getX() + i * (bounds.getWidth() / GRAPHS_QUALITY);
            float y = calculateYValue(linLog, values[i], bounds);
            responsePath.lineTo(x, y);
            
            if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
            {
                g.setColour(juce::Colour(GRID_COLOUR));
                g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());
                g.setColour(juce::Colour(TEXT_COLOUR));
                g.drawText(formatFrequency(referenceFrequencies[i / (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)] * sampleRate), x - 10, bounds.getCentreY(), 20, 20, juce::Justification::centred);
            }
        }
        
        g.setColour(juce::Colour(LINE_COLOUR));
        g.strokePath(responsePath, juce::PathStrokeType(1.5f));
        g.drawText(formatFrequency(sampleRate * 0.5), bounds.getRight() - 20, bounds.getCentreY(), 20, 20, juce::Justification::centred);
    }
    
    private:
    float calculateYValue(bool linLog, double value, const juce::Rectangle<float>& bounds)
    {
        return linLog ? bounds.getBottom() - juce::jmap<float>(value, 0.0f, 2.0f, 0.0f, bounds.getHeight())
        : juce::jmap<float>(20.0f * std::log10(juce::jmax(value, 1e-12)), -60.0f, 60.0f, bounds.getBottom(), bounds.getY());
    }
};

class PhaseResponse : public GraphicResponse
{
    public:
    using GraphicResponse::GraphicResponse;
    
    void drawResponse(juce::Graphics& g, bool /*linLog*/) override
    {
        auto bounds = getLocalBounds().toFloat();
        juce::Path responsePath;
        
        drawPhaseGrid(g, bounds);
        
        responsePath.startNewSubPath(bounds.getX(), bounds.getBottom() - values[0] * bounds.getHeight());
        
        for (int i = 1; i < GRAPHS_QUALITY; ++i)
        {
            float x = bounds.getX() + i * (bounds.getWidth() / GRAPHS_QUALITY);
            float y = bounds.getBottom() - values[i] * bounds.getHeight();
            responsePath.lineTo(x, y);
            
            if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
            {
                g.setColour(juce::Colour(GRID_COLOUR));
                g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());
                g.setColour(juce::Colour(TEXT_COLOUR));
                g.drawText(formatFrequency(referenceFrequencies[i / (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)] * sampleRate), x - 10, bounds.getCentreY(), 20, 20, juce::Justification::centred);
            }
        }
        
        g.setColour(juce::Colour(LINE_COLOUR));
        g.strokePath(responsePath, juce::PathStrokeType(1.5f));
        g.drawText(formatFrequency(sampleRate * 0.5), bounds.getRight() - 20, bounds.getCentreY(), 20, 20, juce::Justification::centred);
    }
    
    private:
    void drawPhaseGrid(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        juce::String piString(CharPointer_UTF8("π"));
        std::vector<juce::String> referencePhases = { piString, piString + " / 2", "0", "-" + piString + " / 2", "-" + piString };
        
        g.setFont(juce::Font("Gill Sans", 10.0f, juce::Font::plain));
        
        for (int i = 0; i < referencePhases.size(); ++i)
        {
            float y = bounds.getY() + i * (bounds.getHeight() / (referencePhases.size() - 1));
            float offsetY = (i == 0 || i == referencePhases.size() - 1) ? -20.0f : -10.0f;
            
            g.setColour(juce::Colour(GRID_COLOUR));
            if (i != 0 && i != referencePhases.size() - 1)
            {
                g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());
            }
            
            g.setColour(juce::Colour(TEXT_COLOUR));
            g.drawText(referencePhases[i], 6, y + offsetY, 20, 20, juce::Justification::centredLeft);
        }
    }
};


class GaussianPlane : public juce::Component
{
    public:
    GaussianPlane ()
    {
        auto componentBounds = juce::Rectangle<float>(30, 455, 260, 260);
        bounds = componentBounds.reduced(componentBounds.getWidth() * PLANE_PADDING, componentBounds.getHeight() * PLANE_PADDING);
    }
    
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
        return bounds.getWidth() * 0.5 + 50;
    }
    
    float getCentreY ()
    {
        return bounds.getHeight() * 0.5 + 475;
    }
    
    float getWidth ()
    {
        return getBounds().getWidth();
    }
    
    float getHeight ()
    {
        return getBounds().getHeight();
    }
    
    float getRadius ()
    {
        return std::min(bounds.getWidth(), bounds.getHeight()) / 2.0f;
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
    
    DraggableElement(FilterElement e, int elNr, GaussianPlane *gp, PolesAndZerosEQAudioProcessor *p) : conjugateElement(*this)
    {
        dragBounds = juce::Rectangle<int>(42, 467, 236, 126);
        
        updateElement(e, elNr, gp, p);
        addAndMakeVisible(conjugateElement);
    }
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
    
    void mouseDown (const MouseEvent& event) override
    {
        dragOffset = event.getPosition();
    }
    
    void mouseDrag (const MouseEvent& event) override
    {
        int newX = getX() + (event.getPosition().x - dragOffset.x);
        int newY = getY() + (event.getPosition().y - dragOffset.y);
        
        newX = juce::jlimit(dragBounds.getX(), dragBounds.getRight() - getWidth(), newX);
        newY = juce::jlimit(dragBounds.getY(), dragBounds.getBottom() - getHeight(), newY);
        
        setBounds(newX, newY, getWidth(), getHeight());
        
        std::complex<double> newPosition = getComplexFromCoordinates(this->getCentreX(), this->getCentreY());
        double newMagnitude = std::abs(newPosition);
        double newPhase = std::arg(newPosition) / MathConstants<double>::pi;
        
        processor->setParameterValue(processor->parameters.getParameter(MAGNITUDE_NAME + std::to_string(elementNr)), newMagnitude);
        processor->setParameterValue(processor->parameters.getParameter(PHASE_NAME + std::to_string(elementNr)), newPhase);
        
        element = std::polar(newMagnitude, newPhase);
        
        getConjugateCoordinates(element);
        conjugateElement.setBounds(conjX, conjY, getWidth(), getHeight());
        addAndMakeVisible(conjugateElement);
        conjugateElement.toFront(true);
        conjugateElement.repaint();
    }
    
    void updateElement(FilterElement e, int elNr, GaussianPlane *gp, PolesAndZerosEQAudioProcessor *p)
    {
        element = std::polar(e.getMagnitude(), MathConstants<double>::pi * e.getPhase());
        type = e.getType();
        elementNr = elNr;
        if (!e.isActive())
            setVisible(false);
        else
            setVisible(true);
        
        gaussianPlane = gp;
        processor = p;
        
        getCoordinatesFromComplex();
        setBounds(x, y, getWidth(), getHeight());
        repaint();
        
        getConjugateCoordinates(element);
        conjugateElement.setBounds(conjX, conjY, getWidth(), getHeight());
        addAndMakeVisible(conjugateElement);
        conjugateElement.toFront(true);
        conjugateElement.repaint();
    }
    
    float getCentreX ()
    {
        return this->getX() + getWidth() / 2;
    }
    
    float getCentreY()
    {
        return this->getY() + getHeight() / 2;
    }
    
    void paint (juce::Graphics& g) override
    {
        float radius = 5.0f;
        
        switch (type)
        {
            case FilterElement::ZERO:
            {
                g.setColour(juce::Colour(ZEROS_COLOUR));
                g.drawEllipse(3, 3, radius * 2.0f, radius * 2.0f, ELEMENT_LINE_THICKNESS);
            } break;
                
            case FilterElement::POLE:
            {
                g.setColour(juce::Colour(POLES_COLOUR));;
                int centerX = getWidth() * 0.5;
                int centerY = getWidth() * 0.5;
                
                float radius = 5.0f;
                g.drawLine(centerX - radius, centerY - radius, centerX + radius, centerY + radius, ELEMENT_LINE_THICKNESS);
                g.drawLine(centerX + radius, centerY - radius, centerX - radius, centerY + radius, ELEMENT_LINE_THICKNESS);
            } break;
        }
    }
    
    private:
    
    class ConjugateElement : public juce::Component
    {
        public:
        
        ConjugateElement(DraggableElement& parentElement) : parent(parentElement) {}
        
        void paint(juce::Graphics& g) override
        {
            float radius = 5.0f;
            
            switch (parent.type)
            {
                case FilterElement::ZERO:
                {
                    g.setColour(juce::Colour(CONJ_ZEROS_COLOUR));
                    g.drawEllipse(3, 3, radius * 2.0f, radius * 2.0f, ELEMENT_LINE_THICKNESS);
                } break;
                    
                case FilterElement::POLE:
                {
                    g.setColour(juce::Colour(CONJ_POLES_COLOUR));
                    int centerX = getWidth() * 0.5;
                    int centerY = getWidth() * 0.5;
                    
                    g.drawLine(centerX - radius, centerY - radius, centerX + radius, centerY + radius, ELEMENT_LINE_THICKNESS);
                    g.drawLine(centerX + radius, centerY - radius, centerX - radius, centerY + radius, ELEMENT_LINE_THICKNESS);
                } break;
            }
        }
        
        private:
        DraggableElement& parent;
    };
    
    std::complex<double> element;
    FilterElement::Type type;
    int elementNr;
    
    GaussianPlane *gaussianPlane;
    PolesAndZerosEQAudioProcessor *processor;
    
    float x, conjX;
    float y, conjY;
    
    juce::Rectangle<int> dragBounds;
    juce::Point<int> dragOffset;
    
    ConjugateElement conjugateElement;
    
    void getCoordinatesFromComplex ()
    {
        x = (std::real(element) * (gaussianPlane->getWidth() * 0.5)) + gaussianPlane->getCentreX() - getWidth() * 0.5;
        y = (-(std::imag(element)) * (gaussianPlane->getHeight() * 0.5)) + gaussianPlane->getCentreY() - getHeight() * 0.5;
    }
    
    void getConjugateCoordinates(std::complex<double> conjugate)
    {
        conjX = (std::real(conjugate) * (gaussianPlane->getWidth() * 0.5)) + gaussianPlane->getCentreX() - conjugateElement.getWidth() * 0.5;
        conjY = ((std::imag(conjugate)) * (gaussianPlane->getHeight() * 0.5)) + gaussianPlane->getCentreY() - conjugateElement.getHeight() * 0.5;
    }
    
    std::complex<double> getComplexFromCoordinates(float x, float y)
    {
        float xRel = x - gaussianPlane->getCentreX();
        float yRel = -(y - gaussianPlane->getCentreY());
        
        float realPart = xRel / (gaussianPlane->getWidth() * 0.5);
        float imagPart = yRel / (gaussianPlane->getHeight() * 0.5);
        
        return std::complex<double>(realPart, imagPart);
    }
};


