#pragma once
#include "../Parameters.h"
#include "CustomButtons.h"
#include <JuceHeader.h>

// =============================================================================
/** Filter design panel  */
class DesignerPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    DesignerPanel(VTSAudioProcessor&);

    // =========================================================================
    void resized() override;

private:
    // =========================================================================
    VTSAudioProcessor& processor;
    juce::Label panelLabel;
    juce::ComboBox typeCBox, shapeCBox;
    juce::Slider orderSlider, cutoffSlider;
    juce::ToggleButton autoButton;
    juce::TextButton applyButton;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
