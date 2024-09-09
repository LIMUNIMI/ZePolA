#pragma once
#include <JuceHeader.h>

#define MAGNITUDE_SLIDERS_COLOUR            0xff1f1f1c
#define PHASE_SLIDERS_COLOUR                0xff6f7071
#define SLIDER_BAR_COLOUR                   0xff797d7f
#define SLIDER_LABEL_COLOUR                 0xff333333

class MagnitudeSlidersTheme : public LookAndFeel_V4
{
public:
    MagnitudeSlidersTheme ()
    {
    }
    
    ~MagnitudeSlidersTheme ()
    {
        
    }
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour (SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 2, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour (MAGNITUDE_SLIDERS_COLOUR));
        g.fillEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17);
        
        g.setColour(juce::Colour (SLIDER_LABEL_COLOUR));
        g.setFont(juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.0f));
        g.drawText(juce::String(slider.getValue()), x + 2.0f, y + 2.0f, width, height, juce::Justification::centredBottom);
    }
};

class PhaseSlidersTheme : public LookAndFeel_V4
{
public:
    PhaseSlidersTheme ()
    {
    }
    
    ~PhaseSlidersTheme ()
    {
        
    }
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour (SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 2, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour (PHASE_SLIDERS_COLOUR));
        g.fillEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17);
        
        g.setColour(juce::Colour (SLIDER_LABEL_COLOUR));
        g.setFont(juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.0f));
        g.drawText(juce::String(slider.getValue()), x + 2.0f, y + 2.0f, width, height, juce::Justification::centredBottom);
    }
};

class ButtonTheme : public LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto cornerSize = 6.0f;  // Angoli arrotondati
        
        juce::Colour baseColour = button.getToggleState() ? backgroundColour.darker() : backgroundColour;
        
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.2f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.2f);
        
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
        
        g.setColour(button.findColour(juce::ComboBox::outlineColourId));  // Bordo del pulsante
        g.drawRoundedRectangle(bounds, cornerSize, 2.0f);
    }
    
    
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        g.setFont(juce::Font ("Poppins", 19.0f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
        
        auto textBounds = button.getLocalBounds();
        g.drawFittedText(button.getButtonText(), textBounds, juce::Justification::centred, 1);
    }
};

class LA2AStyleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const int width = 50;
        const int height = 50;

        // Sfondo grigio scuro per l'interruttore, con effetti di ombra se evidenziato
        juce::Colour backgroundColour = shouldDrawButtonAsDown ? juce::Colours::darkgrey : juce::Colours::black;
        g.fillAll(backgroundColour);

        // Definiamo il rettangolo dello switch
        auto switchWidth = width * 0.6f;    // Larghezza del rettangolo (60% della larghezza totale)
        auto switchHeight = height * 0.3f;  // Altezza del rettangolo (30% dell'altezza totale)
        juce::Rectangle<float> switchRect((width - switchWidth) / 2, (height - switchHeight) / 2, switchWidth, switchHeight);

        // Disegno della cornice dello switch
        g.setColour(juce::Colours::black);
        g.drawRect(switchRect, 2.0f);

        // Riempimento dello switch: giallo per ON, grigio scuro per OFF
        g.setColour(button.getToggleState() ? juce::Colours::yellow : juce::Colours::darkgrey);
        g.fillRect(switchRect);

        // Disegna le viti in stile vintage
        auto screwDiameter = 3.0f;
        juce::Colour screwColour = juce::Colours::silver;
        g.setColour(screwColour);
        g.fillEllipse(switchRect.getX() - 1, switchRect.getY() - 1, screwDiameter, screwDiameter); // vite in alto a sinistra
        g.fillEllipse(switchRect.getRight() - screwDiameter + 1, switchRect.getY() - 1, screwDiameter, screwDiameter); // vite in alto a destra
        g.fillEllipse(switchRect.getX() - 1, switchRect.getBottom() - screwDiameter + 1, screwDiameter, screwDiameter); // vite in basso a sinistra
        g.fillEllipse(switchRect.getRight() - screwDiameter + 1, switchRect.getBottom() - screwDiameter + 1, screwDiameter, screwDiameter); // vite in basso a destra

        // Etichetta di stato ON/OFF
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        juce::String text = button.getToggleState() ? "ON" : "OFF";
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};
