#include "PluginEditor.h"
#include "DSP/FilterDesign.h"

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
    parameterPanel = std::make_unique<ParameterPanel>(processor);
    plotsPanel = std::make_unique<PlotsPanel>(processor, applicationProperties);
    addAndMakeVisible(*parameterPanel.get());
    addAndMakeVisible(*plotsPanel.get());
    addAndMakeVisible(designGroup);
    addAndMakeVisible(masterGroup);
    resized();

    // !REMOVE THIS! Test FilterDesign
    FilterParameters params(processor.getSampleRate());
    params.cutoff = 500.0;
    params.order  = 4;
    params.computeZPK();

    auto degree = params.zpk.degree();
    auto k_db   = juce::Decibels::gainToDecibels(params.zpk.gain, -1000.0)
                / (params.zpk.zeros.size() + params.zpk.poles.size());
    for (auto i = 0; i < degree; ++i)
    {
        if (i < params.zpk.zeros.size())
            DBG("  Z(" << abs(params.zpk.zeros[i]) << "; "
                       << std::arg(params.zpk.zeros[i])
                              / juce::MathConstants<double>::pi
                       << ") " << k_db << "dB");
        if (i < params.zpk.poles.size())
            DBG("  P(" << abs(params.zpk.poles[i]) << "; "
                       << std::arg(params.zpk.poles[i])
                              / juce::MathConstants<double>::pi
                       << ") " << k_db << "dB");
    }
    // !REMOVE THIS!
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
