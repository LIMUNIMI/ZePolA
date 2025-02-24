#include "CustomButtons.h"

// =============================================================================
LabelledToggleButton::LabelledToggleButton(const std::vector<juce::String>& l,
                                           const std::vector<int>& c,
                                           const std::vector<bool>& p)
    : labels(l), colourIDs(c), ledPositions(p), state(0)
{
    jassert(labels.size());
    jassert(labels.size() == colourIDs.size());
    jassert(labels.size() == ledPositions.size());
}
size_t LabelledToggleButton::getCurrentState() const { return state; }
const juce::String& LabelledToggleButton::getCurrentLabel() const
{
    return labels[state];
}
int LabelledToggleButton::getCurrentColourID() const
{
    return colourIDs[state];
}
bool LabelledToggleButton::getCurrentLedPosition() const
{
    return ledPositions[state];
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
