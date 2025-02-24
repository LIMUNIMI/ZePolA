#include "CustomButtons.h"

// =============================================================================
LabelledToggleButton::LabelledToggleButton(const std::vector<juce::String>& l)
    : labels(l), state(0)
{
    jassert(labels.size());
}
const juce::String& LabelledToggleButton::getCurrentLabel()
{
    return labels[state];
}
void LabelledToggleButton::clicked()
{
    state = (state + 1) % labels.size();
    ToggleButton::clicked();
}
void LabelledToggleButton::paintButton(juce::Graphics& g,
                                       bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown)
{
    if (auto laf = dynamic_cast<LabelledToggleButtonLookAndFeelMethods*>(
            &getLookAndFeel()))
        laf->drawLabelledToggleButton(g, *this, shouldDrawButtonAsHighlighted,
                                      shouldDrawButtonAsDown);
}
