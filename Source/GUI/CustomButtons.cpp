/*
  ==============================================================================

    CustomButtons.cpp

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

#include "CustomButtons.h"
#include "../Macros.h"

// =============================================================================
/** Separator character for the two texts of a toggle button */
static const char OFF_ON_CHARSEP = 0x17;

juce::String Button_getOnOffLabel(const juce::Button& b, bool state)
{
    const juce::String& txt = b.getButtonText();
    auto i                  = txt.indexOfChar(OFF_ON_CHARSEP);
    if (i >= 0)
    {
        return (state) ? txt.substring(i + 1) : txt.substring(0, i);
    }
    else if (txt.isEmpty())
        return (state) ? "ON" : "OFF";  // Default
    else
        return txt;  // Same label for both
}
juce::String Button_getOnOffLabel(const juce::Button& b)
{
    return Button_getOnOffLabel(b, b.getToggleState());
}
void Button_setOnOffLabel(juce::Button& b, const juce::String& labelOff,
                          const juce::String& labelOn)
{
    b.setButtonText(labelOff + OFF_ON_CHARSEP + labelOn);
}

// =============================================================================
ToggleButtonCheckbox::ToggleButtonCheckbox() {}

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

// =============================================================================
static const juce::String _tandiButtPadding("  ");
TextAndImageButton::TextAndImageButton(const juce::String& name,
                                       juce::Drawable* d,
                                       const juce::Justification& j)
    : juce::Button(name)
    , image(d)
    , label(name, _tandiButtPadding + name + _tandiButtPadding)
{
    if (image) addChildComponent(*image.get());
    addChildComponent(label);

    label.setJustificationType(j);
}
void TextAndImageButton::paintButton(juce::Graphics& g,
                                     bool /* shouldDrawButtonAsHighlighted */,
                                     bool /* shouldDrawButtonAsDown */)
{
    if (image)
        image->drawWithin(g, image->getBounds().toFloat(),
                          juce::RectanglePlacement::centred, 1.0);
    g.setOrigin(label.getBounds().getTopLeft());
    getLookAndFeel().drawLabel(g, label);
}
void TextAndImageButton::resized()
{
    if (image)
    {
        image->setBoundsToFit(getLocalBounds(), label.getJustificationType(),
                              false);
        switch (label.getJustificationType().getOnlyHorizontalFlags())
        {
        case juce::Justification::left:
            label.setBounds(image->getRight(), 0,
                            getWidth() - image->getRight(), getHeight());
            break;
        case juce::Justification::right:
            label.setBounds(0, 0, image->getX(), getHeight());
            break;
        default:
            UNHANDLED_SWITCH_CASE("Unhandled case for justification type");
            label.setBounds(0, 0, getWidth(), getHeight());
            break;
        }
    }
    else
        label.setBounds(getLocalBounds());
}
