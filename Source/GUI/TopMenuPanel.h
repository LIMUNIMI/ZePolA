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
    void resized() override;

private:
    // =========================================================================
    TextAndImageButton undoButton, redoButton, resetButton, exportButton,
        saveButton, loadButton;
    juce::Label autoGainLabel;
    juce::ToggleButton autoGainButton;
    SeparatorComponent sep;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TopMenuPanel)
};
