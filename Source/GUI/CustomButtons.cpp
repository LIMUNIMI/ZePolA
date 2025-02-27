#include "CustomButtons.h"

// =============================================================================
LabelledToggleButton::LabelledToggleButton(const juce::String& l0,
                                           const juce::String& l1, int c0,
                                           int c1, bool p0, bool p1)
    : label0(l0)
    , label1(l1)
    , colourID0(c0)
    , colourID1(c1)
    , ledPosition0(p0)
    , ledPosition1(p1)
{
}
const juce::String& LabelledToggleButton::getCurrentLabel() const
{
    return (getToggleState()) ? label1 : label0;
}
int LabelledToggleButton::getCurrentColourID() const
{
    return (getToggleState()) ? colourID1 : colourID0;
}
bool LabelledToggleButton::getCurrentLedPosition() const
{
    return (getToggleState()) ? ledPosition1 : ledPosition0;
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
