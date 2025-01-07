#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Colour palette for poles and zeros GUI */
class PolesAndZerosEQColourScheme
{
public:
    // =========================================================================
    /** Colour labels for colour scheme */
    enum Label
    {
        // Background color label
        BACKGROUND = 0,
        BOX_FILL,
        BOX_STROKE,

        // This should not be used as a label, but only to get how many
        // labels are there
        N_LABELS
    };

    // =========================================================================
    /** Build an all-default colour scheme */
    PolesAndZerosEQColourScheme();

    // =========================================================================
    /** Build the theme for the editor */
    static PolesAndZerosEQColourScheme getEditorTheme();

    // =========================================================================
    /** Set colour for the specified label */
    void setColour(Label, juce::Colour);
    /** Set colour for the specified label */
    template <typename ColourType>
    void setColour(Label, ColourType);
    /** Get colour for the specified label */
    juce::Colour getColour(Label);
    /** Get colour for the specified label */
    juce::Colour operator[](Label);

private:
    // =========================================================================
    std::array<juce::Colour, Label::N_LABELS> colours;

    // =========================================================================
    JUCE_LEAK_DETECTOR(PolesAndZerosEQColourScheme);
};
