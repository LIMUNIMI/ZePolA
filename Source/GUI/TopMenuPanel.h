#pragma once
#include "../Parameters.h"
#include "CustomButtons.h"
#include "InvisibleGroupComponent.h"
#include <JuceHeader.h>

// =============================================================================
/** Top menu panel  */
class TopMenuPanel : public InvisibleGroupComponent
{
public:
    // =========================================================================
    TopMenuPanel(VTSAudioProcessor&);

    // =========================================================================
    /** Button callback for saving parameters */
    void saveParameters();
    /** Button callback for saving parameters */
    void loadParameters();
    /** Button callback for saving parameters */
    void exportParameters();

    // =========================================================================
    void resized() override;

private:
    // =========================================================================
    void updatePresetLocation(const juce::File&);

    // =========================================================================
    VTSAudioProcessor& processor;
    TextAndImageButton undoButton, redoButton, resetButton, exportButton,
        saveButton, loadButton;
    juce::Label autoGainLabel;
    juce::ToggleButton autoGainButton;
    SeparatorComponent sep;
    juce::File presetLocation;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopMenuPanel)
};
