#include "Palette.h"
typedef PolesAndZerosEQColourScheme::Label Label;

// =============================================================================
PolesAndZerosEQColourScheme::PolesAndZerosEQColourScheme()
{
    colours.fill(juce::Colour());
}

// =============================================================================
PolesAndZerosEQColourScheme PolesAndZerosEQColourScheme::getEditorTheme()
{
    PolesAndZerosEQColourScheme cs;
    cs.setColour(Label::BACKGROUND, 0xffecf0f1);
    return cs;
}

// =============================================================================
void PolesAndZerosEQColourScheme::setColour(
    PolesAndZerosEQColourScheme::Label label, juce::Colour colour)
{
    colours[label] = colour;
}
template <typename ColourType>
void PolesAndZerosEQColourScheme::setColour(Label label, ColourType colour)
{
    setColour(label, juce::Colour(colour));
}
juce::Colour
PolesAndZerosEQColourScheme::getColour(PolesAndZerosEQColourScheme::Label label)
{
    return (*this)[label];
}
juce::Colour PolesAndZerosEQColourScheme::operator[](Label label)
{
    return colours[label];
}