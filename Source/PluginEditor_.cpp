#include "PluginEditor_.h"

// =============================================================================
PolesAndZerosEQAudioProcessorEditor::PolesAndZerosEQAudioProcessorEditor(
    PolesAndZerosEQAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processor(p)
    , aspectRatioConstrainer()
    , parameterPanel(processor, processor.getNElements())
{
    setLookAndFeel(&claf);
    addAndMakeVisible(parameterPanel);
    addAndMakeVisible(plotsGroup);
    addAndMakeVisible(designGroup);
    addAndMakeVisible(masterGroup);
    sizeSetup();
}
void PolesAndZerosEQAudioProcessorEditor::sizeSetup()
{
    aspectRatioConstrainer.setFixedAspectRatio(claf.getAspectRatio());
    setConstrainer(&aspectRatioConstrainer);

    juce::PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.commonToAllUsers    = true;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";

    applicationProperties.setStorageParameters(options);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        claf.setResizeRatio(static_cast<float>(
            pf->getDoubleValue("sizeRatio", claf.getResizeRatio())));

    setResizable(true, true);
    setSize(claf.getResizedWidth(), claf.getResizedHeight());
}

// =============================================================================
void PolesAndZerosEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}
void PolesAndZerosEQAudioProcessorEditor::resized()
{
    claf.setResizeRatio(getWidth(), getHeight());
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue("sizeRatio", claf.getResizeRatio());

    auto panel_rects = claf.divideInPanels(getLocalBounds());
    jassert(panel_rects.size() == 5);

    parameterPanel.setBounds(panel_rects[1]);
    plotsGroup.setBounds(panel_rects[2]);
    designGroup.setBounds(panel_rects[3]);
    masterGroup.setBounds(panel_rects[4]);
}
