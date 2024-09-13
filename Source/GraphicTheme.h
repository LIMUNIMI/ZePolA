#pragma once
#include <JuceHeader.h>

#define MAGNITUDE_SLIDERS_COLOUR            0xff1f1f1c
#define PHASE_SLIDERS_COLOUR                0xff6f7071
#define SLIDER_BAR_COLOUR                   0xff797d7f
#define SLIDER_LABEL_COLOUR                 0xff333333
#define SWITCH_BACKGROUND                   0xffe7eaeb
#define OFF_SWITCH_COLOUR                   0xffcacfd2
#define ON_SWITCH_COLOUR                    0xff909497
#define ZEROS_COLOUR                        0xd79b59b6
#define POLES_COLOUR                        0xffffbc2e
#define BYPASS_ON_COLOUR                    0xffff5f58
#define BYPASS_OFF_COLOUR                   0xff781a13
#define GAIN_SLIDER_COLOUR                  0xff2e86c1
#define GAIN_SLIDER_BORDER_COLOUR           0xff626567
#define LIN_COLOUR                          0xcd3498d8
#define LOG_COLOUR                          0xcde74c3c

class MagnitudeSliderTheme : public LookAndFeel_V4
{
public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 1, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour(MAGNITUDE_SLIDERS_COLOUR));
        g.fillEllipse(sliderPos - 6.0f, y + height / 2 - 6.0f, 12, 12);
        
        g.setColour(juce::Colours::white);
        g.drawEllipse(sliderPos - 6.0f, y + height / 2 - 6.0f, 12, 12, 1.0f);
    }
    
    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font("Gill Sans", 15.0f, juce::Font::plain);
    }
};

class PhaseSliderTheme : public LookAndFeel_V4
{
public:
    
    PhaseSliderTheme (const double sampleRate)
    {
        nyquistFreq = sampleRate / 2.0;
    }
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 1, width, 2.5f, 0.08f);
        
        g.setColour(juce::Colour(PHASE_SLIDERS_COLOUR));
        g.fillEllipse(sliderPos - 6.0f, y + height / 2 - 6.0f, 12, 12);
        
        g.setColour(juce::Colours::white);
        g.drawEllipse(sliderPos - 6.0f, y + height / 2 - 6.0f, 12, 12, 1.0f);
    }
    
    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font("Gill Sans", 15.0f, juce::Font::plain);
    }

    
    
private:
    double nyquistFreq;
};

class GainSliderTheme : public LookAndFeel_V4
{
public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x + width / 2 - 1.25f, y, 2.5f, height, 0.08f);
        
        g.setColour(juce::Colour(GAIN_SLIDER_COLOUR));
        g.fillEllipse(x + width / 2 - 9.5f, sliderPos - 9.5f, 19, 19);
        
        g.setColour(juce::Colour(GAIN_SLIDER_BORDER_COLOUR));
        g.drawEllipse(x + width / 2 - 9.5f, sliderPos - 9.5f, 19, 19, 2.0f);
    }
    
    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font("Gill Sans", 17.0f, juce::Font::plain);
    }
};

class TypeSwitchTheme : public juce::LookAndFeel_V4
{
public:
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 8.0f, button.getHeight() - 2.0f);

            auto zeroColor = juce::Colour(ZEROS_COLOUR);
            auto poleColor = juce::Colour(POLES_COLOUR);
            auto borderColor = juce::Colours::black;
            auto indicatorColor = juce::Colours::white;

            g.setColour(button.getToggleState() ? zeroColor : poleColor);
            g.fillRoundedRectangle(bounds.toFloat(), 9.5f);

            g.setColour(borderColor);
            g.drawRoundedRectangle(bounds.toFloat(), 9.5f, 1.5f);

            const int indicatorDiameter = 14;
            auto indicatorX = button.getToggleState() ? (bounds.getRight() - indicatorDiameter - 2) : (bounds.getX() + 2);
            auto indicatorY = bounds.getCentreY() - (indicatorDiameter / 2) + 0.5;

            g.setColour(indicatorColor);
            g.fillEllipse(indicatorX, indicatorY, indicatorDiameter, indicatorDiameter);
            
            g.setColour(juce::Colours::black);
            g.setFont(juce::Font(10.0f, juce::Font::bold));
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
        float switchWidth = button.getWidth() - 4.0f;
        float switchHeight = button.getHeight() - 4.0f;
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
            switchWidth -= 1.5f;
            switchHeight -= 1.5f;
            xOffset += .75f;
            yOffset += .75f;
        }
        juce::Rectangle<float> buttonRect(xOffset, yOffset, switchWidth, switchHeight);
        g.fillRoundedRectangle(buttonRect, 5.0f);

        g.setColour(juce::Colours::white);
        float fontSize = button.getToggleState() ? 10.0f : 10.5f;
        g.setFont(juce::Font(fontSize, juce::Font::bold));
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
        auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 2.0f, button.getHeight() - 2.0f).toFloat();
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
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        juce::String text = "RESET";
        g.drawText(text, bounds, juce::Justification::centred);
    }
};

class BypassSwitchTheme : public juce::LookAndFeel_V4
{
public:
    
    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        float switchWidth = button.getWidth() - 4.0f;
        float switchHeight = button.getHeight() - 4.0f;
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
        float fontSize = button.getToggleState() ? 10.5f : 11.0f;
        g.setFont(juce::Font(fontSize, juce::Font::bold));
        juce::String text = "BYPASS";
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};

class LinLogSwitchTheme : public juce::LookAndFeel_V4
{
public:
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 8.0f, button.getHeight() - 2.0f);

            auto linColor = juce::Colour(LIN_COLOUR);
            auto logColor = juce::Colour(LOG_COLOUR);
            auto borderColor = juce::Colours::black;
            auto indicatorColor = juce::Colours::white;

            g.setColour(button.getToggleState() ? linColor : logColor);
            g.fillRoundedRectangle(bounds.toFloat(), 9.5f);

            g.setColour(borderColor);
            g.drawRoundedRectangle(bounds.toFloat(), 9.5f, 1.5f);

            const int indicatorDiameter = 14;
            auto indicatorX = button.getToggleState() ? (bounds.getRight() - indicatorDiameter - 2) : (bounds.getX() + 2);
            auto indicatorY = bounds.getCentreY() - (indicatorDiameter / 2) + 0.5;

            g.setColour(indicatorColor);
            g.fillEllipse(indicatorX, indicatorY, indicatorDiameter, indicatorDiameter);
            
            g.setColour(juce::Colours::black);
            g.setFont(juce::Font(10.0f, juce::Font::bold));
            juce::String text = button.getToggleState() ? "LIN" : "LOG";
            juce::Rectangle<float> switchRect(bounds.toFloat());
            if (text == "LIN")
                switchRect.translate(-7.0f, 0.0f);
            else
                switchRect.translate(+7.0f, 0.0f);
            g.drawText(text, switchRect, juce::Justification::centred);
        }
};
