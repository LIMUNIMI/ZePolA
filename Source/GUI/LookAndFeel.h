#pragma once
#include "InvisibleGroupComponent.h"
#include "ParameterPanel.h"
#include <JuceHeader.h>

// =============================================================================
/** Custom look and feel for the GUI  */
class CustomLookAndFeel : public juce::LookAndFeel_V4,
                          public InvisibleGroupComponentLookAndFeelMethods
{
public:
    // =========================================================================
    enum ColourIDs
    {
        GroupComponent_backgroundColourId = 0,
        InvisibleGroupComponent_outlineColourId
    };

    // =========================================================================
    CustomLookAndFeel();

    // =========================================================================
    void drawGroupComponentOutline(juce::Graphics&, int width, int height,
                                   const juce::String& text,
                                   const juce::Justification&,
                                   juce::GroupComponent&) override;
    void dontDrawGroupComponent(juce::Graphics&, int width, int height,
                                const juce::String& text,
                                const juce::Justification&,
                                juce::GroupComponent&) override;
    juce::Font getLabelFont(juce::Label&) override;
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width,
                          int height, float sliderPos, float minSliderPos,
                          float maxSliderPos, const juce::Slider::SliderStyle,
                          juce::Slider& slider) override;

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
    /**
     * Divide GUI area in panels
     *
     * @return Header bar, parameters panel, plots panel, designer panel, master
     * panel
     */
    std::vector<juce::Rectangle<int>>
    divideInPanels(const juce::Rectangle<int>&) const;
    /** Split rectangle for a parameter strip */
    std::vector<juce::Rectangle<int>>
    splitProportionalStrip(const juce::Rectangle<int>&) const;
    /** Split rectangle for a panel */
    std::vector<juce::Rectangle<int>>
    splitProportionalPanel(const juce::Rectangle<int>&) const;
    /** Resize a slider */
    void resizeSlider(juce::Slider&) const;

private:
    // =========================================================================
    int fullWidth, fullHeight, fullHeaderHeight, fullPanelOuterMargin;
    float resizeRatio, fullPanelMargin, groupComponentThickness,
        groupComponentCornerSize;
    std::vector<int> stripColumnProportions, panelRowProportions,
        panelProportions, lastPanelProportions;

    // =========================================================================
    float fullLabelFontSize, fullSliderHeight, fullSliderThumbRadius,
        sliderTextBoxProportionW, sliderTextBoxProportionH, inactiveBrightness;
    juce::String fontName;
};
