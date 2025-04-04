#include "PluginEditor.h"

// =============================================================================
ZePolAudioProcessorEditor::ZePolAudioProcessorEditor(
    ZePolAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
    , processor(p)
    , aspectRatioConstrainer()
    , tooltipWindow(this)
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
    topMenuPanel
        = std::make_unique<TopMenuPanel>(processor, applicationProperties);
    parameterPanel = std::make_unique<ParameterPanel>(processor);
    plotsPanel = std::make_unique<PlotsPanel>(processor, applicationProperties);
    designerPanel
        = std::make_unique<DesignerPanel>(processor, applicationProperties);
    masterPanel = std::make_unique<MasterPanel>(processor);

    addAndMakeVisible(*topMenuPanel.get());
    addAndMakeVisible(*parameterPanel.get());
    addAndMakeVisible(*plotsPanel.get());
    addAndMakeVisible(*designerPanel.get());
    addAndMakeVisible(*masterPanel.get());
    resized();
}
ZePolAudioProcessorEditor::~ZePolAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

// =============================================================================
void ZePolAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}
void ZePolAudioProcessorEditor::resized()
{
    claf.setResizeRatio(getWidth(), getHeight());
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue("sizeRatio", claf.getResizeRatio());

    auto panel_rects = claf.divideInPanels(getLocalBounds());
    jassert(panel_rects.size() == 5);

    if (topMenuPanel) topMenuPanel->setBounds(panel_rects[0]);
    if (parameterPanel) parameterPanel->setBounds(panel_rects[1]);
    if (plotsPanel) plotsPanel->setBounds(panel_rects[2]);
    if (designerPanel) designerPanel->setBounds(panel_rects[3]);
    if (masterPanel) masterPanel->setBounds(panel_rects[4]);
}
