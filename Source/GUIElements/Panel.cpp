#include "Panel.h"
#include "../PluginEditor_.h"

// =============================================================================
Panel::Panel() {}
void Panel::paint(juce::Graphics& g)
{
    juce::Rectangle<float> r = getLocalBounds().toFloat();

    if (PolesAndZerosEQAudioProcessorEditor* ape
        = findParentComponentOfClass<PolesAndZerosEQAudioProcessorEditor>())
    {
        float t                       = ape->getRectThickness();
        juce::Rectangle<float> r_trim = r.reduced(t);

        g.setColour(ape->getColour(GUISettings::ColourLabel::BOX_FILL));
        g.fillRoundedRectangle(r_trim, ape->getRectCornerSize());
        g.setColour(ape->getColour(GUISettings::ColourLabel::BOX_STROKE));
        g.drawRoundedRectangle(r_trim, ape->getRectCornerSize(), t);
    }
}
