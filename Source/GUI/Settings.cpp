#include "Settings.h"

// =============================================================================
GUISettings::GUISettings()
{
    colours.fill(juce::Colour());
    settings.fill(0.0f);
}

// =============================================================================
GUISettings GUISettings::getEditorTheme()
{
    GUISettings gs;
    gs.setColour(ColourLabel::BACKGROUND, 0xffecf0f1);
    gs.setColour(ColourLabel::BOX_FILL, 0x17b1b1b1);
    gs.setColour(ColourLabel::BOX_STROKE, 0xff383838);

    gs.setValue(SettingLabel::FULL_WIDTH, 1200.0f);
    gs.setValue(SettingLabel::FULL_HEIGHT, 790.0f);
    gs.setValue(SettingLabel::FULL_RECT_CORNER_SIZE, 14.5f);
    gs.setValue(SettingLabel::FULL_RECT_THICKNESS, 1.5f);
    gs.setValue(SettingLabel::PANELS_OUTER_MARGIN, 15.0f);

    return gs;
}

// =============================================================================
void GUISettings::setColour(GUISettings::ColourLabel label, juce::Colour colour)
{
    colours[label] = colour;
}
template <typename ColourType>
void GUISettings::setColour(GUISettings::ColourLabel label, ColourType colour)
{
    setColour(label, juce::Colour(colour));
}
juce::Colour GUISettings::getColour(GUISettings::ColourLabel label)
{
    return colours[label];
}

// =============================================================================
void GUISettings::setValue(GUISettings::SettingLabel label, float v)
{
    settings[label] = v;
}
float GUISettings::getValue(GUISettings::SettingLabel label)
{
    return settings[label];
}
