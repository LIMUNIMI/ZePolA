#pragma once
#include "ParameterPanel.h"
#include <JuceHeader.h>

// =============================================================================
/** Custom look and feel for the GUI  */
class CustomLookAndFeel : public juce::LookAndFeel_V4,
                          public ParameterStripLookAndFeelMethods
{
public:
    // =========================================================================
    enum ColourIDs
    {
        GroupComponent_backgroundColourId = 0,
    };

    // =========================================================================
    CustomLookAndFeel();

    // =========================================================================
    void drawGroupComponentOutline(juce::Graphics&, int width, int height,
                                   const juce::String& text,
                                   const juce::Justification&,
                                   juce::GroupComponent&) override;
    virtual void drawParameterStrip(juce::Graphics&, int width, int height,
                                    ParameterStrip&) override;

    // =========================================================================
    /** Set the new resize ratio */
    void setResizeRatio(float);
    /** Set the new resize ratio by specifying width and height */
    void setResizeRatio(int w, int h);
    /** Get the current resize ratio */
    float getResizeRatio() const;
    /** Resize a given length using the resize ratio */
    template <typename ValueType>
    ValueType resizeSize(ValueType) const;
    /** Get the current width */
    int getResizedWidth() const;
    /** Get the current height */
    int getResizedHeight() const;
    /** Get the current panel outer margin */
    int getResizedPanelOuterMargin() const;
    /** Get the intended aspect ratio */
    float getAspectRatio() const;

    // =========================================================================
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
                      bool vertical = false, float fullMargin = 0.0f) const;
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
                            bool vertical = false) const;
    /** Pad a panel according to the corner size and return the inner rectangle
     * for its content */
    template <typename RectType>
    juce::Rectangle<RectType>
    getPanelInnerRect(const juce::Rectangle<RectType>&) const;

private:
    // =========================================================================
    int fullWidth, fullHeight;
    float resizeRatio;
    float fullPanelOuterMargin, fullPanelMargin;
    float groupComponentThickness, groupComponentCornerSize;
};
