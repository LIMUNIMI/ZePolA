#pragma once
#include "../Parameters.h"
#include <JuceHeader.h>

// =============================================================================
/** Master control panel  */
class MasterPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    MasterPanel(VTSAudioProcessor&);

    // =========================================================================
    void resized() override;

private:
    // =========================================================================
    juce::Label gainLabel, bypassLabel;
    juce::Slider gainSlider;
    juce::ToggleButton bypassButton;

    // =========================================================================
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gainSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        bypassButtonAttachment;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterPanel)
};
