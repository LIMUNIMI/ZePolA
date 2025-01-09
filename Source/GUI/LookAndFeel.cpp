#include "LookAndFeel.h"
#include "../Macros.h"
#include "ParameterPanel.h"

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
    , stripColumnProportions({120, 120, 60, 50, 50, 50})
    , panelRowProportions({5, 45, 5, 45})
    , panelProportions({510, 480, 180})
    , lastPanelProportions({396, 324})
    , fontName("Gill Sans")
    , fullLabelFontSize(13.0f)
    , fullSliderHeight(2.0f)
    , fullSliderThumbRadius(6.0f)
    , inactiveBrightness(0.8f)
{
    // Panels
    setColour(juce::ResizableWindow::backgroundColourId,
              juce::Colour(0xffecf0f1));
    setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff383838));
    setColour(CustomLookAndFeel::GroupComponent_backgroundColourId,
              juce::Colour(0x17b1b1b1));
    // Slider
    setColour(juce::Slider::trackColourId, juce::Colour(0xff797d7f));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xff383838));
    setColour(juce::Slider::backgroundColourId, juce::Colour(0x88ffffff));
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
        auto take = (vertical) ? &(juce::Rectangle<float>::removeFromTop)
                               : &(juce::Rectangle<float>::removeFromLeft);
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
            rects.push_back(((t.*take)(f * fractionsK)).toType<RectType>());
            (t.*take)(resizedMargin);
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

// =============================================================================
void CustomLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int width,
                                                  int height,
                                                  const juce::String& text,
                                                  const juce::Justification&,
                                                  juce::GroupComponent& gp)
{
    juce::Rectangle<float> b(0.0f, 0.0f, static_cast<float>(width),
                             static_cast<float>(height));
    b = b.reduced(groupComponentThickness * resizeRatio);
    g.setColour(
        gp.findColour(CustomLookAndFeel::GroupComponent_backgroundColourId));
    g.fillRoundedRectangle(b, resizeSize(groupComponentCornerSize));
    g.setColour(gp.findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(b, resizeSize(groupComponentCornerSize),
                           resizeSize(groupComponentThickness));
}
void CustomLookAndFeel::dontDrawGroupComponent(
    juce::Graphics& g, int ONLY_ON_DEBUG(width), int ONLY_ON_DEBUG(height),
    const juce::String&, const juce::Justification&, juce::GroupComponent&)
{
    ONLY_ON_DEBUG({
        juce::Rectangle<float> b(0.0f, 0.0f, static_cast<float>(width),
                                 static_cast<float>(height));
        g.setColour(juce::Colour(0x88ff0000));
        g.drawRect(b, resizeSize(1.0f));
    });
}
juce::Font CustomLookAndFeel::getLabelFont(juce::Label&)
{
    return juce::Font(fontName, resizeSize(fullLabelFontSize),
                      juce::Font::plain);
}
void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y,
                                         int width, int height, float sliderPos,
                                         float minSliderPos, float maxSliderPos,
                                         const juce::Slider::SliderStyle,
                                         juce::Slider& slider)
{
    juce::Colour trackColour = slider.findColour(juce::Slider::trackColourId);
    juce::Colour thumbColour = slider.findColour(juce::Slider::thumbColourId);
    juce::Colour backgroundColour
        = slider.findColour(juce::Slider::backgroundColourId);

    auto ps = dynamic_cast<ParameterStrip*>(slider.getParentComponent());
    if (ps && ps->isActive())
    {
        trackColour      = trackColour.brighter(inactiveBrightness);
        thumbColour      = thumbColour.brighter(inactiveBrightness);
        backgroundColour = backgroundColour.brighter(inactiveBrightness);
    }

    auto sh = resizeSize(fullSliderHeight);
    g.setColour(trackColour);
    g.fillRoundedRectangle(static_cast<float>(x), y + (height - sh) / 2.0f,
                           static_cast<float>(width), sh, sh / 2.0f);

    auto st_rad = resizeSize(fullSliderThumbRadius);
    juce::Rectangle<float> st_rect(sliderPos - st_rad,
                                   y + height / 2.0f - st_rad, 2.0f * st_rad,
                                   2.0f * st_rad);

    g.setColour(thumbColour);
    g.fillEllipse(st_rect);
    g.setColour(backgroundColour);
    g.drawEllipse(st_rect, resizeSize(1.0f));
}
