#include "PluginEditor_.h"

// =============================================================================
const int PolesAndZerosEQAudioProcessorEditor::originalWidth  = 1200;
const int PolesAndZerosEQAudioProcessorEditor::originalHeight = 790;
typedef PolesAndZerosEQColourScheme::Label ColourLabels;

// =============================================================================
PolesAndZerosEQAudioProcessorEditor::PolesAndZerosEQAudioProcessorEditor(
    PolesAndZerosEQAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processor(p)
    , aspectRatioConstrainer()
    , palette(PolesAndZerosEQColourScheme::getEditorTheme())
{
    sizeSetup();
}
void PolesAndZerosEQAudioProcessorEditor::sizeSetup()
{
    aspectRatioConstrainer.setFixedAspectRatio(
        static_cast<double>(originalWidth) / originalHeight);
    setConstrainer(&aspectRatioConstrainer);

    juce::PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.commonToAllUsers    = true;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";

    applicationProperties.setStorageParameters(options);

    auto sizeRatio = 1.0;
    if (auto* properties = applicationProperties.getCommonSettings(true))
        sizeRatio = properties->getDoubleValue("sizeRatio", 1.0);

    setResizable(true, true);
    setSize(static_cast<int>(originalWidth * sizeRatio),
            static_cast<int>(originalHeight * sizeRatio));
}

// =============================================================================
void PolesAndZerosEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(palette.getColour(ColourLabels::BACKGROUND));
}
void PolesAndZerosEQAudioProcessorEditor::resized() {}
