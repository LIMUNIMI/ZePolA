#include "PluginEditor.h"

// =============================================================================
PolesAndZerosEQAudioProcessorEditor::PolesAndZerosEQAudioProcessorEditor(
    PolesAndZerosEQAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p), aspectRatioConstrainer()
{
    // Look and feel
    setResizable(true, true);
    setLookAndFeel(&claf);
    aspectRatioConstrainer.setFixedAspectRatio(claf.getAspectRatio());
    setConstrainer(&aspectRatioConstrainer);

    // Application properties options
    juce::PropertiesFile::Options storage_params;
    storage_params.applicationName     = ProjectInfo::projectName;
    storage_params.commonToAllUsers    = true;
    storage_params.filenameSuffix      = "settings";
    storage_params.osxLibrarySubFolder = "Application Support";
    applicationProperties.setStorageParameters(storage_params);

    // Load size from application properties
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        claf.setResizeRatio(static_cast<float>(
            pf->getDoubleValue("sizeRatio", claf.getResizeRatio())));
    setSize(claf.getResizedWidth(), claf.getResizedHeight());

    // Subcomponents
    parameterPanel
        = std::make_unique<ParameterPanel>(processor, processor.getNElements());
    plotsPanel = std::make_unique<PlotsPanel>(processor, applicationProperties);
    addAndMakeVisible(*parameterPanel.get());
    addAndMakeVisible(*plotsPanel.get());
    addAndMakeVisible(designGroup);
    addAndMakeVisible(masterGroup);
    resized();
}
PolesAndZerosEQAudioProcessorEditor::~PolesAndZerosEQAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
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

    if (parameterPanel) parameterPanel->setBounds(panel_rects[1]);
    if (plotsPanel) plotsPanel->setBounds(panel_rects[2]);
    designGroup.setBounds(panel_rects[3]);
    masterGroup.setBounds(panel_rects[4]);
}
