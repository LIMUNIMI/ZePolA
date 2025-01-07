#include "PluginEditor_.h"

// =============================================================================
PolesAndZerosEQAudioProcessorEditor::PolesAndZerosEQAudioProcessorEditor(
    PolesAndZerosEQAudioProcessor& p)
    : juce::AudioProcessorEditor(&p), processor(p), aspectRatioConstrainer()
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
        claf.setResizeRatio(
            pf->getDoubleValue("sizeRatio", claf.getResizeRatio()));

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

    juce::Rectangle<float> panels_box = getLocalBounds().toFloat();
    juce::Rectangle<float> header_bar = panels_box.removeFromTop(40.0f);

    panels_box = panels_box.reduced(claf.getResizedPanelOuterMargin());

    juce::Rectangle<float> sliders_panel_box
        = panels_box.removeFromLeft(claf.resizeSize(510.0f));
    juce::Rectangle<float> plots_panel_box
        = panels_box.removeFromLeft(claf.resizeSize(480.0f));
    juce::Rectangle<float> design_panel_box
        = panels_box.removeFromTop(claf.resizeSize(396.0f));

    slidersGroup.setBounds(sliders_panel_box.toNearestIntEdges());
    plotsGroup.setBounds(plots_panel_box.toNearestIntEdges());
    designGroup.setBounds(design_panel_box.toNearestIntEdges());
    masterGroup.setBounds(panels_box.toNearestIntEdges());
}
