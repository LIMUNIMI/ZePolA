#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Toggle button with labels */
class LabelledToggleButton : public juce::ToggleButton
{
public:
    // =========================================================================
    LabelledToggleButton(const std::vector<juce::String>& labels);

    // =========================================================================
    const juce::String& getCurrentLabel();

    // =========================================================================
    void clicked() override;
    void paintButton(juce::Graphics&, bool, bool) override;

private:
    // =========================================================================
    std::vector<juce::String> labels;
    size_t state;

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
