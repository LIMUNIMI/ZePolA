#include "TopMenuPanel.h"
#include "LookAndFeel.h"
#include "ParameterPanel.h"

// =============================================================================
TopMenuPanel::TopMenuPanel(VTSAudioProcessor& processor)
    : undoButton("UNDO",
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
}
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
