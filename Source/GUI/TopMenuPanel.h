#pragma once
#include "../PluginProcessor.h"
#include "ApplicationPropertiesListeners.h"
#include "CustomButtons.h"
#include "InvisibleGroupComponent.h"
#include <JuceHeader.h>

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
