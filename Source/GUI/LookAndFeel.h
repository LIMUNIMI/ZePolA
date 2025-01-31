#pragma once
#include "InvisibleGroupComponent.h"
#include "ParameterPanel.h"
#include "PlotsPanel.h"
#include <JuceHeader.h>

// =============================================================================
/** Transpose rectangle */
template <typename RectType>
juce::Rectangle<RectType>
transposeRectangle(const juce::Rectangle<RectType>& r);
/**
 * Force rectangle to a specific aspect ratio by shrinking it and centering it
 *
 * @param r Outer rectangle
 * @param a Aspect ratio
 * @return Inner rectangle
 */
template <typename RectType>
juce::Rectangle<RectType>
forceAspectRatioCentered(const juce::Rectangle<RectType>& r, float a);

// =============================================================================
/** Custom look and feel for the GUI  */
class CustomLookAndFeel : public juce::LookAndFeel_V4,
                          public InvisibleGroupComponentLookAndFeelMethods,
                          public PlotComponentLookAndFeelMethods,
                          public ParameterPanelLookAndFeelMethods
{
public:
    // =========================================================================
    enum ColourIDs
    {
        GroupComponent_backgroundColourId = 0,
        InvisibleGroupComponent_outlineColourId,
        ParameterStripSeparator_fillColourId,
        OnOffButton_backgroundOnColourId,
        OnOffButton_backgroundOffColourId,
        OnOffButton_ledOnColourId,
        OnOffButton_ledOffColourId,
        OnOffButton_textOnColourId,
        OnOffButton_textOffColourId,
        OnOffButton_outlineColourId,
        PlotComponent_backgroundColourId,
        PlotComponent_gridColourId,
        PlotComponent_gridLabelsColourId,
        PlotComponent_lineColourId,
        PlotComponent_tickLabelColourId
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
    juce::Font getLabelFont(float fullFontSize);
    juce::Font getLabelFont(juce::Typeface::Ptr);
    juce::Font getLabelFont(juce::Typeface::Ptr, float fullFontSize);
    juce::Font getLabelFont();
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width,
                          int height, float sliderPos, float minSliderPos,
                          float maxSliderPos, const juce::Slider::SliderStyle,
                          juce::Slider& slider) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void drawParameterStripSeparators(juce::Graphics&, float x,
                                      std::vector<float> y, float width,
                                      ParameterPanel&) override;
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
    void drawPlotComponent(juce::Graphics&, float x, float y, float width,
                           float height, const std::vector<float>& x_values,
                           const std::vector<float>& y_values, float period,
                           const std::vector<float>& x_grid,
                           const std::vector<float>& y_grid,
                           const std::vector<juce::String>& x_labels,
                           const std::vector<juce::String>& y_labels,
                           bool log_x, const juce::String& topRightText,
                           PlotComponent&) override;

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
    /** Get the lower frequency for the log plot */
    double getLogPlotLowFreq(double sr) const;

    // =========================================================================
    /** Set the properties of the magnitude plot */
    void setMagnitudePlotProperties(PlotComponent&, double sr, bool db = false);
    /** Set the properties of the phase plot */
    void setPhasePlotProperties(PlotComponent&, double sr);
    /** Make linear x ticks */
    std::vector<float> makeLinearXTicks(double sr);
    /** Make logarithmic x ticks */
    std::vector<float> makeLogXTicks(double sr);

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
    /** Resize a toggle button */
    void resizeToggleButton(juce::Component&) const;

private:
    // =========================================================================
    juce::Typeface::Ptr typeface, boldTypeface;

    // =========================================================================
    int fullWidth, fullHeight, fullHeaderHeight, fullPanelOuterMargin;
    float resizeRatio, fullPanelMargin, groupComponentThickness,
        groupComponentCornerSize, fullSeparatorThickness;
    std::vector<int> stripColumnProportions, panelRowProportions,
        panelProportions, lastPanelProportions;

    // =========================================================================
    float osFontScale, topRightTextScale, fullLabelFontSize, fullSliderHeight,
        fullSliderThumbRadius, sliderTextBoxProportionW,
        sliderTextBoxProportionH, inactiveBrightness;
    juce::String fontName;

    // =========================================================================
    float buttonAspectRatio, fullButtonPadding, fullButtonOutline,
        fullButtonRadius;

    // =========================================================================
    int n_x_ticks;
    float fullPlotComponentCornerSize, fullPlotStrokeThickness,
        fullPlotGridThickness, logPlotCenterFreq;
    std::vector<float> logPlotCenterFreqUnits;
    std::vector<float> dbPlotTicks;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
