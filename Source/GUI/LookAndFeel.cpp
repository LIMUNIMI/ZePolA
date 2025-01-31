#include "LookAndFeel.h"
#include "../Macros.h"
#include "../Mappers.h"
#include "ParameterPanel.h"

// =============================================================================
template <typename RectType>
juce::Rectangle<RectType> transposeRectangle(const juce::Rectangle<RectType>& r)
{
    return {r.getY(), r.getX(), r.getHeight(), r.getWidth()};
}
template <typename RectType>
juce::Rectangle<RectType>
forceAspectRatioCentered(const juce::Rectangle<RectType>& r, float a)
{
    if (!r.getHeight() || !r.getWidth()) return r;

    float scale    = a * r.getHeight() / r.getWidth();
    bool transpose = scale > 1.0f;

    juce::Rectangle<RectType> out_r(r);
    if (transpose)
    {
        scale = 1.0f / scale;
        out_r = transposeRectangle(out_r);
    }

    out_r.setWidth(static_cast<RectType>(out_r.getWidth() * scale));

    if (transpose) out_r = transposeRectangle(out_r);
    out_r.setCentre(r.getCentre());
    return out_r;
}

// =============================================================================
CustomLookAndFeel::CustomLookAndFeel()
    : resizeRatio(1.0f)
    , fullWidth(1200)
    , fullHeight(790)
    , fullHeaderHeight(40)
    , fullPanelOuterMargin(15)
    , fullPanelMargin(0.0f)
    , groupComponentThickness(1.5f)
    , groupComponentCornerSize(14.5f)
    // radius, angle, frequency, type, active, gain
    , stripColumnProportions({100, 100, 50, 50, 50, 50})
    , panelRowProportions({25, 450, 50, 450, 25})
    , panelProportions({510, 480, 180})
    , lastPanelProportions({396, 324})
    , fontName("Gill Sans")
    , fullLabelFontSize(20.0f)
    , fullSliderHeight(2.0f)
    , fullSliderThumbRadius(4.5f)
    , inactiveBrightness(0.8f)
    , sliderTextBoxProportionW(50.0f / 120.0f)
    , sliderTextBoxProportionH(0.5f)
    , fullSeparatorThickness(1.0f)
    , buttonAspectRatio(2.5f)
    , fullButtonPadding(5.0f)
    , fullButtonOutline(2.5f)
    , relativeButtonRadius(0.3f)
    , fullPlotComponentCornerSize(6.0f)
    , fullPlotStrokeThickness(1.5f)
    , fullPlotGridThickness(1.0f)
    , n_x_ticks(9)
    , logPlotCenterFreq(1000.0f)
    , logPlotCenterFreqUnits({1.0f, 2.0f, 5.0f})
    , dbPlotTicks({6.0f, 12.0f, 20.0f, 40.0f, 60.0f})
    , typeface(juce::Typeface::createSystemTypefaceFor(
          BinaryData::MuktaSemiBold_ttf, BinaryData::MuktaSemiBold_ttfSize))
    , boldTypeface(juce::Typeface::createSystemTypefaceFor(
          BinaryData::MuktaBold_ttf, BinaryData::MuktaBold_ttfSize))
    , osFontScale(
#ifdef JUCE_WINDOWS
          1.5f
#else
#ifdef JUCE_LINUX
          1.0f
#else
#ifdef JUCE_MAC
          1.0f
#else
          1.0f
#endif
#endif
#endif
          )
    , topRightTextScale(0.75f)
{
    // Panels
    setColour(juce::ResizableWindow::backgroundColourId,
              juce::Colour(0xffecf0f1));
    setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff383838));
    setColour(GroupComponent_backgroundColourId, juce::Colour(0x17b1b1b1));
    setColour(InvisibleGroupComponent_outlineColourId,
              juce::Colours::transparentBlack);
    setColour(ParameterStripSeparator_fillColourId, juce::Colour(0xffdee0e1));
    // Labels
    setColour(juce::Label::textColourId, juce::Colour(0xff383838));
    setColour(juce::TextEditor::textColourId, juce::Colours::black);
    setColour(juce::TextEditor::backgroundColourId,
              juce::Colours::transparentBlack);
    setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Label::backgroundWhenEditingColourId,
              findColour(juce::Label::backgroundColourId));
    setColour(juce::Label::textWhenEditingColourId,
              findColour(juce::Label::textColourId));
    setColour(juce::Label::outlineWhenEditingColourId,
              findColour(juce::Label::outlineColourId));
    // Slider
    setColour(juce::Slider::trackColourId, juce::Colour(0xff797d7f));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xff383838));
    setColour(juce::Slider::backgroundColourId,
              juce::Colours::black.withAlpha(0.25f));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
    setColour(juce::Slider::textBoxBackgroundColourId,
              juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(0x66686868));
    setColour(juce::Slider::textBoxOutlineColourId,
              juce::Colours::transparentBlack);
    // Buttons
    setColour(OnOffButton_backgroundOnColourId, juce::Colour(0xff909497));
    setColour(OnOffButton_backgroundOffColourId, juce::Colour(0xffcacfd2));
    setColour(OnOffButton_ledOnColourId, juce::Colour(0xffff5f58));
    setColour(OnOffButton_ledOffColourId, juce::Colour(0xff781a13));
    setColour(OnOffButton_textOnColourId, juce::Colours::white);
    setColour(OnOffButton_textOffColourId, juce::Colours::black);
    setColour(OnOffButton_outlineColourId, juce::Colours::black);
    // Plots
    setColour(PlotComponent_backgroundColourId, juce::Colour(0x45979a9a));
    setColour(PlotComponent_lineColourId, juce::Colours::black);
    setColour(PlotComponent_gridColourId, juce::Colour(0x28979a9a));
    setColour(PlotComponent_gridLabelsColourId, juce::Colour(0xff797d7f));
}

// =============================================================================
void CustomLookAndFeel::setResizeRatio(float f) { resizeRatio = f; }
void CustomLookAndFeel::setResizeRatio(int w, int h)
{
    float rW = static_cast<float>(w) / fullWidth;
    float rH = static_cast<float>(h) / fullHeight;
    setResizeRatio(fminf(rW, rH));
}
template <typename ValueType>
ValueType CustomLookAndFeel::resizeSize(ValueType f) const
{
    return static_cast<ValueType>(f * resizeRatio);
}
int CustomLookAndFeel::getResizedWidth() const { return resizeSize(fullWidth); }
int CustomLookAndFeel::getResizedHeight() const
{
    return resizeSize(fullHeight);
}
float CustomLookAndFeel::getResizeRatio() const { return resizeRatio; }
float CustomLookAndFeel::getAspectRatio() const
{
    return static_cast<float>(fullWidth) / fullHeight;
}
double CustomLookAndFeel::getLogPlotLowFreq(double sr) const
{
    return logPlotCenterFreq * logPlotCenterFreq * 2.0 / sr;
}

// =============================================================================
std::vector<float> CustomLookAndFeel::makeLinearXTicks(double sr)
{
    float x_max = static_cast<float>(sr * 0.5);
    auto q      = pow(10, juce::roundToInt(log10(x_max))) / 200.0;
    auto step   = static_cast<float>(
        juce::roundToInt(x_max / (q * (n_x_ticks - 1))) * q);
    std::vector<float> ticks({0.0f});
    for (float t = step; t < x_max - step * 0.5f; t += step) ticks.push_back(t);
    ticks.push_back(x_max);
    return ticks;
}
std::vector<float> CustomLookAndFeel::makeLogXTicks(double sr)
{
    float x_max = static_cast<float>(sr * 0.5);
    float x_min = static_cast<float>(getLogPlotLowFreq(sr));
    std::vector<float> ticks({x_min});
    float pow_10 = pow(10.0f, floor(log10(x_min)));
    auto n_units = logPlotCenterFreqUnits.size();
    float x;
    for (int i = 0;; ++i)
    {
        if (i >= n_units)
        {
            i %= n_units;
            pow_10 *= 10.0f;
        }
        x = pow_10 * logPlotCenterFreqUnits[i];
        if (x > x_max) break;
        if (x > x_min) ticks.push_back(x);
    }
    ticks.push_back(x_max);
    return ticks;
}
void CustomLookAndFeel::setMagnitudePlotProperties(PlotComponent& pc, double sr,
                                                   bool db)
{
    pc.setPeriod();
    if (db)
    {
        std::vector<float> ticks({0.0f});
        for (auto t : dbPlotTicks)
        {
            ticks.push_back(t);
            ticks.insert(ticks.begin(), -t);
        }
        juce::String suffix(" dB");
        std::vector<juce::String> labels;
        for (auto t : ticks) labels.push_back(juce::String(t) + suffix);
        pc.setYGrid(ticks, labels);
    }
    else
        pc.setYGrid({0.0f, 0.5f, 1.0f, 1.5f, 2.0f});
    pc.setXGrid((pc.getLogX()) ? makeLogXTicks(sr) : makeLinearXTicks(sr));
}
void CustomLookAndFeel::setPhasePlotProperties(PlotComponent& pc, double sr)
{
    pc.setPeriod(juce::MathConstants<float>::twoPi);
    pc.setYGrid({-juce::MathConstants<float>::pi,
                 -juce::MathConstants<float>::halfPi, 0.0f,
                 juce::MathConstants<float>::halfPi,
                 juce::MathConstants<float>::pi},
                {
                    CharPointer_UTF8("-π"),
                    CharPointer_UTF8("-π/2"),
                    CharPointer_UTF8("0"),
                    CharPointer_UTF8("+π/2"),
                    CharPointer_UTF8("+π"),
                });
    pc.setXGrid((pc.getLogX()) ? makeLogXTicks(sr) : makeLinearXTicks(sr));
}

// =============================================================================
template <typename RectType>
std::vector<juce::Rectangle<RectType>>
CustomLookAndFeel::splitProportional(const juce::Rectangle<RectType>& r,
                                     const std::vector<RectType>& fractions,
                                     bool vertical, float fullMargin) const
{
    std::vector<juce::Rectangle<RectType>> rects;
    if (!fractions.empty())
    {
        float fullSize
            = static_cast<float>((vertical) ? r.getHeight() : r.getWidth());
        float resizedMargin = resizeSize(fullMargin);

        fullSize -= resizedMargin * (fractions.size() - 1);
        float fractionsK = 0.0f;
        for (auto f : fractions)
        {
            jassert(f > 0.0f);
            fractionsK += f;
        }
        if (fractionsK) fractionsK = fullSize / fractionsK;

        juce::Rectangle<float> t(r.toFloat());
        for (auto f : fractions)
        {
            float amnt = f * fractionsK;
            juce::Rectangle<float> r_f;
            if (vertical)
            {
                r_f = t.removeFromTop(amnt);
                t.removeFromTop(resizedMargin);
            }
            else
            {
                r_f = t.removeFromLeft(amnt);
                t.removeFromLeft(resizedMargin);
            }
            rects.push_back(r_f.toType<RectType>());
        }
    }

    jassert(rects.size() == fractions.size());
    return rects;
}
template <typename RectType>
std::vector<juce::Rectangle<RectType>>
CustomLookAndFeel::splitProportionalPanels(
    const juce::Rectangle<RectType>& r, const std::vector<RectType>& fractions,
    bool vertical) const
{
    return splitProportional(r, fractions, vertical, fullPanelMargin);
}

template <typename RectType>
juce::Rectangle<RectType>
CustomLookAndFeel::getPanelInnerRect(const juce::Rectangle<RectType>& r) const
{
    auto delta = static_cast<RectType>(resizeSize(groupComponentCornerSize));
    return r.reduced(delta, delta);
}

template juce::Rectangle<int>
CustomLookAndFeel::getPanelInnerRect(const juce::Rectangle<int>&) const;

std::vector<juce::Rectangle<int>>
CustomLookAndFeel::divideInPanels(const juce::Rectangle<int>& r) const
{
    std::vector<juce::Rectangle<int>> rects;

    auto panels_box(r);
    rects.push_back(panels_box.removeFromTop(resizeSize(fullHeaderHeight)));
    panels_box = panels_box.reduced(resizeSize(fullPanelOuterMargin));

    auto v_panels = splitProportionalPanels(panels_box, panelProportions);
    auto n        = v_panels.size() - 1;
    for (auto i = 0; i < n; ++i) rects.push_back(v_panels[i]);
    for (auto r_i :
         splitProportionalPanels(v_panels[n], lastPanelProportions, true))
        rects.push_back(r_i);

    return rects;
}

std::vector<juce::Rectangle<int>>
CustomLookAndFeel::splitProportionalStrip(const juce::Rectangle<int>& r) const
{
    return splitProportional(r, stripColumnProportions);
}

std::vector<juce::Rectangle<int>>
CustomLookAndFeel::splitProportionalPanel(const juce::Rectangle<int>& r) const
{
    return splitProportional(r, panelRowProportions, true);
}

void CustomLookAndFeel::resizeSlider(juce::Slider& s) const
{
    s.setTextBoxStyle(
        s.getTextBoxPosition(), false,
        juce::roundToInt(s.getBounds().getWidth() * sliderTextBoxProportionW),
        juce::roundToInt(s.getBounds().getHeight() * sliderTextBoxProportionH));
}
void CustomLookAndFeel::resizeToggleButton(juce::Component& c) const
{
    auto r = juce::roundToInt(resizeSize(fullButtonPadding));
    auto e = juce::roundToInt(resizeSize(fullButtonOutline * 0.5f));

    auto bounds = c.getBounds();
    bounds.reduce(r, r);
    bounds = forceAspectRatioCentered(bounds, buttonAspectRatio);
    bounds.expand(e, e);
    c.setBounds(bounds);
}

// =============================================================================
void CustomLookAndFeel::drawGroupComponentOutline(
    juce::Graphics& g, int width, int height, const juce::String& /* text */,
    const juce::Justification&, juce::GroupComponent& gp)
{
    juce::Rectangle<float> b(0.0f, 0.0f, static_cast<float>(width),
                             static_cast<float>(height));
    b = b.reduced(resizeSize(groupComponentThickness));
    g.setColour(gp.findColour(GroupComponent_backgroundColourId));
    g.fillRoundedRectangle(b, resizeSize(groupComponentCornerSize));
    g.setColour(gp.findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(b, resizeSize(groupComponentCornerSize),
                           resizeSize(groupComponentThickness));
}
void CustomLookAndFeel::dontDrawGroupComponent(juce::Graphics& g, int width,
                                               int height, const juce::String&,
                                               const juce::Justification&,
                                               juce::GroupComponent& gc)
{
    juce::Rectangle<float> b(0.0f, 0.0f, static_cast<float>(width),
                             static_cast<float>(height));
    g.setColour(gc.findColour(InvisibleGroupComponent_outlineColourId));
    g.drawRect(b, resizeSize(1.0f));
}
juce::Font CustomLookAndFeel::getLabelFont(juce::Typeface::Ptr t,
                                           float fullFontSize)
{
    juce::Font f(t);
    f.setSizeAndStyle(resizeSize(fullFontSize) * osFontScale, f.getStyleFlags(),
                      f.getHorizontalScale(), f.getExtraKerningFactor());
    return f;
}
juce::Font CustomLookAndFeel::getLabelFont(juce::Typeface::Ptr t)
{
    return getLabelFont(t, fullLabelFontSize);
}
juce::Font CustomLookAndFeel::getLabelFont(float fullFontSize)
{
    return getLabelFont(typeface, fullFontSize);
}
juce::Font CustomLookAndFeel::getLabelFont()
{
    return getLabelFont(fullLabelFontSize);
}
juce::Font CustomLookAndFeel::getLabelFont(juce::Label&)
{
    return getLabelFont();
}
void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y,
                                         int width, int height, float sliderPos,
                                         float /* minSliderPos */,
                                         float /* maxSliderPos */,
                                         const juce::Slider::SliderStyle,
                                         juce::Slider& slider)
{
    juce::Colour trackColour = slider.findColour(juce::Slider::trackColourId);
    juce::Colour thumbColour = slider.findColour(juce::Slider::thumbColourId);
    juce::Colour backgroundColour
        = slider.findColour(juce::Slider::backgroundColourId);

    if (!ParameterStrip::parentComponentIsActive(slider))
    {
        trackColour      = trackColour.brighter(inactiveBrightness);
        thumbColour      = thumbColour.brighter(inactiveBrightness);
        backgroundColour = backgroundColour.brighter(inactiveBrightness);
    }

    auto sh = resizeSize(fullSliderHeight);
    g.setColour(trackColour);
    g.fillRoundedRectangle(static_cast<float>(x), y + (height - sh) * 0.5f,
                           static_cast<float>(width), sh, sh * 0.5f);

    auto st_rad = resizeSize(fullSliderThumbRadius);
    juce::Rectangle<float> st_rect(sliderPos - st_rad,
                                   y + height * 0.5f - st_rad, 2.0f * st_rad,
                                   2.0f * st_rad);

    g.setColour(thumbColour);
    g.fillEllipse(st_rect);
    g.setColour(backgroundColour);
    g.drawEllipse(st_rect, resizeSize(1.0f));
}
void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    auto textColour       = label.findColour(Label::textColourId);
    auto outlineColour    = label.findColour(Label::outlineColourId);
    auto backgroundColour = label.findColour(Label::backgroundColourId);

    // Also check if label is inside a ParameterStrip slider
    if (!ParameterStrip::parentComponentIsActive(label)
        || !ParameterStrip::parentComponentIsActive(
            *label.getParentComponent()))
    {
        textColour       = textColour.brighter(inactiveBrightness);
        outlineColour    = outlineColour.brighter(inactiveBrightness);
        backgroundColour = backgroundColour.brighter(inactiveBrightness);
    }

    // --- Code edited from juce::LookAndFeel_V2::drawLabel --------------------
    g.fillAll(backgroundColour);
    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font(getLabelFont(label));

        g.setColour(textColour.withMultipliedAlpha(alpha));
        g.setFont(font);

        auto textArea
            = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

        g.drawFittedText(label.getText(), textArea,
                         label.getJustificationType(),
                         juce::jmax(1, (int)((float)textArea.getHeight()
                                             / font.getHeight())),
                         label.getMinimumHorizontalScale());

        g.setColour(outlineColour.withMultipliedAlpha(alpha));
    }
    else if (label.isEnabled())
    {
        g.setColour(outlineColour);
    }
    g.drawRect(label.getLocalBounds());
    // --- End of code edited from juce::LookAndFeel_V2::drawLabel -------------
}
void CustomLookAndFeel::drawParameterStripSeparators(juce::Graphics& g, float,
                                                     std::vector<float> y,
                                                     float width,
                                                     ParameterPanel& pp)
{
    float x = resizeSize(2.0f * groupComponentThickness);
    float h = resizeSize(fullSeparatorThickness);
    width -= 2.0f * x;
    g.setColour(pp.findColour(ParameterStripSeparator_fillColourId));
    for (auto i : y) g.fillRect(x, i - h * 0.5f, width, h);
}
void CustomLookAndFeel::drawToggleButton(
    juce::Graphics& g, juce::ToggleButton& button,
    bool /* shouldDrawButtonAsHighlighted */, bool /* shouldDrawButtonAsDown */)
{
    bool on      = button.getToggleState();
    float othick = resizeSize(fullButtonOutline);
    float bpad   = resizeSize(fullButtonPadding);
    float radius = relativeButtonRadius * button.getHeight();

    juce::Rectangle<float> rect(0.0f, 0.0f,
                                static_cast<float>(button.getWidth()),
                                static_cast<float>(button.getHeight()));
    rect = rect.reduced(othick * 0.5f);

    auto bgc = button.findColour((on) ? OnOffButton_backgroundOnColourId
                                      : OnOffButton_backgroundOffColourId);
    auto lc  = button.findColour((on) ? OnOffButton_ledOnColourId
                                     : OnOffButton_ledOffColourId);
    auto oc  = button.findColour(OnOffButton_outlineColourId);
    auto tc  = button.findColour((on) ? OnOffButton_textOnColourId
                                     : OnOffButton_textOffColourId);
    if (!ParameterStrip::parentComponentIsActive(button))
    {
        bgc = bgc.brighter(inactiveBrightness);
        lc  = lc.brighter(inactiveBrightness);
        oc  = oc.brighter(inactiveBrightness);
        tc  = tc.brighter(inactiveBrightness);
    }

    g.setColour(bgc);
    g.fillRoundedRectangle(rect, radius);
    g.setColour(oc);
    g.drawRoundedRectangle(rect, radius, othick);

    float led_diam = rect.getHeight() - 2.0f * bpad - othick;
    juce::Rectangle<float> led_rect(
        rect.getRight() - othick * 0.5f - bpad - led_diam,
        rect.getY() + bpad + othick * 0.5f, led_diam, led_diam);
    juce::Rectangle<float> text_rect(rect.getX() + othick * 0.5f + bpad,
                                     rect.getY() + othick * 0.5f + bpad * 0.25f,
                                     rect.getWidth(),
                                     rect.getHeight() - othick - 0.5f * bpad);
    text_rect.setRight(led_rect.getX() - bpad);

    // g.setColour(juce::Colours::red);
    // g.fillRect(text_rect);

    juce::String txt((on) ? "ON" : "OFF");
    g.setColour(tc);
    auto font = getLabelFont(boldTypeface);
    font.setHeight((text_rect.getHeight() + bpad) * osFontScale);
    g.setFont(font);
    g.drawText(txt, text_rect, juce::Justification::centred);

    g.setColour(lc);
    g.fillEllipse(led_rect);
    if (led_rect.getWidth() >= 1.0f && led_rect.getHeight() >= 1.0f)
        g.setColour(oc);
    g.drawEllipse(led_rect, othick * 0.5f);
}
void CustomLookAndFeel::drawPlotComponent(
    juce::Graphics& g, float /* x */, float /* y */, float width, float height,
    const std::vector<float>& x_values, const std::vector<float>& y_values,
    float period, const std::vector<float>& x_grid,
    const std::vector<float>& y_grid, const std::vector<juce::String>& x_labels,
    const std::vector<juce::String>& y_labels, bool log_x,
    const juce::String& topRightText, PlotComponent& pc)
{
    // Background
    auto corner_s = resizeSize(fullPlotComponentCornerSize);
    g.setColour(pc.findColour(PlotComponent_backgroundColourId));
    g.fillRoundedRectangle(0.0f, 0.0f, width, height, corner_s);

    auto font = getLabelFont(fullLabelFontSize * topRightTextScale);
    g.setFont(font);
    // Top right text
    if (topRightText.length())
    {
        g.setColour(pc.findColour(PlotComponent_gridLabelsColourId));
        g.drawText(topRightText,
                   juce::Rectangle<float>(0.0f, 0.0f, width, height)
                       .reduced(corner_s, corner_s * 0.5f),
                   juce::Justification::topRight);
    }

    // Grid
    juce::Path gridlines;
    auto num_x_ticks = x_grid.size();
    auto num_y_ticks = y_grid.size();
    if (num_x_ticks < 2)
    {
        // Too few x ticks for defining a grid. Don't plot anything
        return;
    }
    if (num_y_ticks < 2)
    {
        // Too few y ticks for defining a grid. Don't plot anything
        return;
    }
    //   Vertical grid lines
    if (x_grid[0] <= 0)
    {
        // jassert(!log_x);
        // If plot is inconsistent, force linear scale. This happens when
        // paint is called after changing between log and lin, but before
        // changing the x ticks
        log_x = false;
    }
    InputTransformMapper<float> x_mapper(
        x_grid[0], 0.0f, x_grid[num_x_ticks - 1], width,
        (log_x) ? static_cast<float (*)(float)>(logf) : identity<float>);
    for (auto xt : std::vector<float>(x_grid.begin() + 1, x_grid.end() - 1))
    {
        auto xt_mapped = x_mapper.map(xt);
        gridlines.startNewSubPath(xt_mapped, 0.0f);
        gridlines.lineTo(xt_mapped, height);
    }
    //   Horizontal grid lines
    LinearMapper<float> y_mapper(y_grid[0], height, y_grid[num_y_ticks - 1],
                                 0.0f);
    for (auto yt : std::vector<float>(y_grid.begin() + 1, y_grid.end() - 1))
    {
        auto yt_mapped = y_mapper.map(yt);
        gridlines.startNewSubPath(0.0f, yt_mapped);
        gridlines.lineTo(width, yt_mapped);
    }
    //   Stroke all grid lines
    g.setColour(pc.findColour(PlotComponent_gridColourId));
    g.strokePath(gridlines,
                 juce::PathStrokeType(resizeSize(fullPlotGridThickness)));

    // Grid labels
    auto gt_pad = corner_s * 0.5f;
    juce::Rectangle r(gt_pad * 2.0f, gt_pad, width - 4.0f * gt_pad,
                      font.getHeight() - 2.0f * gt_pad);
    g.setColour(pc.findColour(PlotComponent_gridLabelsColourId));
    //   Y tick labels
    // Tick labels count mismatch
    jassert(num_y_ticks >= y_labels.size());
    if (num_y_ticks > y_labels.size())
    {
        jassertfalse;
        num_y_ticks = y_labels.size();
    }
    for (auto i = 0; i < num_y_ticks; ++i)
    {
        r.setY((i) ? ((i == num_y_ticks - 1)
                          ? gt_pad
                          : y_mapper.map(y_grid[i]) - font.getHeight() * 0.5f)
                   : height - gt_pad - font.getHeight());
        g.drawText(y_labels[i], r, juce::Justification::topLeft);
    }
    //   X tick labels
    r.setY(height / 2.0f + gt_pad);
    // Tick labels count mismatch
    jassert(num_x_ticks >= x_labels.size());
    if (num_x_ticks > x_labels.size())
    {
        jassertfalse;
        num_x_ticks = x_labels.size();
    }
    for (auto i = 1; i < num_x_ticks - 1; ++i)
    {
        auto x_pos = x_mapper.map(x_grid[i]);
        // Avoid labels near the vertical edges
        if (!width || (0.5f - abs(x_pos / width - 0.5f)) > 0.05f)
        {
            r.setCentre(x_pos, r.getCentreY());
            g.drawText(x_labels[i], r, juce::Justification::centredTop);
        }
    }

    // Plot lines
    auto n_points = x_values.size();
    // Value counts mismatch
    jassert(n_points >= y_values.size());
    if (n_points > y_values.size())
    {
        jassertfalse;
        n_points = y_values.size();
    }
    if (n_points < 2)
    {
        // Can't plot lines if less than 2 points
        jassertfalse;
    }
    else
    {
        juce::Path plot;
        plot.startNewSubPath(x_mapper.map(x_values[0]),
                             y_mapper.map(y_values[0]));
        auto period_half = period * 0.5f;
        auto is_periodic = period > 0.0f;
        for (auto i = 1; i < n_points; ++i)
        {
            if (is_periodic && abs(y_values[i] - y_values[i - 1]) > period_half)
            {
                float offset
                    = (y_values[i] > y_values[i - 1]) ? -period : period;
                plot.lineTo(x_mapper.map(x_values[i]),
                            y_mapper.map(y_values[i] + offset));
                plot.startNewSubPath(x_mapper.map(x_values[i - 1]),
                                     y_mapper.map(y_values[i - 1] - offset));
            }
            plot.lineTo(x_mapper.map(x_values[i]), y_mapper.map(y_values[i]));
        }
        g.setColour(pc.findColour(PlotComponent_lineColourId));
        g.strokePath(plot,
                     juce::PathStrokeType(resizeSize(fullPlotStrokeThickness)));
    }
}
