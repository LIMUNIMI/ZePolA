/*
  ==============================================================================

    TopMenuPanel.cpp

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

#include "TopMenuPanel.h"
#include "LookAndFeel.h"
#include "ParameterPanel.h"

// =============================================================================
AutoGainAttachment::ParameterListener::ParameterListener(AutoGainAttachment* a)
    : aga(a)
{
}
void AutoGainAttachment::ParameterListener::parameterChanged(
    const juce::String&, float)
{
    if (aga)
    {
        aga->conditionalTrigger();
    }
}

// =============================================================================
AutoGainAttachment::ButtonListener::ButtonListener(AutoGainAttachment* a)
    : aga(a)
{
}
void AutoGainAttachment::ButtonListener::buttonClicked(juce::Button* b)
{
    if (aga)
    {
        aga->setState(b->getToggleState());
        aga->conditionalTrigger();
    }
}
void AutoGainAttachment::ButtonListener::buttonStateChanged(juce::Button*) {}

// =============================================================================
AutoGainAttachment::AutoGainAttachment(ZePolAudioProcessor& p, juce::Button* b,
                                       int idx)
    : i(idx)
    , processor(p)
    , autoGainListener(this)
    , paramListener(this)
    , button(b)
    , doAutoGain(false)
{
    juce::String i_str(i);
    gain_id = GAIN_ID_PREFIX + i_str;
    processor.addParameterListener(MAGNITUDE_ID_PREFIX + i_str, &paramListener);
    processor.addParameterListener(PHASE_ID_PREFIX + i_str, &paramListener);
    processor.addParameterListener(TYPE_ID_PREFIX + i_str, &paramListener);
    processor.addParameterListener(INVERTED_ID_PREFIX + i_str, &paramListener);
    processor.addParameterListener(SINGLE_ID_PREFIX + i_str, &paramListener);
    // processor.addParameterListener(gain_id, &paramListener);
    if (button) button->addListener(&autoGainListener);
}
AutoGainAttachment::~AutoGainAttachment()
{
    juce::String i_str(i);
    processor.removeParameterListener(MAGNITUDE_ID_PREFIX + i_str,
                                      &paramListener);
    processor.removeParameterListener(PHASE_ID_PREFIX + i_str, &paramListener);
    processor.removeParameterListener(TYPE_ID_PREFIX + i_str, &paramListener);
    processor.removeParameterListener(INVERTED_ID_PREFIX + i_str,
                                      &paramListener);
    processor.removeParameterListener(SINGLE_ID_PREFIX + i_str, &paramListener);
    // processor.removeParameterListener(gain_id, &paramListener);
    if (button) button->removeListener(&autoGainListener);
}
bool AutoGainAttachment::conditionalTrigger()
{
    if (doAutoGain)
    {
        // DBG("AUTO GAIN TRIGGERED: " << i);
        auto f = static_cast<float>(processor.getElementAutoGain(i));
        if (processor.getParameterUnnormValue(gain_id) != f)
        {
            // DBG("  " << processor.getParameterUnnormValue(gain_id) << " -> "
            //          << f);
            processor.setParameterValue(gain_id, f);
        }
    }
    return doAutoGain;
}
void AutoGainAttachment::setState(bool active) { doAutoGain = active; }

// =============================================================================
TopMenuPanel::TopMenuPanel(ZePolAudioProcessor& p,
                           juce::ApplicationProperties& properties)
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
    , autoGainLabel("", "GAIN")
    , autoGainButton(std::make_shared<juce::ToggleButton>())
    , presetLocation(std::make_shared<juce::Value>())
{
    addAndMakeVisible(sep);
    addAndMakeVisible(undoButton);
    addAndMakeVisible(redoButton);
    addAndMakeVisible(resetButton);
    addAndMakeVisible(autoGainLabel);
    addAndMakeVisible(*autoGainButton.get());
    addAndMakeVisible(exportButton);
    addAndMakeVisible(saveButton);
    addAndMakeVisible(loadButton);

    Button_setOnOffLabel(*autoGainButton.get(), "MAN", "AUTO");
    autoGainLabel.setJustificationType(juce::Justification::centred);
    sep.drawBottom = true;
    undoButton.onClick
        = std::bind(&VTSAudioProcessor::undoManagerUndo, &processor);
    redoButton.onClick
        = std::bind(&VTSAudioProcessor::undoManagerRedo, &processor);
    resetButton.onClick  = std::bind(&TopMenuPanel::resetParameters, this);
    exportButton.onClick = std::bind(&TopMenuPanel::exportParameters, this);
    saveButton.onClick   = std::bind(&TopMenuPanel::saveParameters, this);
    loadButton.onClick   = std::bind(&TopMenuPanel::loadParameters, this);

    auto n = processor.getNElements();
    for (auto i = 0; i < n; ++i)
        paramAGAttachments.push_back(std::make_unique<AutoGainAttachment>(
            processor, autoGainButton.get(), i));
    presetLocationAttachment
        = std::make_unique<ApplicationPropertiesValueAttachment>(
            properties, "presetLocation", presetLocation,
            ValueApplicationPropertyListener::ValueType::STRING);
    autoGainAttachment
        = std::make_unique<ApplicationPropertiesButtonAttachment>(
            properties, AUTO_GAIN_PROPERTY_ID, autoGainButton);
    if (presetLocation->toString().isEmpty())
        setPresetLocation(juce::File::getSpecialLocation(
            juce::File::SpecialLocationType::userDocumentsDirectory));

    autoFilterUpdateValue.reset(new juce::Value(juce::var(false)));
    autoFilterUpdateValueAttachment.reset(
        new ApplicationPropertiesValueAttachment(
            properties, AUTO_FILTER_PROPERTY_ID, autoFilterUpdateValue,
            ValueApplicationPropertyListener::ValueType::BOOL));
}

// =============================================================================
void TopMenuPanel::setPresetLocation(const juce::File& f)
{
    presetLocation->setValue(
        ((f.existsAsFile()) ? f.getParentDirectory() : f).getFullPathName());
}
std::shared_ptr<juce::File> TopMenuPanel::getPresetLocation() const
{
    return std::make_shared<juce::File>(presetLocation->toString());
}
void TopMenuPanel::saveParameters()
{
    juce::FileChooser chooser("Select the save location...",
                              *getPresetLocation().get(), "*.xml");
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
            setPresetLocation(file);
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
    juce::FileChooser chooser("Select the preset to load...",
                              *getPresetLocation().get(), "*.xml");
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
            processor.setStateInformation(srcData.getData(),
                                          static_cast<int>(srcData.getSize()));
            setPresetLocation(file);
        }
        else
        {
            DBG("Something wrong happened while reading the file: '"
                << file.getFullPathName() << "'");
            jassertfalse;
        }
    }
}
void TopMenuPanel::exportParameters()
{
    juce::FileChooser chooser("Select the save location...",
                              *getPresetLocation().get(), "*.csv;*.txt");
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
            outputStream.setPosition(0);
            outputStream.truncate();
            outputStream << "index,a0,a1,a2,b0,b1,b2,k";
            int i = 0;
            for (const auto& coeffs : processor.getCoefficients())
            {
                jassert(coeffs.size() == 8);
                if (coeffs[0])
                {
                    outputStream << i;
                    for (auto it = coeffs.begin() + 1; it != coeffs.end(); ++it)
                        outputStream << "," << *it;
                    outputStream << "";
                    ++i;
                }
            }
            setPresetLocation(file);
        }
        else
        {
            DBG("Something wrong happened while opening the file for writing: '"
                << file.getFullPathName() << "'");
            jassertfalse;
        }
    }
}
void TopMenuPanel::resetParameters()
{
    processor.resetParameters();
    if (autoGainButton)
        autoGainButton->setToggleState(
            false, juce::NotificationType::sendNotification);
    if (autoFilterUpdateValue)
        autoFilterUpdateValue->setValue(juce::var(false));
}

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
            jassert(paramaterStripRegions.size() == 8);
            autoGainButton->setBounds(
                paramaterStripRegions[4].withY(0).withHeight(getHeight()));
            autoGainLabel.setBounds(paramaterStripRegions[3]
                                        .withY(inner.getY())
                                        .withHeight(inner.getHeight()));
        }
        claf->resizeToggleButton(*autoGainButton.get());

        loadButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
        saveButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
        exportButton.setBounds(inner.removeFromRight(w));
        inner.removeFromRight(pad);
    }
}
