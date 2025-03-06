#include "TopMenuPanel.h"
#include "LookAndFeel.h"

// =============================================================================
TopMenuPanel::TopMenuPanel(VTSAudioProcessor&)
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
    addAndMakeVisible(exportButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);

    sep.drawBottom = true;
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
        loadButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
        saveButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
        exportButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
    }
}
