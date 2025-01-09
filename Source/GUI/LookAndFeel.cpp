#include "LookAndFeel.h"
#include "../Macros.h"

// =============================================================================
CustomLookAndFeel::CustomLookAndFeel()
    : resizeRatio(1.0f)
    , fullWidth(1200)
    , fullHeight(790)
    , fullPanelOuterMargin(15.0f)
    , fullPanelMargin(0.0f)
    , groupComponentThickness(1.5f)
    , groupComponentCornerSize(14.5f)
{
    setColour(juce::ResizableWindow::backgroundColourId,
              juce::Colour(0xffecf0f1));
    setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff383838));
    setColour(CustomLookAndFeel::GroupComponent_backgroundColourId,
              juce::Colour(0x17b1b1b1));
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
template float CustomLookAndFeel::resizeSize<float>(float) const;
template int CustomLookAndFeel::resizeSize<int>(int) const;

int CustomLookAndFeel::getResizedWidth() const { return resizeSize(fullWidth); }
int CustomLookAndFeel::getResizedHeight() const
{
    return resizeSize(fullHeight);
}
int CustomLookAndFeel::getResizedPanelOuterMargin() const
{
    return juce::roundToInt(resizeSize(fullPanelOuterMargin));
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

template std::vector<juce::Rectangle<float>>
CustomLookAndFeel::splitProportionalPanels(const juce::Rectangle<float>& r,
                                           const std::vector<float>& fractions,
                                           bool vertical) const;
template std::vector<juce::Rectangle<int>>
CustomLookAndFeel::splitProportionalPanels(const juce::Rectangle<int>& r,
                                           const std::vector<int>& fractions,
                                           bool vertical) const;

template <typename RectType>
juce::Rectangle<RectType>
CustomLookAndFeel::getPanelInnerRect(const juce::Rectangle<RectType>& r) const
{
    return r.reduced(juce::roundToInt(resizeSize(groupComponentCornerSize)));
}

template juce::Rectangle<int>
CustomLookAndFeel::getPanelInnerRect(const juce::Rectangle<int>&) const;
template juce::Rectangle<float>
CustomLookAndFeel::getPanelInnerRect(const juce::Rectangle<float>&) const;

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
        g.setColour(juce::Colour(0xaaff0000));
        g.drawRect(b, resizeSize(1.0f));
    })
}
