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

    auto panels_box = getLocalBounds();
    auto header_bar = panels_box.removeFromTop(40);
    panels_box      = panels_box.reduced(claf.getResizedPanelOuterMargin());

    auto v_panels = claf.splitProportionalPanels(panels_box, {510, 480, 180});
    jassert(v_panels.size() == 3);
    auto h_panels_2
        = claf.splitProportionalPanels(v_panels[2], {396, 324}, true);
    jassert(h_panels_2.size() == 2);

    parameterPanel.setBounds(v_panels[0]);
    plotsGroup.setBounds(v_panels[1]);
    designGroup.setBounds(h_panels_2[0]);
    masterGroup.setBounds(h_panels_2[1]);
}
