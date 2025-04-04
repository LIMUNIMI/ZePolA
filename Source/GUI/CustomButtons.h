#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Get the appropriate button label */
juce::String Button_getOnOffLabel(const juce::Button&, bool state);
/** Get the appropriate button label */
juce::String Button_getOnOffLabel(const juce::Button&);
/** Set the button label for both states */
void Button_setOnOffLabel(juce::Button&, const juce::String& labelOff,
                          const juce::String& labelOn);

// =============================================================================
/** Checkbox class */
class ToggleButtonCheckbox : public juce::ToggleButton
{
public:
    // =========================================================================
    ToggleButtonCheckbox();

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButtonCheckbox)
};

// =============================================================================
/** Toggle button with labels */
class LabelledToggleButton : public juce::ToggleButton
{
public:
    // =========================================================================
    LabelledToggleButton(const juce::String& label0, const juce::String& label1,
                         int colourID0, int colourID1, bool ledPosition0,
                         bool ledPosition1);

    // =========================================================================
    const juce::String& getCurrentLabel() const;
    int getCurrentColourID() const;
    bool getCurrentLedPosition() const;

    // =========================================================================
    void paintButton(juce::Graphics&, bool, bool) override;

private:
    // =========================================================================
    juce::String label0, label1;
    int colourID0, colourID1;
    bool ledPosition0, ledPosition1;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelledToggleButton)
};

// =============================================================================
/** Look and feel methods for LabelledToggleButton */
class LabelledToggleButtonLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void drawLabelledToggleButton(juce::Graphics& g,
                                          LabelledToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
        = 0;
};

// =============================================================================
class TextAndImageButton : public juce::Button
{
public:
    // =============================================================================
    TextAndImageButton(const juce::String&, juce::Drawable*,
                       const juce::Justification&);

    // =============================================================================
    void resized() override;

protected:
    // =========================================================================
    void paintButton(Graphics&, bool shouldDrawButtonAsHighlighted,
                     bool shouldDrawButtonAsDown) override;

private:
    // =========================================================================
    std::unique_ptr<juce::Drawable> image;
    juce::Label label;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextAndImageButton)
};
