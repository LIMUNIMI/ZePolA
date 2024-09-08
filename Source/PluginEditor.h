/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.12

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphs.h"
#include "MyTheme.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
#define NUMBER_OF_ELEMENTS          12
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PluginEditor  : public juce::AudioProcessorEditor,
                      public juce::Button::Listener
{
public:
    //==============================================================================
    PluginEditor (PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts);
    ~PluginEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PolesAndZerosEQAudioProcessor& processor;
    AudioProcessorValueTreeState& valueTreeState;

    std::vector<std::unique_ptr<SliderAttachment>> magnitudesAttachments;
    std::vector<std::unique_ptr<SliderAttachment>> phasesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> typesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> activeAttachments;

    Theme theme;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TextButton> reset_button;
    std::unique_ptr<FrequencyResponse> frequency_response;
    std::unique_ptr<juce::Label> freq_response_label;
    std::unique_ptr<PhaseResponse> phase_response;
    std::unique_ptr<juce::Label> ph_response_label;
    std::unique_ptr<GaussianPlane> gaussian_plane;
    std::unique_ptr<juce::Slider> m1_slider;
    std::unique_ptr<juce::Slider> p1_slider;
    std::unique_ptr<juce::Slider> m2_slider;
    std::unique_ptr<juce::Slider> m3_slider;
    std::unique_ptr<juce::Slider> m4_slider;
    std::unique_ptr<juce::Slider> m5_slider;
    std::unique_ptr<juce::Label> elements_chain_label;
    std::unique_ptr<juce::Label> element1_label;
    std::unique_ptr<juce::Label> magnitudes_label;
    std::unique_ptr<juce::Label> phases_label;
    std::unique_ptr<juce::Label> zero_pole_label;
    std::unique_ptr<juce::Label> element2_label;
    std::unique_ptr<juce::Slider> p2_slider;
    std::unique_ptr<juce::Label> active_label;
    std::unique_ptr<juce::Label> element3_label;
    std::unique_ptr<juce::Label> element4_label;
    std::unique_ptr<juce::Label> element5_label;
    std::unique_ptr<juce::Label> element6_label;
    std::unique_ptr<juce::Label> element7_label;
    std::unique_ptr<juce::Label> element8_label;
    std::unique_ptr<juce::Label> element9_label;
    std::unique_ptr<juce::Label> element10_label;
    std::unique_ptr<juce::Label> element11_label;
    std::unique_ptr<juce::Label> element12_label;
    std::unique_ptr<juce::Slider> m6_slider;
    std::unique_ptr<juce::Slider> m7_slider;
    std::unique_ptr<juce::Slider> m8_slider;
    std::unique_ptr<juce::Slider> m9_slider;
    std::unique_ptr<juce::Slider> m10_slider;
    std::unique_ptr<juce::Slider> m11_slider;
    std::unique_ptr<juce::Slider> m12_slider;
    std::unique_ptr<juce::Slider> p3_slider;
    std::unique_ptr<juce::Slider> p4_slider;
    std::unique_ptr<juce::Slider> p5_slider;
    std::unique_ptr<juce::Slider> p6_slider;
    std::unique_ptr<juce::Slider> p7_slider;
    std::unique_ptr<juce::Slider> p8_slider;
    std::unique_ptr<juce::Slider> p9_slider;
    std::unique_ptr<juce::Slider> p10_slider;
    std::unique_ptr<juce::Slider> p11_slider;
    std::unique_ptr<juce::Slider> p12_slider;
    std::unique_ptr<juce::ToggleButton> e1_type;
    std::unique_ptr<juce::ToggleButton> e2_type;
    std::unique_ptr<juce::ToggleButton> e3_type;
    std::unique_ptr<juce::ToggleButton> e4_type;
    std::unique_ptr<juce::ToggleButton> e5_type;
    std::unique_ptr<juce::ToggleButton> e6_type;
    std::unique_ptr<juce::ToggleButton> e7_type;
    std::unique_ptr<juce::ToggleButton> e8_type;
    std::unique_ptr<juce::ToggleButton> e9_type;
    std::unique_ptr<juce::ToggleButton> e10_type;
    std::unique_ptr<juce::ToggleButton> e11_type;
    std::unique_ptr<juce::ToggleButton> e12_type;
    std::unique_ptr<juce::ToggleButton> e1_active;
    std::unique_ptr<juce::ToggleButton> e2_active;
    std::unique_ptr<juce::ToggleButton> e3_active;
    std::unique_ptr<juce::ToggleButton> e4_active;
    std::unique_ptr<juce::ToggleButton> e5_active;
    std::unique_ptr<juce::ToggleButton> e6_active;
    std::unique_ptr<juce::ToggleButton> e7_active;
    std::unique_ptr<juce::ToggleButton> e8_active;
    std::unique_ptr<juce::ToggleButton> e9_active;
    std::unique_ptr<juce::ToggleButton> e10_active;
    std::unique_ptr<juce::ToggleButton> e11_active;
    std::unique_ptr<juce::ToggleButton> e12_active;
    std::unique_ptr<juce::Label> gaussian_plane_label;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

