#pragma once
#include "../Parameters.h"
#include <JuceHeader.h>

// =============================================================================
/** Single parameter row  */
class ParameterStrip : public juce::GroupComponent
{
public:
    // =========================================================================
    ParameterStrip(VTSAudioProcessor&, int index);

    //==========================================================================
    void paint(juce::Graphics& g) override;
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
/** Look and feel methods for parameter strips  */
class ParameterStripLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void drawParameterStrip(juce::Graphics&, int width, int height,
                                    ParameterStrip&)
        = 0;
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
