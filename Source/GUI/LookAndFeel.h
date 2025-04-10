/*
  ==============================================================================

    LookAndFeel.h

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

#pragma once
#include "CustomButtons.h"
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
                          public SeparatorComponent::LookAndFeelMethods,
                          public PlotComponentLookAndFeelMethods,
                          public GaussianPlanePanel::LookAndFeelMethods,
                          public ZPoint::LookAndFeelMethods,
                          public LabelledToggleButtonLookAndFeelMethods
{
public:
    // =========================================================================
    enum ColourIDs
    {
        GroupComponent_backgroundColourId = 0,
        GroupComponent_warningBackgroundColourId,
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
        PlotComponent_tickLabelColourId,
        GaussianPlanePanel_circleColourId,
        GaussianPlanePanel_gridColourId,
        ZPoint_zerosColourId,
        ZPoint_polesColourId,
        PlotButtons_linColourId,
        PlotButtons_logColourId
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
    void drawSeparator(juce::Graphics&, int x, int y, int width, int height,
                       bool drawTop, bool drawBottom, bool drawLeft,
                       bool drawRight, SeparatorComponent&) override;
    juce::Font getPopupMenuFont() override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getCustomFont(juce::Typeface::Ptr, float fontSize);
    juce::Font getCustomFont(float fontSize);
    juce::Font getCustomFont(juce::Typeface::Ptr);
    juce::Font getCustomFontResized(juce::Typeface::Ptr, float fullFontSize);
    juce::Font getCustomFontResized(float fullFontSize);
    juce::Font getCustomFont();
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width,
                          int height, float sliderPos, float minSliderPos,
                          float maxSliderPos, const juce::Slider::SliderStyle,
                          juce::Slider& slider) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
    void drawLabelledToggleButton(juce::Graphics& g,
                                  LabelledToggleButton& button,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&,
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
    void drawGaussianPlane(juce::Graphics&, float x, float y, float width,
                           float height, float radius,
                           GaussianPlanePanel&) override;
    void drawZPoint(juce::Graphics&, float x, float y, float width,
                    float height, float p_x, float p_y, bool type,
                    bool conjugate, bool single, bool inverted, ZPoint&);
    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox&) override;
    void drawPopupMenuBackground(juce::Graphics&, int width,
                                 int height) override;
    void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>& area,
                           const bool isSeparator, const bool isActive,
                           const bool isHighlighted, const bool isTicked,
                           const bool hasSubMenu, const juce::String& text,
                           const juce::String& shortcutKeyText,
                           const juce::Drawable* icon,
                           const juce::Colour* textColour) override;

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
    /** Get proportional size of the points in the Gaussian plane */
    float getRelativePointSize() const;
    /** Get the current group component outline thickness */
    float getGroupComponentThickness() const;

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
    /** Split rectangle for the linear/logarithmic switches row */
    std::vector<juce::Rectangle<int>>
    splitProportionalLinLogRow(const juce::Rectangle<int>&) const;
    /** Split rectangle for the shortcut buttons column */
    std::vector<juce::Rectangle<int>>
    splitProportionalShortcuts(const juce::Rectangle<int>&) const;
    /** Configure rectangles and sizes for the designer panel */
    std::vector<juce::Rectangle<int>>
    configureDesignerPanel(const juce::Rectangle<int>&, int* row_height,
                           int* spacer_height) const;
    /** Configure rectangles and sizes for the master control panel */
    std::vector<juce::Rectangle<int>>
    configureMasterPanel(const juce::Rectangle<int>&) const;

private:
    // =========================================================================
    juce::Typeface::Ptr typeface, boldTypeface;

    // =========================================================================
    int fullWidth, fullHeight, fullHeaderHeight, fullPanelOuterMargin;
    float resizeRatio, fullPanelMargin, groupComponentThickness,
        groupComponentCornerSize, fullSeparatorThickness;
    std::vector<int> stripColumnProportions, panelRowProportions,
        panelProportions, lastPanelProportions, linLogSwitchesHeightProportions,
        linLogSwitchesRowProportions, shortcutsWidthProportions,
        shortcutsColumnProportions;

    // =========================================================================
    float topRightTextScale, fullLabelFontSize, fullSliderHeight,
        fullSliderThumbRadius, sliderTextBoxProportionW,
        sliderTextBoxProportionH, inactiveBrightness;
    juce::String fontName;

    // =========================================================================
    float buttonAspectRatio, fullButtonPadding, fullButtonOutline,
        fullLabelledButtonOutline, relativeButtonRadius,
        relativeLabelledButtonRadius;

    // =========================================================================
    int n_x_ticks;
    float fullPlotComponentCornerSize, fullPlotStrokeThickness,
        fullPlotGridThickness, logPlotCenterFreq;
    std::vector<float> logPlotCenterFreqUnits;
    std::vector<float> dbPlotTicks;

    // =========================================================================
    float fullGaussianCircleThickness, fullGaussianMinorThickness,
        relativePointSize, fullPointThickness, singlePointRelativeThickness,
        conjugateAlpha;
    int nGaussianCircleMajorTicks, nGaussianCircleMinorTicksRadial,
        nGaussianCircleMinorTicksCircular;

    // =========================================================================
    float designerParamToSpacerRatio, designerLastRowFraction,
        fullComboBoxArrowWidth, fullComboBoxArrowHeight,
        popupMenuSeparatorTextAlpha;
    int designerMaxParams, designerMaxSpacers;
    std::vector<int> designerLastRowProportions;

    // =========================================================================
    int fullMasterLabelSize, fullMasterButtonSize;

    // =========================================================================
    void _drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown, float buttonOutline,
                           float buttonPadding, float relativeRadius,
                           juce::Colour backgroundColour,
                           juce::Colour ledColour, juce::Colour outlineColour,
                           juce::Colour ledOutlineColour,
                           juce::Colour textColour, const juce::String& label,
                           bool ledSide);
    void _drawCheckbox(Graphics&, juce::ToggleButton&,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown);
    template <typename ValueType>
    void _autoFontScale(juce::Font&, const juce::Rectangle<ValueType>&,
                        const juce::String&);

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
