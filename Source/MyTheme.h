#pragma once
#include <JuceHeader.h>

#define SLIDER_COLOUR       0xffa0a0a0

class Theme : public LookAndFeel_V4
{
public:
    Theme ()
    {
    }
    
    ~Theme ()
    {
        
    }
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colours::grey);
        g.fillRoundedRectangle(x, y + height / 2 - 2, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour (SLIDER_COLOUR));
        g.fillEllipse(sliderPos - 9.0f, y + height / 2 - 9.0f, 18, 18);
        
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font ("Poppins", 14.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
        g.drawText(juce::String(slider.getValue()), x + 4, y + 4, width, height, juce::Justification::centredBottom);
    }
    
    
};
