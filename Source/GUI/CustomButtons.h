#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Toggle button with labels */
class LabelledToggleButton : public juce::ToggleButton
{
public:
    // =========================================================================
    LabelledToggleButton(const std::vector<juce::String>& labels,
                         const std::vector<int>& colourIDs,
                         const std::vector<bool>& ledPositions);

    // =========================================================================
    size_t getCurrentState() const;
    const juce::String& getCurrentLabel() const;
    int getCurrentColourID() const;
    bool getCurrentLedPosition() const;

    // =========================================================================
    void clicked() override;
    void paintButton(juce::Graphics&, bool, bool) override;

private:
    // =========================================================================
    std::vector<juce::String> labels;
    std::vector<int> colourIDs;
    std::vector<bool> ledPositions;
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
