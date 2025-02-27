#pragma once
#include <JuceHeader.h>

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
