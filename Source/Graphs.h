#pragma once
#include <JuceHeader.h>

// Magnitude & phase response MACROS
#define GRAPHS_BACKGROUND                   0x45979a9a
#define GRID_COLOUR                         0x28979a9a
#define TEXT_COLOUR                         0xff797d7f
#define GRAPHS_QUALITY                      2048
#define NUMBER_OF_REFERENCE_FREQUENCIES     8

// Gaussian plane MACROS
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
#define GAUSSIAN_PLANE_RECTANGLE            juce::Rectangle<float>(30, 455, 260, 260)
#define ELEMENT_WIDTH                       16
#define ELEMENT_HEIGHT                      16
#define ELEMENT_LINE_THICKNESS              2.0f

class FrequencyResponse : public juce::Component
{
    public:
    FrequencyResponse (double* vls, double* rf, const double sr, bool yType)
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
            
            g.setColour(juce::Colour(TEXT_COLOUR));
            g.drawText(linLog ? juce::String(level) : juce::String(level) + " dB", 5, yPos - (level == 2 ? 0 : 10), 40, 20, juce::Justification::left);
            
            if (level != 2)
            {
                g.setColour(juce::Colour(GRID_COLOUR));
                g.drawHorizontalLine(static_cast<int>(yPos), bounds.getX(), bounds.getRight());
            }
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

class MagnitudeResponse : public FrequencyResponse
{
    public:
    using FrequencyResponse::FrequencyResponse;
    
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
        g.setColour(juce::Colour(TEXT_COLOUR));
        g.drawText(formatFrequency(sampleRate * 0.5), bounds.getRight() - 20, bounds.getCentreY(), 20, 20, juce::Justification::centred);
        
        g.setFont(juce::Font("Gill Sans", 11.0f, juce::Font::plain));
        g.drawText("Sample rate: " + juce::String(sampleRate) + " Hz", bounds.getRight() - 100, 2, 90, 20, juce::Justification::centred);
    }
    
    private:
    float calculateYValue(bool linLog, double value, const juce::Rectangle<float>& bounds)
    {
        return linLog ? bounds.getBottom() - juce::jmap<float>(value, 0.0f, 2.0f, 0.0f, bounds.getHeight())
        : juce::jmap<float>(20.0f * std::log10(juce::jmax(value, 1e-12)), -60.0f, 60.0f, bounds.getBottom(), bounds.getY());
    }
};

class PhaseResponse : public FrequencyResponse
{
    public:
    using FrequencyResponse::FrequencyResponse;
    
    void drawResponse(juce::Graphics& g, bool /*linLog*/) override
    {
        auto bounds = getLocalBounds().toFloat();
        juce::Path responsePath;
        
        drawPhaseGrid(g, bounds);
        
        responsePath.startNewSubPath(bounds.getX(), bounds.getBottom() - values[0] * bounds.getHeight());
        
        float max;
        
        for (int i = 1; i < GRAPHS_QUALITY; ++i)
        {
            float x = bounds.getX() + i * (bounds.getWidth() / GRAPHS_QUALITY);
            float y = bounds.getBottom() - values[i] * bounds.getHeight();
            float deltaPhase = std::abs(values[i] - values[i - 1]);
            
            // Se il salto è maggiore di π, creo un nuovo subpath
            if (deltaPhase >= 0.5)
            {
                responsePath.startNewSubPath(x, y);
            }
            else
            {
                responsePath.lineTo(x, y);
            }
            
            
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
        g.setColour(juce::Colour(TEXT_COLOUR));
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
            
            if(!i)
                g.drawText(referencePhases[i], 6, y, 20, 20, juce::Justification::centredLeft);
            else
                g.drawText(referencePhases[i], 6, y + offsetY, 20, 20, juce::Justification::centredLeft);
        }
    }
};


class GaussianPlane : public juce::Component
{
    public:
    GaussianPlane (const std::vector<FilterElement>& elements)
    {
        auto componentBounds = GAUSSIAN_PLANE_RECTANGLE;
        bounds = componentBounds.reduced(componentBounds.getWidth() * PLANE_PADDING, componentBounds.getHeight() * PLANE_PADDING);
        updateConjugate(elements);
    }
    
    void updateConjugate (const std::vector<FilterElement>& elements)
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
    
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(GRAPHS_BACKGROUND));
        float cornerSize = 6.0f;
        auto componentBounds = getLocalBounds().toFloat();
        g.fillRoundedRectangle(componentBounds, cornerSize);
        
        bounds = componentBounds.reduced(componentBounds.getWidth() * PLANE_PADDING, componentBounds.getHeight() * PLANE_PADDING);
        
        g.setColour(juce::Colour(LINE_COLOUR));
        drawPlane(g);
        drawPolesAndZeros(g);
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
        return bounds.getHeight() * 0.5 + 500;
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
        return std::min(bounds.getWidth(), bounds.getHeight()) * 0.5;
    }
    
    private:
    juce::Rectangle<float> bounds;
    
    std::vector<std::complex<double>> zeros;
    std::vector<std::complex<double>> poles;
    
    void drawPlane(juce::Graphics& g)
    {
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();
        
        g.setColour(juce::Colour(LINE_COLOUR));
        g.drawLine(bounds.getX(), centerY, bounds.getRight(), centerY, PLANE_LINE_THICKNESS);
        g.drawLine(centerX, bounds.getY(), centerX, bounds.getBottom(), PLANE_LINE_THICKNESS);
        
        float radius = std::min(width, height) * 0.5f;
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
    }
    
    void drawPolesAndZeros(juce::Graphics& g)
    {
        auto width = bounds.getWidth();
        auto height = bounds.getHeight();
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();
        float radius = 5.0f;
        
        for (const auto& zero : zeros)
        {
            g.setColour(juce::Colour (CONJ_ZEROS_COLOUR));
            float x = (std::real(zero) * (width * 0.5)) + centerX;
            float y = ((std::imag(zero)) * (height * 0.5)) + centerY;

            g.drawEllipse(x - radius, y - radius, radius * 2.0f, radius * 2.0f, 2.0f);
        }
    
        for (const auto& pole : poles)
        {
            g.setColour(juce::Colour (CONJ_POLES_COLOUR));
            float x = (std::real(pole) * (width * 0.5)) + centerX;
            float y = ((std::imag(pole)) * (height * 0.5)) + centerY;
            
            g.drawLine(x - radius, y - radius, x + radius, y + radius, 2.0f);
            g.drawLine(x + radius, y - radius, x - radius, y + radius, 2.0f);
        }
    }
};

class DraggableElement : public juce::Component
{
    public:
    
    DraggableElement(FilterElement e, int elNr, GaussianPlane* gp, PolesAndZerosEQAudioProcessor* p)
    {
        updateElement(e, elNr, gp, p);
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
        int newX = getX() + (event.getPosition().x - dragOffset.x) + getWidth() * 0.5;
        int newY = getY() + (event.getPosition().y - dragOffset.y) + getHeight() * 0.5;
        
        juce::Point<int> planeCentre(gaussianPlane->getCentreX(), gaussianPlane->getCentreY());
        juce::Point<int> newRelativePosition(newX - planeCentre.x, planeCentre.y - newY);
        
        auto theta = std::atan2(static_cast<float>(newRelativePosition.y), static_cast<float>(newRelativePosition.x));
        auto radius = gaussianPlane->getRadius();
        auto xLimit = radius * std::cos(theta);
        auto yLimit = radius * std::sin(theta);
        
        int limit1, limit2;
        
        if (!newRelativePosition.x)
        {
            limit1 = static_cast<int>(planeCentre.x);
            limit2 = planeCentre.x + radius;
        } else if (newRelativePosition.x > 0)
        {
            limit1 = static_cast<int>(planeCentre.x);
            limit2 = static_cast<int>(std::ceil(xLimit + planeCentre.x));
        }
        else
        {
            limit1 = static_cast<int>(std::floor(xLimit + planeCentre.x));
            limit2 = static_cast<int>(planeCentre.x);
        }
        
        newX = juce::jlimit(limit1, limit2, newX);
        newY = juce::jlimit(juce::jlimit(0, planeCentre.y, static_cast<int>(std::floor(planeCentre.y - yLimit))), static_cast<int>(planeCentre.y), newY);
        
        setBounds(newX - getWidth() * 0.5, newY - getHeight() * 0.5, getWidth(), getHeight());
        
        toFront(true);
        
        std::complex<double> newPosition = getComplexFromCoordinates(getCentreX(), getCentreY());
        double newMagnitude = std::abs(newPosition);
        double newPhase = std::abs(std::arg(newPosition) / MathConstants<double>::pi);
        
        processor->setParameterValue(processor->parameters.getParameter(MAGNITUDE_NAME + std::to_string(elementNr)), newMagnitude);
        processor->setParameterValue(processor->parameters.getParameter(PHASE_NAME + std::to_string(elementNr)), newPhase);
        
        element = std::polar(newMagnitude, newPhase);
        
        setConjugateCoordinates();
        
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
        
        setCoordinatesFromComplex();
        setBounds(x, y, getWidth(), getHeight());
        repaint();
        
        setConjugateCoordinates();
        
    }
    
    float getCentreX ()
    {
        return this->getX() + getWidth() * 0.5;
    }
    
    float getCentreY()
    {
        return this->getY() + getHeight() * 0.5;
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
    std::complex<double> element;
    FilterElement::Type type;
    int elementNr;
    
    GaussianPlane *gaussianPlane;
    PolesAndZerosEQAudioProcessor *processor;
    
    float x, conjX;
    float y, conjY;
    
    juce::Point<int> dragOffset;
    
    void setCoordinatesFromComplex ()
    {
        x = (std::real(element) * (gaussianPlane->getWidth() * 0.5)) + gaussianPlane->getCentreX() - getWidth() * 0.5;
        y = (-(std::imag(element)) * (gaussianPlane->getHeight() * 0.5)) + gaussianPlane->getCentreY() - getHeight() * 0.5;
    }
    
    void setConjugateCoordinates ()
    {
        conjX = x;
        conjY = (std::imag(element)) * (gaussianPlane->getHeight() * 0.5) + gaussianPlane->getCentreY() - getHeight() * 0.5;
    }
    
    std::complex<double> getComplexFromCoordinates (float x, float y)
    {
        float xRel = x - gaussianPlane->getCentreX();
        float yRel = - (y - gaussianPlane->getCentreY());
        
        float realPart = xRel / (gaussianPlane->getWidth() * 0.5);
        float imagPart = yRel / (gaussianPlane->getHeight() * 0.5);
        
        return std::complex<double>(realPart, imagPart);
    }
};




