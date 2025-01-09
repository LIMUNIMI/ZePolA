#pragma once
#include "../Parameters.h"
#include "InvisibleGroupComponent.h"
#include <JuceHeader.h>

// =============================================================================
/** Single parameter row  */
class ParameterStrip : public InvisibleGroupComponent
{
public:
    // =========================================================================
    ParameterStrip(VTSAudioProcessor&, int index);

    //==========================================================================
    void resized() override;

private:
    // =========================================================================
    juce::Slider mSlider, pSlider;
    juce::ToggleButton aButton, tButton;

    // =========================================================================
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mSliderAttachment, pSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        aButtonAttachment, tButtonAttachment;
};

// =============================================================================
/** Parameter control panel  */
class ParameterPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    ParameterPanel(VTSAudioProcessor&, size_t);

    //===================================================================
    void resized() override;

private:
    // =========================================================================
    std::vector<std::unique_ptr<ParameterStrip>> strips;
};
