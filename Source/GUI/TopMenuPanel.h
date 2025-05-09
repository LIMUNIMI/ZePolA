/*
  ==============================================================================

    TopMenuPanel.h

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

#pragma once
#include "../PluginProcessor.h"
#include "ApplicationPropertiesListeners.h"
#include "CustomButtons.h"
#include "InvisibleGroupComponent.h"
#include <JuceHeader.h>

// =============================================================================
/** Class for handling the listeners involved in the automatic gain adjustment
 * routine */
class AutoGainAttachment
{
public:
    // =========================================================================
    class ParameterListener
        : public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        // =====================================================================
        ParameterListener(AutoGainAttachment*);
        void parameterChanged(const juce::String&, float) override;

    private:
        // =====================================================================
        AutoGainAttachment* aga;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterListener)
    };

    // =========================================================================
    class ButtonListener : public juce::Button::Listener
    {
    public:
        // =====================================================================
        ButtonListener(AutoGainAttachment*);

        // =====================================================================
        void buttonClicked(juce::Button*) override;
        void buttonStateChanged(juce::Button*) override;

    private:
        // =====================================================================
        AutoGainAttachment* aga;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonListener)
    };

    // =========================================================================
    AutoGainAttachment(ZePolAudioProcessor&, juce::Button*, int idx);
    ~AutoGainAttachment();

    // =========================================================================
    /** Trigger gain adjustment conditionally to the activation state */
    bool conditionalTrigger();
    /** Set the activation state */
    void setState(bool active);

private:
    // =========================================================================
    int i;
    juce::String gain_id;
    ZePolAudioProcessor& processor;
    ButtonListener autoGainListener;
    ParameterListener paramListener;
    juce::Button* button;
    bool doAutoGain;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoGainAttachment)
};

// =============================================================================
/** Top menu panel  */
class TopMenuPanel : public InvisibleGroupComponent
{
public:
    // =========================================================================
    TopMenuPanel(ZePolAudioProcessor&, juce::ApplicationProperties&);

    // =========================================================================
    /** Button callback for saving parameters */
    void saveParameters();
    /** Button callback for saving parameters */
    void loadParameters();
    /** Button callback for saving parameters */
    void exportParameters();
    /** Button callback for resetting parameters */
    void resetParameters();

    // =========================================================================
    void resized() override;

    // =========================================================================
    /** Set the location for presets */
    void setPresetLocation(const juce::File&);
    /** Get the location for presets */
    std::shared_ptr<juce::File> getPresetLocation() const;

private:
    // =========================================================================
    std::unique_ptr<ApplicationPropertiesValueAttachment>
        presetLocationAttachment;
    std::unique_ptr<ApplicationPropertiesButtonAttachment> autoGainAttachment;
    std::vector<std::unique_ptr<AutoGainAttachment>> paramAGAttachments;

    // =========================================================================
    ZePolAudioProcessor& processor;
    TextAndImageButton undoButton, redoButton, resetButton, exportButton,
        saveButton, loadButton;
    juce::Label autoGainLabel;
    std::shared_ptr<juce::ToggleButton> autoGainButton;
    SeparatorComponent sep;
    std::shared_ptr<juce::Value> presetLocation;
    std::shared_ptr<juce::Value> autoFilterUpdateValue;
    std::unique_ptr<ApplicationPropertiesValueAttachment>
        autoFilterUpdateValueAttachment;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopMenuPanel)
};
