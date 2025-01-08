#include "LookAndFeel.h"

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

ValueType CustomLookAndFeel::resizeSize(ValueType f)
{
    return static_cast<ValueType>(f * resizeRatio);
}
template float CustomLookAndFeel::resizeSize<float>(float);
template int CustomLookAndFeel::resizeSize<int>(int);

int CustomLookAndFeel::getResizedWidth() { return resizeSize(fullWidth); }
int CustomLookAndFeel::getResizedHeight() { return resizeSize(fullHeight); }
int CustomLookAndFeel::getResizedPanelOuterMargin()
{
    return juce::roundToInt(resizeSize(fullPanelOuterMargin));
}
float CustomLookAndFeel::getResizeRatio() { return resizeRatio; }
float CustomLookAndFeel::getAspectRatio()
{
    return static_cast<float>(fullWidth) / fullHeight;
}

// =============================================================================
template <typename RectType>
std::vector<juce::Rectangle<RectType>>
CustomLookAndFeel::splitProportional(const juce::Rectangle<RectType>& r,
                                     const std::vector<RectType>& fractions,
                                     bool vertical, float fullMargin)
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
    bool vertical)
{
    return splitProportional(r, fractions, vertical, fullPanelMargin);
}

template std::vector<juce::Rectangle<float>>
CustomLookAndFeel::splitProportionalPanels(const juce::Rectangle<float>& r,
                                           const std::vector<float>& fractions,
                                           bool vertical);
template std::vector<juce::Rectangle<int>>
CustomLookAndFeel::splitProportionalPanels(const juce::Rectangle<int>& r,
                                           const std::vector<int>& fractions,
                                           bool vertical);

// =============================================================================
void CustomLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int w,
                                                  int h, const juce::String&,
                                                  const juce::Justification&,
                                                  juce::GroupComponent& gp)
{
    juce::Rectangle<float> b(0.0f, 0.0f, static_cast<float>(w),
                             static_cast<float>(h));
    b = b.reduced(groupComponentThickness * resizeRatio);
    g.setColour(
        gp.findColour(CustomLookAndFeel::GroupComponent_backgroundColourId));
    g.fillRoundedRectangle(b, groupComponentCornerSize * resizeRatio);
    g.setColour(gp.findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(b, groupComponentCornerSize * resizeRatio,
                           groupComponentThickness * resizeRatio);
}
