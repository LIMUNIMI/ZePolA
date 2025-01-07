#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Custom look and feel for the GUI  */
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // ===========================================================================
    enum ColourIDs
    {
        GroupComponent_backgroundColourId = 0,
    };

    // ===========================================================================
    CustomLookAndFeel();

    // ===========================================================================
    void drawGroupComponentOutline(juce::Graphics&, int w, int h,
                                   const juce::String& text,
                                   const juce::Justification&,
                                   juce::GroupComponent&) override;

    // ===========================================================================
    void setResizeRatio(float);

private:
    // ===========================================================================
    float resizeRatio, groupComponentThickness, groupComponentCornerSize;
};