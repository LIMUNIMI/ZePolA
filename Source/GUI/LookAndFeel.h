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
    /** Set the new resize ratio */
    void setResizeRatio(float);
    /** Set the new resize ratio by specifying width and height */
    void setResizeRatio(int w, int h);
    /** Get the current resize ratio */
    float getResizeRatio();
    /** Resize a given length using the resize ratio */
    template <typename ValueType>
    ValueType resizeSize(ValueType);
    /** Get the current width */
    int getResizedWidth();
    /** Get the current height */
    int getResizedHeight();
    /** Get the current panel outer margin */
    int getResizedPanelOuterMargin();
    /** Get the intended aspect ratio */
    float getAspectRatio();

private:
    // ===========================================================================
    int fullWidth, fullHeight;
    float resizeRatio;
    int fullPanelOuterMargin;
    float groupComponentThickness, groupComponentCornerSize;
};
