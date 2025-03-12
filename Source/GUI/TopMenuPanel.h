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
    AutoGainAttachment(PolesAndZerosEQAudioProcessor&, juce::Button*, int idx);
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
    PolesAndZerosEQAudioProcessor& processor;
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
    TopMenuPanel(PolesAndZerosEQAudioProcessor&, juce::ApplicationProperties&);

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
    PolesAndZerosEQAudioProcessor& processor;
    TextAndImageButton undoButton, redoButton, resetButton, exportButton,
        saveButton, loadButton;
    juce::Label autoGainLabel;
    std::shared_ptr<juce::ToggleButton> autoGainButton;
    SeparatorComponent sep;
    std::shared_ptr<juce::Value> presetLocation;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopMenuPanel)
};
