#include "LookAndFeel.h"

// =============================================================================
CustomLookAndFeel::CustomLookAndFeel()
    : resizeRatio(1.0f)
    , groupComponentThickness(1.5f)
    , groupComponentCornerSize(14.5f)
{
    setColour(juce::GroupComponent::outlineColourId, juce::Colour(0xff383838));
    setColour(CustomLookAndFeel::GroupComponent_backgroundColourId,
              juce::Colour(0x17b1b1b1));
}

// =============================================================================
void CustomLookAndFeel::setResizeRatio(float f) { resizeRatio = f; }

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
