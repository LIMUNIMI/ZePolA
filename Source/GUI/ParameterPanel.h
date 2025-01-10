#pragma once
#include "../Parameters.h"
#include "InvisibleGroupComponent.h"
#include <JuceHeader.h>

// =============================================================================
/** Parameter slider for parameter strip */
class ParameterSlider : public juce::Slider
{
public:
    // =========================================================================
    ParameterSlider();

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSlider)
};

// =============================================================================
/** Single parameter row  */
class ParameterStrip : public InvisibleGroupComponent
{
public:
    // =========================================================================
    class ParentRepaintButtonListener : public juce::Button::Listener
    {
    public:
        // =====================================================================
        ParentRepaintButtonListener();

        // =====================================================================
        virtual void buttonClicked(juce::Button*) override;
        virtual void buttonStateChanged(juce::Button*) override;
    };

    // =========================================================================
    ParameterStrip(VTSAudioProcessor&, int index);

    //==========================================================================
    void resized() override;

    //==========================================================================
    /** Return true if the active button is down */
    bool isActive();
    /** Return true if the parent component is an active ParameterStrip or not a
     * ParameterStrip */
    static bool parentComponentIsActive(const juce::Component&);

private:
    // =========================================================================
    ParameterSlider mSlider, pSlider;
    juce::ToggleButton aButton, tButton;

    // =========================================================================
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mSliderAttachment, pSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        aButtonAttachment, tButtonAttachment;
    ParentRepaintButtonListener aButtonListener;
};

// =============================================================================
/** Parameter control panel  */
class ParameterPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    ParameterPanel(VTSAudioProcessor&, size_t);

    //==========================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    // =========================================================================
    std::vector<std::unique_ptr<ParameterStrip>> strips;
    std::vector<std::unique_ptr<juce::Label>> headerLabels;
};

// =============================================================================
/** Look and Feel methods for parameter control panel  */
class ParameterPanelLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void drawParameterStripSeparators(juce::Graphics&, float x,
                                              std::vector<float> y, float width,
                                              ParameterPanel&)
        = 0;
};
