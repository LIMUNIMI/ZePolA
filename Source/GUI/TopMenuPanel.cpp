#include "TopMenuPanel.h"
#include "LookAndFeel.h"
#include "ParameterPanel.h"

// =============================================================================
TopMenuPanel::TopMenuPanel(VTSAudioProcessor& p)
    : processor(p)
    , undoButton("UNDO",
                 juce::Drawable::createFromImageData(
                     BinaryData::anticlockwise_arrow_svg,
                     BinaryData::anticlockwise_arrow_svgSize)
                     .release(),
                 juce::Justification::centredRight)
    , redoButton("REDO",
                 juce::Drawable::createFromImageData(
                     BinaryData::clockwise_arrow_svg,
                     BinaryData::clockwise_arrow_svgSize)
                     .release(),
                 juce::Justification::centredRight)
    , resetButton("RESET", nullptr, juce::Justification::centredRight)
    , autoGainLabel("", "GAIN")
    , exportButton("EXPORT", nullptr, juce::Justification::centredLeft)
    , saveButton("SAVE",
                 juce::Drawable::createFromImageData(
                     BinaryData::save_icon_svg, BinaryData::save_icon_svgSize)
                     .release(),
                 juce::Justification::centredLeft)
    , loadButton("LOAD",
                 juce::Drawable::createFromImageData(
                     BinaryData::load_icon_svg, BinaryData::load_icon_svgSize)
                     .release(),
                 juce::Justification::centredLeft)
    , presetLocation(juce::File::getSpecialLocation(
          juce::File::SpecialLocationType::userDocumentsDirectory))
{
    addAndMakeVisible(sep);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(autoGainLabel);
    addAndMakeVisible(autoGainButton);
    addAndMakeVisible(exportButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);

    Button_setOnOffLabel(autoGainButton, "MAN", "AUTO");
    autoGainLabel.setJustificationType(juce::Justification::centred);
    sep.drawBottom = true;
    undoButton.onClick
        = std::bind(&VTSAudioProcessor::undoManagerUndo, &processor);
    redoButton.onClick
        = std::bind(&VTSAudioProcessor::undoManagerRedo, &processor);
    resetButton.onClick
        = std::bind(&VTSAudioProcessor::resetParameters, &processor);
    exportButton.onClick = std::bind(&TopMenuPanel::exportParameters, this);
    saveButton.onClick   = std::bind(&TopMenuPanel::saveParameters, this);
    loadButton.onClick   = std::bind(&TopMenuPanel::loadParameters, this);
}

// =============================================================================
void TopMenuPanel::updatePresetLocation(const juce::File& f)
{
    DBG("New preset location: '" << f.getFullPathName() << "'");
    presetLocation = (f.existsAsFile()) ? f.getParentDirectory() : f;
    DBG("  -> '" << presetLocation.getFullPathName() << "'");
}
void TopMenuPanel::saveParameters()
{
    juce::FileChooser chooser("Select the save location...", presetLocation,
                              "*.xml");
    if (chooser.browseForFileToSave(true))
    {
        auto file = chooser.getResult();
        if (file.existsAsFile())
            file.deleteFile();
        else if (file.exists())
        {
            DBG("A directory was selected: '" << file.getFullPathName() << "'");
            jassertfalse;
            return;
        }
        juce::FileOutputStream outputStream(file);
        if (outputStream.openedOk())
        {
            juce::MemoryBlock destData;
            processor.getStateInformation(destData);
            outputStream.write(destData.getData(), destData.getSize());
            updatePresetLocation(file);
        }
        else
        {
            DBG("Something wrong happened while opening the file for writing: '"
                << file.getFullPathName() << "'");
            jassertfalse;
        }
    }
}
void TopMenuPanel::loadParameters()
{
    juce::FileChooser chooser("Select the preset to load...", presetLocation,
                              "*.xml");
    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
        if (!file.existsAsFile())
        {
            DBG("File not found: '" << file.getFullPathName() << "'");
            jassertfalse;
            return;
        }
        juce::MemoryBlock srcData;
        if (file.loadFileAsData(srcData))
        {
            processor.setStateInformation(srcData.getData(), srcData.getSize());
        }
        else
        {
            DBG("Something wrong happened while reading the file: '"
                << file.getFullPathName() << "'");
            jassertfalse;
        }
    }
}
void TopMenuPanel::exportParameters() { DBG("EXPORT PARAMETERS"); }

// =============================================================================
void TopMenuPanel::resized()
{
    sep.setBounds(getLocalBounds());
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        int pad = (getHeight() - claf->resizeSize(12 /* fullMasterLabelSize */))
                  / 2;
        int w      = claf->resizeSize(56);
        auto inner = getLocalBounds().reduced(pad, pad);
        undoButton.setBounds(inner.removeFromLeft(w));
        inner.removeFromLeft(pad);
        redoButton.setBounds(inner.removeFromLeft(w));
        inner.removeFromLeft(pad);
        resetButton.setBounds(inner.removeFromLeft(w));
        inner.removeFromLeft(pad);

        {
            // Match button to parameter strip
            auto panel_rects
                = claf->divideInPanels(getParentComponent()->getLocalBounds());
            jassert(panel_rects.size() == 5);
            auto parameterPanelRect    = panel_rects[1];
            auto parameterPanelRegions = claf->splitProportionalPanel(
                claf->getPanelInnerRect(parameterPanelRect));
            jassert(parameterPanelRegions.size() == 5);
            auto paramaterStripRegions
                = claf->splitProportionalStrip(parameterPanelRegions[0]);
            jassert(paramaterStripRegions.size() == 6);
            autoGainButton.setBounds(
                paramaterStripRegions[4].withY(0).withHeight(getHeight()));
            autoGainLabel.setBounds(paramaterStripRegions[3]
                                        .withY(inner.getY())
                                        .withHeight(inner.getHeight()));
        }
        claf->resizeToggleButton(autoGainButton);

        loadButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
        saveButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
        exportButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
    }
}
