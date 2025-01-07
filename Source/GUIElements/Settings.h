#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Settings and colours for poles and zeros GUI */
class GUISettings
{
public:
    // =========================================================================
    /** Colour labels for colour scheme */
    enum ColourLabel
    {
        // Background color label
        BACKGROUND = 0,
        // Rectangle fill colour
        BOX_FILL,
        // Rectangle stroke colour
        BOX_STROKE,

        // This should not be used as a label, but only to get how many
        // labels are there
        N_COLOURS
    };
    /** Settings labels for numeric settings */
    enum SettingLabel
    {
        // Default full width of the GUI
        FULL_WIDTH = 0,
        // Default full height of the GUI
        FULL_HEIGHT,
        // Rectangle corner size at full size
        FULL_RECT_CORNER_SIZE,
        // Rectangle stroke thickness at full size
        FULL_RECT_THICKNESS,

        // This should not be used as a label, but only to get how many
        // labels are there
        N_SETTINGS
    };

    // =========================================================================
    /** All-zero settings */
    GUISettings();

    // =========================================================================
    /** Build the settings for the editor */
    static GUISettings getEditorTheme();

    // =========================================================================
    /** Set colour for the specified label */
    void setColour(ColourLabel, juce::Colour);
    /** Set colour for the specified label */
    template <typename ColourType>
    void setColour(ColourLabel, ColourType);
    /** Get colour for the specified label */
    juce::Colour getColour(ColourLabel);

    // =========================================================================
    /** Set setting value for the specified label */
    void setValue(SettingLabel, float);
    /** Get setting value for the specified label */
    float getValue(SettingLabel);

private:
    // =========================================================================
    std::array<juce::Colour, ColourLabel::N_COLOURS> colours;
    std::array<float, SettingLabel::N_SETTINGS> settings;

    // =========================================================================
    JUCE_LEAK_DETECTOR(GUISettings);
};
