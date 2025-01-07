#include "PluginEditor_.h"

// =============================================================================
PolesAndZerosEQAudioProcessorEditor::PolesAndZerosEQAudioProcessorEditor(
    PolesAndZerosEQAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processor(p)
    , aspectRatioConstrainer()
    , settings(GUISettings::getEditorTheme())
    , sizeRatio(1.0f)
{
    setLookAndFeel(&claf);
    sizeSetup();
    addAndMakeVisible(slidersGroup);
    addAndMakeVisible(plotsGroup);
    addAndMakeVisible(designGroup);
    addAndMakeVisible(masterGroup);
}
void PolesAndZerosEQAudioProcessorEditor::sizeSetup()
{
    aspectRatioConstrainer.setFixedAspectRatio(
        settings.getValue(GUISettings::SettingLabel::FULL_WIDTH)
        / settings.getValue(GUISettings::SettingLabel::FULL_HEIGHT));
    setConstrainer(&aspectRatioConstrainer);

    juce::PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.commonToAllUsers    = true;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";

    applicationProperties.setStorageParameters(options);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        sizeRatio = pf->getDoubleValue("sizeRatio", sizeRatio);
    claf.setResizeRatio(sizeRatio);

    setResizable(true, true);
    setSize(static_cast<int>(
                settings.getValue(GUISettings::SettingLabel::FULL_WIDTH)
                * sizeRatio),
            static_cast<int>(
                settings.getValue(GUISettings::SettingLabel::FULL_HEIGHT)
                * sizeRatio));
}

// =============================================================================
void PolesAndZerosEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getColour(GUISettings::ColourLabel::BACKGROUND));
}
void PolesAndZerosEQAudioProcessorEditor::resized()
{
    float wRatio
        = getWidth() / settings.getValue(GUISettings::SettingLabel::FULL_WIDTH);
    float hRatio = getHeight()
                   / settings.getValue(GUISettings::SettingLabel::FULL_HEIGHT);
    sizeRatio = fminf(wRatio, hRatio);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue("sizeRatio", sizeRatio);
    claf.setResizeRatio(sizeRatio);

    juce::Rectangle<float> panels_box = getLocalBounds().toFloat();
    juce::Rectangle<float> header_bar = panels_box.removeFromTop(40.0f);

    panels_box = panels_box.reduced(
        settings.getValue(GUISettings::SettingLabel::PANELS_OUTER_MARGIN));

    juce::Rectangle<float> sliders_panel_box
        = panels_box.removeFromLeft(510.0f * sizeRatio);
    juce::Rectangle<float> plots_panel_box
        = panels_box.removeFromLeft(480.0f * sizeRatio);
    juce::Rectangle<float> design_panel_box
        = panels_box.removeFromTop(396.0f * sizeRatio);

    slidersGroup.setBounds(sliders_panel_box.toNearestIntEdges());
    plotsGroup.setBounds(plots_panel_box.toNearestIntEdges());
    designGroup.setBounds(design_panel_box.toNearestIntEdges());
    masterGroup.setBounds(panels_box.toNearestIntEdges());
}

// =============================================================================
juce::Colour
PolesAndZerosEQAudioProcessorEditor::getColour(GUISettings::ColourLabel label)
{
    return settings.getColour(label);
}
float PolesAndZerosEQAudioProcessorEditor::getValue(
    GUISettings::SettingLabel label)
{
    return settings.getValue(label);
}
float PolesAndZerosEQAudioProcessorEditor::getRectCornerSize()
{
    return settings.getValue(GUISettings::SettingLabel::FULL_RECT_CORNER_SIZE)
           * sizeRatio;
}
float PolesAndZerosEQAudioProcessorEditor::getRectThickness()
{
    return settings.getValue(GUISettings::SettingLabel::FULL_RECT_THICKNESS)
           * sizeRatio;
}
