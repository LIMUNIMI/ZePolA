#pragma once
#include <JuceHeader.h>

#define MAGNITUDE_SLIDERS_COLOUR            0xff1f1f1c
#define PHASE_SLIDERS_COLOUR                0xff6f7071
#define SLIDER_BAR_COLOUR                   0xff797d7f
#define SLIDER_LABEL_COLOUR                 0xff333333
#define SWITCH_BACKGROUND                   0xffe7eaeb
#define OFF_SWITCH_COLOUR                   0xffcacfd2
#define ON_SWITCH_COLOUR                    0xff909497
#define ZEROS_COLOUR                        0xffbb8fce
#define POLES_COLOUR                        0xfff7dc6f
#define BYPASS_ON_COLOUR                    0xffeb685c
#define BYPASS_OFF_COLOUR                   0xff781a13
#define GAIN_SLIDER_COLOUR                  0xff2e86c1

class MagnitudeSliderTheme : public LookAndFeel_V4
{
public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 2, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour(MAGNITUDE_SLIDERS_COLOUR));
        g.fillEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17);
        
        g.setColour(juce::Colour(0xff626567));
        g.drawEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17, 2.0f);
        
        g.setColour(juce::Colour(SLIDER_LABEL_COLOUR));
        g.setFont(juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.0f));
        g.drawText(juce::String(slider.getValue()), x, y + 2.5f, width, height, juce::Justification::centredBottom);
    }
};

class PhaseSliderTheme : public LookAndFeel_V4
{
public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 2, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour(PHASE_SLIDERS_COLOUR));
        g.fillEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17);
        
        g.setColour(juce::Colour(0xff979a9a));
        g.drawEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17, 2.0f);
        
        g.setColour(juce::Colour(SLIDER_LABEL_COLOUR));
        g.setFont(juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.0f));
        g.drawText(juce::String(slider.getValue()), x, y + 2.5f, width, height, juce::Justification::centredBottom);
    }
};

class GainSliderTheme : public LookAndFeel_V4
{
public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 2, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour(GAIN_SLIDER_COLOUR));
        g.fillEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17);
        
        g.setColour(juce::Colour(0xff626567));
        g.drawEllipse(sliderPos - 8.5f, y + height / 2 - 8.5f, 17, 17, 2.0f);
        
        g.setColour(juce::Colour (SLIDER_LABEL_COLOUR));
        g.setFont(juce::Font ("Gill Sans", 15.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.0f));
        g.drawText(juce::String(slider.getValue()), x, y + 2.0f, width, height, juce::Justification::centredBottom);
    }
};

class TypeSwitchTheme : public juce::LookAndFeel_V4
{
public:
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            const float buttonWidth = 52;
            const float buttonHeight = 19;

            auto bounds = button.getLocalBounds().withSizeKeepingCentre(buttonWidth, buttonHeight);

            auto zeroColor = juce::Colour(ZEROS_COLOUR);
            auto poleColor = juce::Colour(POLES_COLOUR);
            auto borderColor = juce::Colours::black;
            auto indicatorColor = juce::Colours::white;

            g.setColour(button.getToggleState() ? zeroColor : poleColor);
            g.fillRoundedRectangle(bounds.toFloat(), 9.5f);

            g.setColour(borderColor);
            g.drawRoundedRectangle(bounds.toFloat(), 9.5f, 1.5f);

            const int indicatorDiameter = 15;
            auto indicatorX = button.getToggleState() ? (bounds.getRight() - indicatorDiameter - 2) : (bounds.getX() + 2);
            auto indicatorY = bounds.getCentreY() - (indicatorDiameter / 2);

            g.setColour(indicatorColor);
            g.fillEllipse(indicatorX, indicatorY, indicatorDiameter, indicatorDiameter);
            
            g.setColour(juce::Colours::black);
            g.setFont(juce::Font(12.0f, juce::Font::bold));
            juce::String text = button.getToggleState() ? "ZERO" : "POLE";
            juce::Rectangle<float> switchRect(bounds.toFloat());
            if (text == "ZERO")
                switchRect.translate(-7.0f, 0.0f);
            else
                switchRect.translate(+7.0f, 0.0f);
            g.drawText(text, switchRect, juce::Justification::centred);
        }
};



class ActiveSwitchTheme : public juce::LookAndFeel_V4
{
public:
    
    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        float switchWidth = 34;
        float switchHeight = 19;
        float xOffset = 2.0f;
        float yOffset = 2.0f;
        
        g.fillAll(juce::Colour(SWITCH_BACKGROUND));

        juce::Rectangle<float> switchRect(xOffset, yOffset, switchWidth, switchHeight);

        auto switchMargin = 3.5f;
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(switchRect, 5.0f, switchMargin);

        g.setColour(button.getToggleState() ? juce::Colour (ON_SWITCH_COLOUR) : juce::Colour (OFF_SWITCH_COLOUR));
        
        if (button.getToggleState())
        {
            switchWidth -= 2.0f;
            switchHeight -= 2.0f;
            xOffset += 1.0f;
            yOffset += 1.0f;
        }
        juce::Rectangle<float> buttonRect(xOffset, yOffset, switchWidth, switchHeight);
        g.fillRoundedRectangle(buttonRect, 5.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        juce::String text = button.getToggleState() ? "ON" : "OFF";
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};

class ResetButtonTheme : public LookAndFeel_V4
{
public:
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const float buttonWidth = 84;
        const float buttonHeight = 26;

        auto bounds = button.getLocalBounds().withSizeKeepingCentre(buttonWidth, buttonHeight).toFloat();
        auto cornerSize = 8.0f;
        
        juce::Colour baseColour = button.getToggleState() ? backgroundColour.darker() : backgroundColour;
        
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.4f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.4f);
        
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
        
        g.setColour(button.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
        
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        juce::String text = "RESET";
        g.drawText(text, bounds, juce::Justification::centred);
    }
};

class BypassSwitchTheme : public juce::LookAndFeel_V4
{
public:
    
    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        float switchWidth = 84;
        float switchHeight = 26;
        float xOffset = 2.0f;
        float yOffset = 2.0f;
        
        g.fillAll(juce::Colour(SWITCH_BACKGROUND));

        juce::Rectangle<float> switchRect(xOffset, yOffset, switchWidth, switchHeight);

        auto switchMargin = 3.5f;
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(switchRect, 5.0f, switchMargin);

        g.setColour(button.getToggleState() ? juce::Colour (BYPASS_ON_COLOUR) : juce::Colour (BYPASS_OFF_COLOUR));
        
        if (button.getToggleState())
        {
            switchWidth -= 2.5f;
            switchHeight -= 2.5f;
            xOffset += 1.25f;
            yOffset += 1.25f;
        }
        juce::Rectangle<float> buttonRect(xOffset, yOffset, switchWidth, switchHeight);
        g.fillRoundedRectangle(buttonRect, 5.0f);

        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        juce::String text = "BYPASS";
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};
