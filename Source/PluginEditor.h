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
#include "GraphicTheme.h"
#include "GUImiscellaneous.h"
#include "Parameters.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class PluginEditor  : public juce::AudioProcessorEditor,
                      public juce::Button::Listener,
                      public juce::Slider::Listener,
                      public juce::Label::Listener,
                      public juce::ComboBox::Listener
{
public:
    //==============================================================================
    PluginEditor (PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts);
    ~PluginEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void getSpectrum ();
    void updateReferenceFrequencies();

    void updateFrequencyFromSlider(juce::Slider* slider, juce::Label* label, double sampleRate);
    void updateSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate);

    void formatFrequencyInput(int frequency, juce::Label* label, double sampleRate);
    void formatQualityInput(double quality, juce::Label* label);
    void formatRippleInput(double ripple, juce::Label* label);
    void formatAttenuationInput(double attenuation, juce::Label* label);

    void updateGUIButterworth();
    void updateGUIChebyshevI();
    void updateGUIChebyshevII();

    void filterDesignCalculation();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PolesAndZerosEQAudioProcessor& processor;
    AudioProcessorValueTreeState& valueTreeState;

    std::vector<double> magnitudes;
    std::vector<double> phases;
    std::vector<double> referenceFrequencies;

    bool linLog = false;

    int design_type;
    int design_shape;
    int design_filters_to_activate;
    int design_frequency;
    double design_quality;
    double design_ripple;
    double design_attenuation;

    std::vector<std::unique_ptr<SliderAttachment>> magnitudesAttachments;
    std::vector<std::unique_ptr<SliderAttachment>> phasesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> typesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> activeAttachments;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;
    std::unique_ptr<ButtonAttachment> linLogAttachment;

    MagnitudeSliderTheme magnitudeSlidersTheme;
    PhaseSliderTheme phaseSlidersTheme;

    TypeSwitchTheme typeSwitchesTheme;
    ActiveSwitchTheme activeSwitchesTheme;

    ResetButtonTheme resetButtonTheme;
    GainSliderTheme gainSliderTheme;
    BypassSwitchTheme bypassSwitchTheme;

    LinLogSwitchTheme linLogTheme;

    CalculateButtonTheme calculateButtonTheme;

    MultiplyPhasesButtonTheme multiplyButtonTheme;
    DividePhasesButtonTheme divideButtonTheme;
    SwapElementsButtonTheme swapButtonTheme;
    TurnOnAllElementsButtonTheme turnOnAllElementsButtonTheme;
    TurnOffAllElementsButtonTheme turnOffAllElementsButtonTheme;

    ComboBoxTheme comboBoxTheme;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TextButton> reset_button;
    std::unique_ptr<GraphicResponse> frequency_response;
    std::unique_ptr<juce::Label> freq_response_label;
    std::unique_ptr<GraphicResponse> phase_response;
    std::unique_ptr<juce::Label> ph_response_label;
    std::unique_ptr<GaussianPlane> gaussian_plane;
    std::unique_ptr<juce::Slider> m1_slider;
    std::unique_ptr<juce::Slider> p1_slider;
    std::unique_ptr<juce::Label> magnitudes_label;
    std::unique_ptr<juce::Label> phases_label;
    std::unique_ptr<juce::Label> zero_pole_label;
    std::unique_ptr<juce::Label> active_label;
    std::unique_ptr<juce::ToggleButton> e1_type;
    std::unique_ptr<juce::ToggleButton> e2_type;
    std::unique_ptr<juce::ToggleButton> e3_type;
    std::unique_ptr<juce::ToggleButton> e4_type;
    std::unique_ptr<juce::ToggleButton> e5_type;
    std::unique_ptr<juce::ToggleButton> e6_type;
    std::unique_ptr<juce::ToggleButton> e7_type;
    std::unique_ptr<juce::ToggleButton> e8_type;
    std::unique_ptr<juce::ToggleButton> e1_active;
    std::unique_ptr<juce::ToggleButton> e2_active;
    std::unique_ptr<juce::ToggleButton> e3_active;
    std::unique_ptr<juce::ToggleButton> e4_active;
    std::unique_ptr<juce::ToggleButton> e5_active;
    std::unique_ptr<juce::ToggleButton> e6_active;
    std::unique_ptr<juce::ToggleButton> e7_active;
    std::unique_ptr<juce::ToggleButton> e8_active;
    std::unique_ptr<juce::Label> gaussian_plane_label;
    std::unique_ptr<LEDComponent> e1_led;
    std::unique_ptr<LEDComponent> e2_led;
    std::unique_ptr<LEDComponent> e3_led;
    std::unique_ptr<LEDComponent> e4_led;
    std::unique_ptr<LEDComponent> e5_led;
    std::unique_ptr<LEDComponent> e6_led;
    std::unique_ptr<LEDComponent> e7_led;
    std::unique_ptr<LEDComponent> e8_led;
    std::unique_ptr<juce::ToggleButton> bypass;
    std::unique_ptr<juce::Slider> gain_slider;
    std::unique_ptr<juce::ToggleButton> linLog_switch;
    std::unique_ptr<juce::Slider> m2_slider;
    std::unique_ptr<juce::Slider> m3_slider;
    std::unique_ptr<juce::Slider> m4_slider;
    std::unique_ptr<juce::Slider> m5_slider;
    std::unique_ptr<juce::Slider> m6_slider;
    std::unique_ptr<juce::Slider> m7_slider;
    std::unique_ptr<juce::Slider> m8_slider;
    std::unique_ptr<juce::Slider> p2_slider;
    std::unique_ptr<juce::Slider> p3_slider;
    std::unique_ptr<juce::Slider> p4_slider;
    std::unique_ptr<juce::Slider> p5_slider;
    std::unique_ptr<juce::Slider> p6_slider;
    std::unique_ptr<juce::Slider> p7_slider;
    std::unique_ptr<juce::Slider> p8_slider;
    std::unique_ptr<juce::Label> p1_freq;
    std::unique_ptr<juce::Label> p2_freq;
    std::unique_ptr<juce::Label> p3_freq;
    std::unique_ptr<juce::Label> p4_freq;
    std::unique_ptr<juce::Label> p5_freq;
    std::unique_ptr<juce::Label> p6_freq;
    std::unique_ptr<juce::Label> p7_freq;
    std::unique_ptr<juce::Label> p8_freq;
    std::unique_ptr<juce::ComboBox> type_box;
    std::unique_ptr<juce::ComboBox> shape_box;
    std::unique_ptr<juce::Label> frequency_label;
    std::unique_ptr<juce::Label> quality_label;
    std::unique_ptr<juce::TextButton> calculate_button;
    std::unique_ptr<juce::TextButton> multiply_phases_button;
    std::unique_ptr<juce::TextButton> divide_phases_button;
    std::unique_ptr<juce::TextButton> swap_button;
    std::unique_ptr<juce::TextButton> turn_on_button;
    std::unique_ptr<juce::TextButton> turn_off_button;
    std::unique_ptr<juce::ComboBox> order_box;
    std::unique_ptr<juce::Label> design_frequency_label;
    std::unique_ptr<juce::Label> text_label;
    std::unique_ptr<juce::Label> ripple_label;
    std::unique_ptr<juce::Label> attenuation_label;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

