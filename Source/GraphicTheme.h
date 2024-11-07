#pragma once
#include <JuceHeader.h>

// Sliders MACROS
#define SLIDER_PIN_COLOUR                   0xff383838
#define SLIDER_BAR_COLOUR                   0xff797d7f
#define SLIDER_LABEL_COLOUR                 0xff383838
#define BRIGHTER                            0.8
#define GAIN_SLIDER_COLOUR                  0xff2e86c1
#define GAIN_SLIDER_BORDER_COLOUR           0xff626567

// Switches MACROS
#define SWITCH_BACKGROUND                   0xffe7eaeb
#define OFF_SWITCH_COLOUR                   0xffcacfd2
#define ON_SWITCH_COLOUR                    0xff909497
#define LIN_COLOUR                          0xcd3498d8
#define LOG_COLOUR                          0xffe86d5c
#define AMP_COLOUR                          0xffcacfd2
#define DB_COLOUR                           0xff909497
#define LED_ON_COLOUR                       0xffff5f58
#define LED_OFF_COLOUR                      0xff781a13
#define LED_RADIUS                          4.5f
#define ZEROS_COLOUR                        0xd79b59b6
#define POLES_COLOUR                        0xffffbc2e

// Button MACROS
#define BYPASS_ON_COLOUR                    0xffe86d5c
#define BYPASS_OFF_COLOUR                   0xff781a13
#define AUTO_UPDATE_ON_COLOUR               0xff73cc81
#define AUTO_UPDATE_OFF_COLOUR              0xff558a6a


class CustomButton : public juce::TextButton
{
    public:
    CustomButton(const juce::String& buttonName) : juce::TextButton(buttonName) {}
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
};

class CustomToggleButton : public juce::ToggleButton
{
    public:
    CustomToggleButton(const juce::String& buttonName) : juce::ToggleButton(buttonName) {}
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
};

class CustomSlider : public juce::Slider
{
    public:
    CustomSlider(const juce::String& sliderName) : juce::Slider(sliderName) {}
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
};

class CustomLabel : public juce::Label
{
    public:
    CustomLabel(const juce::String& labelName, const juce::String& text) : juce::Label(labelName) {}
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
};

class CustomComboBox : public juce::ComboBox
{
    public:
    CustomComboBox(const juce::String& comboBoxName) : juce::ComboBox(comboBoxName) {}
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
};

class ActiveSliderTheme : public LookAndFeel_V4
{
    public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width,
                           int height, float sliderPos, float minSliderPos,
                           float maxSliderPos, const juce::Slider::SliderStyle,
                           juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR));
        g.fillRoundedRectangle(x, y + height / 2 - 1, width, 2.0f, 0.08f);
        
        g.setColour(juce::Colour(SLIDER_PIN_COLOUR));
        g.fillEllipse(sliderPos - 6.0f, y + height / 2 - 5.5f, 12, 12);
        
        g.setColour(juce::Colours::white);
        g.drawEllipse(sliderPos - 6.0f, y + height / 2 - 5.5f, 12, 12, 1.0f);
    }
    
    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font("Gill Sans", 13.0f, juce::Font::plain);
    }
};

class UnactiveSliderTheme : public LookAndFeel_V4
{
    public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width,
                           int height, float sliderPos, float minSliderPos,
                           float maxSliderPos, const juce::Slider::SliderStyle,
                           juce::Slider& slider) override
    {
        g.setColour(juce::Colour(SLIDER_BAR_COLOUR).brighter(BRIGHTER));
        g.fillRoundedRectangle(x, y + height / 2 - 1, width, 2.0f, 0.08f);
        
        g.setColour(juce::Colour(SLIDER_PIN_COLOUR).brighter(BRIGHTER));
        g.fillEllipse(sliderPos - 6.0f, y + height / 2 - 5.5f, 12, 12);
        
        g.setColour(juce::Colours::white);
        g.drawEllipse(sliderPos - 6.0f, y + height / 2 - 5.5f, 12, 12, 1.0f);
    }
    
    juce::Font getLabelFont(juce::Label& label) override
    {
        return juce::Font("Gill Sans", 13.0f, juce::Font::plain);
    }
};

/*
 The MasterGainSliderTheme class implements a look and feel for phase sliders.
 */
class MasterGainSliderTheme : public LookAndFeel_V4
{
    public:
    
    void drawLinearSlider (juce::Graphics& g, int x, int y, int width,
                           int height, float sliderPos, float minSliderPos,
                           float maxSliderPos, const juce::Slider::SliderStyle,
                           juce::Slider& slider) override
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

/*
 The TypeSwitchTheme class implements a look and feel for type switches.
 */
class TypeSwitchTheme : public juce::LookAndFeel_V4
{
    public:
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override
    {
        const bool isOn = button.getToggleState();
        
        const float cornerRadius = 9.5f;
        const float borderThickness = 1.5f;
        const int indicatorDiameter = 14;
        const auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 8.0f, button.getHeight() - 2.0f);
        
        const auto backgroundColor = isOn ? juce::Colour(ZEROS_COLOUR) : juce::Colour(POLES_COLOUR);
        const auto borderColor = juce::Colours::black;
        const auto indicatorColor = juce::Colours::white;
        
        g.setColour(backgroundColor);
        g.fillRoundedRectangle(bounds.toFloat(), cornerRadius);
        
        g.setColour(borderColor);
        g.drawRoundedRectangle(bounds.toFloat(), cornerRadius, borderThickness);
        
        const auto indicatorX = isOn? (bounds.getRight() - indicatorDiameter - 2) : (bounds.getX() + 2);
        const auto indicatorY = bounds.getCentreY() - (indicatorDiameter / 2) + 0.5f;
        
        g.setColour(indicatorColor);
        g.fillEllipse(indicatorX, indicatorY, indicatorDiameter, indicatorDiameter);
        
        const juce::String text = isOn ? "ZERO" : "POLE";
        auto switchRect = bounds.toFloat();
        switchRect.translate(button.getToggleState() ? -7.0f : 7.0f, 0.0f);
        
        g.setColour(borderColor);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};

/*
 The ActiveSwitchTheme class implements a look and feel for active switches.
 */
class ActiveSwitchTheme : public juce::LookAndFeel_V4
{
    public:
    
    void drawToggleButton(Graphics& g, ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        const bool isOn = button.getToggleState();
        
        const float borderRadius = 5.0f;
        const float borderThickness = 3.5f;
        const float xOffset = 2.0f;
        const float yOffset = 2.0f;
        const float switchWidth = button.getWidth() - 4.0f;
        const float switchHeight = button.getHeight() - 4.0f;
        
        g.fillAll(juce::Colour(SWITCH_BACKGROUND));
        
        juce::Rectangle<float> switchRect(xOffset, yOffset, switchWidth, switchHeight);
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(switchRect, borderRadius, borderThickness);
        
        const float adjustedSwitchOffset = isOn ? 0.75f : 0.0f;
        juce::Rectangle<float> buttonRect(xOffset + adjustedSwitchOffset, yOffset + adjustedSwitchOffset,
                                          switchWidth - adjustedSwitchOffset * 2, switchHeight - adjustedSwitchOffset * 2);
        
        g.setColour(isOn ? juce::Colour(ON_SWITCH_COLOUR) : juce::Colour(OFF_SWITCH_COLOUR));
        g.fillRoundedRectangle(buttonRect, borderRadius);
        
        const juce::Colour ledColour = juce::Colour(isOn ? LED_ON_COLOUR : LED_OFF_COLOUR);
        const juce::String text = isOn ? "ON" : "OFF";
        const float fontSize = 10.0f;
        
        g.setColour(isOn ? juce::Colours::white : juce::Colours::black);
        g.setFont(juce::Font(fontSize, juce::Font::bold));
        
        juce::Rectangle<float> textRect(xOffset - 8, yOffset, switchWidth, switchHeight);
        g.drawText(text, textRect, juce::Justification::centred);
        
        const float ledXOffset = xOffset + 28;
        const float ledYOffset = yOffset + 4;
        
        g.setColour(ledColour);
        g.fillEllipse(ledXOffset, ledYOffset, LED_RADIUS * 2, LED_RADIUS * 2);
        
        g.setColour(juce::Colours::black);
        g.drawEllipse(ledXOffset, ledYOffset, LED_RADIUS * 2, LED_RADIUS * 2, 1.5f);
    }
};

/*
 The BypassSwitchTheme class implements a look and feel for the bypass button.
 */
class BypassSwitchTheme : public juce::LookAndFeel_V4
{
    public:
    
    void drawToggleButton(Graphics& g, ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        const bool isOn = button.getToggleState();
        
        const float switchMargin = 3.5f;
        const float borderRadius = 5.0f;
        float switchWidth = button.getWidth() - 4.0f;
        float switchHeight = button.getHeight() - 4.0f;
        float xOffset = 2.0f;
        float yOffset = 2.0f;
        
        g.fillAll(juce::Colour(SWITCH_BACKGROUND));
        
        juce::Rectangle<float> switchRect(xOffset, yOffset, switchWidth, switchHeight);
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(switchRect, borderRadius, switchMargin);
        
        g.setColour(isOn ? juce::Colour(BYPASS_ON_COLOUR) : juce::Colour(BYPASS_OFF_COLOUR));
        
        if (isOn)
        {
            const float adjustment = 1.25f;
            switchWidth -= adjustment * 2;
            switchHeight -= adjustment * 2;
            xOffset += adjustment;
            yOffset += adjustment;
        }
        
        juce::Rectangle<float> buttonRect(xOffset, yOffset, switchWidth, switchHeight);
        g.fillRoundedRectangle(buttonRect, borderRadius);
        
        g.setColour(juce::Colours::white);
        const float fontSize = isOn ? 11.0f : 12.0f;
        g.setFont(juce::Font("Gill Sans", fontSize, juce::Font::bold).withTypefaceStyle("SemiBold"));
        g.drawText("BYPASS", switchRect, juce::Justification::centred);
    }
};

/*
 The LinLogSwitchTheme class implements a look and feel for the lin/log switch.
 */
class LinLogSwitchTheme : public juce::LookAndFeel_V4
{
    public:
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const bool isOn = button.getToggleState();
        
        const float cornerRadius = 9.5f;
        const float borderThickness = 1.5f;
        const int indicatorDiameter = 14;
        
        auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 8.0f, button.getHeight() - 2.0f);
        
        const auto linColor = juce::Colour(LIN_COLOUR);
        const auto logColor = juce::Colour(LOG_COLOUR);
        const auto borderColor = juce::Colours::black;
        const auto indicatorColor = juce::Colours::white;
        
        g.setColour(isOn ? linColor : logColor);
        g.fillRoundedRectangle(bounds.toFloat(), cornerRadius);
        
        g.setColour(borderColor);
        g.drawRoundedRectangle(bounds.toFloat(), cornerRadius, borderThickness);
        
        const auto indicatorX = isOn ? (bounds.getRight() - indicatorDiameter - 2) : (bounds.getX() + 2);
        const auto indicatorY = bounds.getCentreY() - (indicatorDiameter / 2) + 0.5f;
        
        g.setColour(indicatorColor);
        g.fillEllipse(indicatorX, indicatorY, indicatorDiameter, indicatorDiameter);
        
        g.setColour(juce::Colours::black);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        
        juce::String text = isOn ? "LIN" : "LOG";
        juce::Rectangle<float> switchRect = bounds.toFloat();
        
        auto xOffset = 7.0f;
        if (isOn)
            xOffset = -7.0f;
        
        switchRect.translate(xOffset, 0.0f);
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};

/*
 The AmpDbSwitchTheme class implements a look and feel for the amp/dB switch.
 */
class AmpDbSwitchTheme : public juce::LookAndFeel_V4
{
    public:
    
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const bool isOn = button.getToggleState();
        
        const float cornerRadius = 9.5f;
        const float borderThickness = 1.5f;
        const int indicatorDiameter = 14;
        
        auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 8.0f, button.getHeight() - 2.0f);
        
        const auto ampColor = juce::Colour(AMP_COLOUR);
        const auto dbColor = juce::Colour(DB_COLOUR);
        const auto borderColor = juce::Colours::black;
        const auto indicatorColor = juce::Colours::white;
        
        g.setColour(isOn ? ampColor : dbColor);
        g.fillRoundedRectangle(bounds.toFloat(), cornerRadius);
        
        g.setColour(borderColor);
        g.drawRoundedRectangle(bounds.toFloat(), cornerRadius, borderThickness);
        
        const auto indicatorX = isOn ? (bounds.getRight() - indicatorDiameter - 2) : (bounds.getX() + 2);
        const auto indicatorY = bounds.getCentreY() - (indicatorDiameter / 2) + 0.5f;
        
        g.setColour(indicatorColor);
        g.fillEllipse(indicatorX, indicatorY, indicatorDiameter, indicatorDiameter);
        
        g.setColour(juce::Colours::white);
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        
        juce::String text = isOn ? "AMP" : "DB";
        juce::Rectangle<float> switchRect = bounds.toFloat();
        
        auto xOffset = 7.0f;
        if (isOn)
        {
            g.setColour(juce::Colours::black);
            xOffset = -7.0f;
        }
        
        switchRect.translate(xOffset, 0.0f);
        g.drawText(text, switchRect, juce::Justification::centred);
    }
};

class GenericButtonTheme : public LookAndFeel_V4
{
    public:
    GenericButtonTheme (juce::String textToDisplay = "", float size = 10.0f) : text(textToDisplay), fontSize(size) {}
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 2.0f, button.getHeight() - 2.0f).toFloat();
        auto cornerSize = 5.0f;
        auto marginSize = 1.5f;
        
        juce::Colour baseColour = button.getToggleState() ? backgroundColour.darker() : backgroundColour;
        
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.4f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.4f);
        
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
        
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(bounds, cornerSize, marginSize);
        
        juce::Font font("Gill Sans", fontSize, juce::Font::bold);
        font.setTypefaceStyle("SemiBold");
        g.setFont(font);
        g.drawText(text, bounds, juce::Justification::centred);
    }
    
    void setTextToDisplay (juce::String textToDisplay)
    {
        text = textToDisplay;
    }
    
    void setFontSize (float size)
    {
        fontSize = size;
    }
    
    private:
    juce::String text = "";
    float fontSize;
};

class MenuButtonTheme : public LookAndFeel_V4
{
    public:
    enum buttonFunction { UNDO, REDO, SAVE_PRST, LOAD_PRST };
    
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().withSizeKeepingCentre(button.getWidth() - 2.0f, button.getHeight() - 2.0f).toFloat();
        auto cornerSize = 5.0f;
        auto marginSize = 1.5f;
        
        juce::Colour baseColour = button.getToggleState() ? backgroundColour.darker() : backgroundColour;
        
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker(0.4f);
        else if (shouldDrawButtonAsHighlighted)
            baseColour = baseColour.brighter(0.4f);
        
        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);
    
        g.drawRoundedRectangle(bounds, cornerSize, marginSize);
        
        g.setColour(juce::Colour(0xc8333333));
        juce::Font font("Gill Sans", fontSize, juce::Font::bold);
        font.setTypefaceStyle("SemiBold");
        g.setFont(font);
        g.drawText(text, bounds, juce::Justification::centredLeft);
    }
    
    void setButtonFunction (buttonFunction func)
    {
        function = func;
        switch (func)
        {
            case UNDO: text = "UNDO"; break;
            case REDO: text = "REDO"; break;
            case SAVE_PRST: text = "SAVE PRESET"; break;
            case LOAD_PRST: text = "LOAD PRESET"; break;
        }
    }
    
    private:
    juce::String text = "";
    float fontSize = 13.0f;
    buttonFunction function;
};

/*
 The ComboBoxTheme class implements a look and feel for a combo box.
 */
class ComboBoxTheme : public juce::LookAndFeel_V4
{
    public:
    ComboBoxTheme()
    {
        juce::Colour colour = juce::Colour(0xff909497).darker(0.2);
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff909497).darker(0.2));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff252525));
        setColour(juce::ComboBox::arrowColourId, juce::Colours::white);
        setColour(juce::PopupMenu::backgroundColourId, colour.brighter(0.05));
        setColour(juce::PopupMenu::textColourId, juce::Colours::white);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::whitesmoke);
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);
    }
    
    juce::Font getComboBoxFont(juce::ComboBox& box) override
    {
        return juce::Font("Gill Sans", 12.0f, juce::Font::plain);
    }
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
    {
        auto cornerSize = 5.0f;
        juce::Rectangle<int> boxBounds(0, 0, width, height);
        
        g.setColour(findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
        
        g.setColour(findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f), cornerSize, 1.0f);
        
        juce::Path arrowPath;
        float baseWidth = 8.0f;
        float arrowHeight = 5.0f;
        
        arrowPath.addTriangle(0.0f, 0.0f, baseWidth, 0.0f, baseWidth * 0.5f, arrowHeight);
        
        auto arrowX = buttonX + buttonW * 0.5f - (baseWidth * 0.5f);
        auto arrowY = buttonY + buttonH * 0.5f - (arrowHeight * 0.5f);
        
        g.setColour(findColour(juce::ComboBox::arrowColourId));
        g.fillPath(arrowPath, juce::AffineTransform::translation(arrowX, arrowY));
    }
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        auto cornerSize = 5.0f;
        juce::Rectangle<float> bounds(0, 0, width, height);
        
        g.fillAll(juce::Colour(0xffc4c6c7));
        
        g.setColour(findColour(juce::PopupMenu::backgroundColourId));
        g.fillRoundedRectangle(bounds, cornerSize);
        
        g.setColour(findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds.reduced(0.5f), cornerSize, 1.0f);
    }
    
    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           const bool isSeparator, const bool isActive,
                           const bool isHighlighted, const bool isTicked,
                           const bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText, const juce::Drawable* icon,
                           const juce::Colour* textColour) override
    {
        if (isSeparator)
        {
            juce::Rectangle<int> r(area.reduced(5, 0));
            g.setColour(findColour(juce::PopupMenu::textColourId).withAlpha(0.3f));
            g.fillRect(r.removeFromTop(1));
        }
        else
        {
            auto textColourToUse = findColour(juce::PopupMenu::textColourId);
            
            if (textColour != nullptr)
                textColourToUse = *textColour;
            
            if (isHighlighted)
            {
                g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
                g.fillRect(area);
                
                textColourToUse = findColour(juce::PopupMenu::highlightedTextColourId);
            }
            
            g.setColour(textColourToUse);
            g.setFont(juce::Font("Gill Sans", 12.0f, juce::Font::plain));
            g.drawFittedText(text, area.reduced(10, 0), juce::Justification::centredLeft, 1);
        }
    }
};

class AutoUpdateSwitchTheme : public juce::LookAndFeel_V4
{
    public:
    
    void drawToggleButton(Graphics& g, ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        const bool isOn = button.getToggleState();
        
        const float borderRadius = 5.0f;
        const float switchMargin = 3.5f;
        const float adjustment = 0.75f;
        
        float switchWidth = button.getWidth() - 4.0f;
        float switchHeight = button.getHeight() - 4.0f;
        float xOffset = 2.0f;
        float yOffset = 2.0f;
        
        g.fillAll(juce::Colour(SWITCH_BACKGROUND));
        
        juce::Rectangle<float> switchRect(xOffset, yOffset, switchWidth, switchHeight);
        g.setColour(juce::Colours::black);
        g.drawRoundedRectangle(switchRect, borderRadius, switchMargin);
        
        g.setColour(isOn ? juce::Colour(AUTO_UPDATE_ON_COLOUR) : juce::Colour(AUTO_UPDATE_OFF_COLOUR));
        
        if (isOn)
        {
            switchWidth -= adjustment * 2;
            switchHeight -= adjustment * 2;
            xOffset += adjustment;
            yOffset += adjustment;
        }
        
        juce::Rectangle<float> buttonRect(xOffset, yOffset, switchWidth, switchHeight);
        g.fillRoundedRectangle(buttonRect, borderRadius);
        
        g.setColour(juce::Colours::black);
        const float fontSize = isOn ? 11.5f : 12.0f;
        g.setFont(juce::Font("Gill Sans", fontSize, juce::Font::bold).withTypefaceStyle("SemiBold"));
        g.drawText("AUTO UPDATE", switchRect, juce::Justification::centred);
    }
};

class WarningWindowTheme : public juce::LookAndFeel_V4
{
public:
    WarningWindowTheme()
    {
        setColour(juce::AlertWindow::backgroundColourId, juce::Colours::darkgrey);
        setColour(juce::AlertWindow::textColourId, juce::Colours::white);
        setColour(juce::AlertWindow::outlineColourId, juce::Colours::orange);
    }
};

