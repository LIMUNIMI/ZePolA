#include "LookAndFeel.h"

// =============================================================================
CustomLookAndFeel::CustomLookAndFeel()
    : resizeRatio(1.0f)
    , fullWidth(1200)
    , fullHeight(790)
    , fullPanelOuterMargin(15)
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
    return resizeSize(fullPanelOuterMargin);
}
float CustomLookAndFeel::getResizeRatio() { return resizeRatio; }
float CustomLookAndFeel::getAspectRatio()
{
    return static_cast<float>(fullWidth) / fullHeight;
}

// =============================================================================
void CustomLookAndFeel::drawGroupComponentOutline(juce::Graphics& g, int w,
                                                  int h,
                                                  const juce::String& text,
                                                  const juce::Justification& j,
                                                  juce::GroupComponent& gp)
{
    juce::Rectangle<float> b(0.0f, 0.0f, w, h);
    b = b.reduced(groupComponentThickness * resizeRatio);
    g.setColour(
        gp.findColour(CustomLookAndFeel::GroupComponent_backgroundColourId));
    g.fillRoundedRectangle(b, groupComponentCornerSize * resizeRatio);
    g.setColour(gp.findColour(juce::GroupComponent::outlineColourId));
    g.drawRoundedRectangle(b, groupComponentCornerSize * resizeRatio,
                           groupComponentThickness * resizeRatio);
}
