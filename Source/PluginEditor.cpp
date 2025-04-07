/*
  ==============================================================================

    PluginEditor.cpp

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

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
