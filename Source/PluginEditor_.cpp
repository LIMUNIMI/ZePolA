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
    sizeSetup();
    addAndMakeVisible(slidersPanel);
    addAndMakeVisible(plotsPanel);
    addAndMakeVisible(designPanel);
    addAndMakeVisible(masterPanel);
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

    slidersPanel.setBounds(
        static_cast<int>(15 * sizeRatio), static_cast<int>(55 * sizeRatio),
        static_cast<int>(510 * sizeRatio), static_cast<int>(720 * sizeRatio));
    plotsPanel.setBounds(
        static_cast<int>(525 * sizeRatio), static_cast<int>(55 * sizeRatio),
        static_cast<int>(480 * sizeRatio), static_cast<int>(720 * sizeRatio));
    designPanel.setBounds(
        static_cast<int>(1005 * sizeRatio), static_cast<int>(55 * sizeRatio),
        static_cast<int>(180 * sizeRatio), static_cast<int>(396 * sizeRatio));
    masterPanel.setBounds(
        static_cast<int>(1005 * sizeRatio), static_cast<int>(451 * sizeRatio),
        static_cast<int>(180 * sizeRatio), static_cast<int>(324 * sizeRatio));

    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue("sizeRatio", sizeRatio);
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
