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

    // ===========================================================================
    /**
     * @brief Split rectangle in proportional rectangles
     *
     * @param r Rectangle to split
     * @param fractions Proportional size of each rectangle
     * @param vertical If true, then split rectangle vertically. If false
     * (default), horizontally
     * @param fullMargin Space between rectangles (when GUI is at full size)
     * @return Vector of proportional rectangles
     */
    template <typename RectType>
    std::vector<juce::Rectangle<RectType>>
    splitProportional(const juce::Rectangle<RectType>& r,
                      const std::vector<RectType>& fractions,
                      bool vertical = false, float fullMargin = 0.0f);
    /**
     * @brief Split rectangle in proportional rectangles, with resized panel
     * margin
     *
     * @param r Rectangle to split
     * @param fractions Proportional size of each rectangle
     * @param vertical If true, then split rectangle vertically. If false
     * (default), horizontally
     * @return Vector of proportional rectangles
     */
    template <typename RectType>
    std::vector<juce::Rectangle<RectType>>
    splitProportionalPanels(const juce::Rectangle<RectType>& r,
                            const std::vector<RectType>& fractions,
                            bool vertical = false);

private:
    // ===========================================================================
    int fullWidth, fullHeight;
    float resizeRatio;
    float fullPanelOuterMargin, fullPanelMargin;
    float groupComponentThickness, groupComponentCornerSize;
};
