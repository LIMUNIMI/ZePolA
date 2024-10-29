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
#include "Parameters.h"

#define NUMBER_OF_REFERENCE_FREQUENCIES         8
#define FREQUENCY_FLOOR                         10.0

#define DESIGN_FREQUENCY_FLOOR                  1.0

#define SELECTABLE_FILTER_TYPES                 {"BUTTERWORTH", "CHEBYSHEV I", "CHEBYSHEV II", "ELLIPTIC"}

#define SELECTABLE_ORDERS_BUTTERWORTH           {"2", "4", "6", "8"}

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
                      public juce::Slider::Listener,
                      public juce::Button::Listener,
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
    void updateElements();

    void updateFrequencyFromSlider(juce::Slider* slider, juce::Label* label, double sampleRate);
    void updateSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate);

    double updateFrequencyFromDesignSlider(juce::Slider* slider, juce::Label* label, double sampleRate);
    double updateDesignSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate);

    void formatFrequencyInput(int& frequency, juce::Label* label, double sampleRate);
    void formatDesignFrequencyInput(int& frequency, juce::Label* label, double sampleRate);
    void updateDesignFrequencySlider(short int option);
    void setTransitionWidthRange();

    void updateGUIGivenShape();
    void updateGUIButterworth();
    void updateGUIEllipticChebyshevIandII();

    void coefficientsNormalization(double& c0, double& c1, double& c2);
    void fromCoefficientsToMagnitudeAndPhase(double& mg, double& ph, double c1, double c2);
    void filterDesignAndSetup();
    void autoUpdateCheckAndSetup();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;

    // Binary resources:
    static const char* anticlockwise_arrow_png;
    static const int anticlockwise_arrow_pngSize;
    static const char* clockwise_arrow_png;
    static const int clockwise_arrow_pngSize;
    static const char* load_icon_png;
    static const int load_icon_pngSize;
    static const char* save_icon_png;
    static const int save_icon_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    PolesAndZerosEQAudioProcessor& processor;
    AudioProcessorValueTreeState& valueTreeState;

    double magnitudes[GRAPHS_QUALITY];
    double phases[GRAPHS_QUALITY];
    double referenceFrequencies[NUMBER_OF_REFERENCE_FREQUENCIES];

    bool linLog = false;
    bool ampDb = false;
    bool autoUpdate = false;

    int design_type = -1;
    int design_shape;
    int design_filters_to_activate;
    int design_frequency;
    double normalisedTransitionWidth;
    double passbandAmplitudedB = -0.1;
    double stopbandAmplitudedB = -21;

    std::vector<juce::String> selectable_filter_types;
    std::vector<juce::String> selectable_orders_butterworth;

    std::vector<std::unique_ptr<SliderAttachment>> magnitudesAttachments;
    std::vector<std::unique_ptr<SliderAttachment>> phasesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> typesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> activeAttachments;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;
    std::unique_ptr<ButtonAttachment> linLogAttachment;

    MagnitudeSliderTheme magnitudeSlidersTheme;
    PhaseSliderTheme phaseSlidersTheme;
    GainSliderTheme gainSliderTheme;

    TypeSwitchTheme typeSwitchesTheme;
    LinLogSwitchTheme linLogTheme;
    AmpDbSwitchTheme ampDbTheme;

    ActiveSwitchTheme activeSwitchesTheme;
    BypassSwitchTheme bypassSwitchTheme;

    GenericButtonTheme resetButtonTheme;
    GenericButtonTheme calculateButtonTheme;

    GenericButtonTheme autoGainButtonTheme;
    GenericButtonTheme turnAllOnTheme;
    GenericButtonTheme turnAllOffTheme;
    GenericButtonTheme multiplyPhasesTheme;
    GenericButtonTheme dividePhasesTheme;
    GenericButtonTheme swapButtonTheme;

    MenuButtonTheme undoButtonTheme;
    MenuButtonTheme redoButtonTheme;

    MenuButtonTheme savePresetButtonTheme;
    MenuButtonTheme loadPresetButtonTheme;

    ComboBoxTheme comboBoxTheme;

    AutoUpdateSwitchTheme autoUpdateSwitchTheme;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<GaussianPlane> gaussian_plane;
    std::unique_ptr<DraggableElement> e2;
    std::unique_ptr<DraggableElement> e3;
    std::unique_ptr<DraggableElement> e4;
    std::unique_ptr<DraggableElement> e5;
    std::unique_ptr<DraggableElement> e6;
    std::unique_ptr<DraggableElement> e7;
    std::unique_ptr<DraggableElement> e8;
    std::unique_ptr<DraggableElement> e1;
    std::unique_ptr<juce::Label> passbandAmplitude_label;
    std::unique_ptr<CustomSlider> stopbandAmplitude_slider;
    std::unique_ptr<CustomButton> reset_button;
    std::unique_ptr<FrequencyResponse> frequency_response;
    std::unique_ptr<juce::Label> freq_response_label;
    std::unique_ptr<PhaseResponse> phase_response;
    std::unique_ptr<juce::Label> ph_response_label;
    std::unique_ptr<CustomSlider> m1_slider;
    std::unique_ptr<CustomSlider> p1_slider;
    std::unique_ptr<juce::Label> magnitudes_label;
    std::unique_ptr<juce::Label> phases_label;
    std::unique_ptr<juce::Label> zero_pole_label;
    std::unique_ptr<juce::Label> active_label;
    std::unique_ptr<CustomToggleButton> e1_type;
    std::unique_ptr<CustomToggleButton> e2_type;
    std::unique_ptr<CustomToggleButton> e3_type;
    std::unique_ptr<CustomToggleButton> e4_type;
    std::unique_ptr<CustomToggleButton> e5_type;
    std::unique_ptr<CustomToggleButton> e6_type;
    std::unique_ptr<CustomToggleButton> e7_type;
    std::unique_ptr<CustomToggleButton> e8_type;
    std::unique_ptr<CustomToggleButton> e1_active;
    std::unique_ptr<CustomToggleButton> e2_active;
    std::unique_ptr<CustomToggleButton> e3_active;
    std::unique_ptr<CustomToggleButton> e4_active;
    std::unique_ptr<CustomToggleButton> e5_active;
    std::unique_ptr<CustomToggleButton> e6_active;
    std::unique_ptr<CustomToggleButton> e7_active;
    std::unique_ptr<CustomToggleButton> e8_active;
    std::unique_ptr<juce::Label> gaussian_plane_label;
    std::unique_ptr<CustomToggleButton> bypass;
    std::unique_ptr<CustomSlider> gain_slider;
    std::unique_ptr<CustomToggleButton> linLog_switch;
    std::unique_ptr<CustomSlider> m2_slider;
    std::unique_ptr<CustomSlider> m3_slider;
    std::unique_ptr<CustomSlider> m4_slider;
    std::unique_ptr<CustomSlider> m5_slider;
    std::unique_ptr<CustomSlider> m6_slider;
    std::unique_ptr<CustomSlider> m7_slider;
    std::unique_ptr<CustomSlider> m8_slider;
    std::unique_ptr<CustomSlider> p2_slider;
    std::unique_ptr<CustomSlider> p3_slider;
    std::unique_ptr<CustomSlider> p4_slider;
    std::unique_ptr<CustomSlider> p5_slider;
    std::unique_ptr<CustomSlider> p6_slider;
    std::unique_ptr<CustomSlider> p7_slider;
    std::unique_ptr<CustomSlider> p8_slider;
    std::unique_ptr<juce::Label> p1_freq;
    std::unique_ptr<juce::Label> p2_freq;
    std::unique_ptr<juce::Label> p3_freq;
    std::unique_ptr<juce::Label> p4_freq;
    std::unique_ptr<juce::Label> p5_freq;
    std::unique_ptr<juce::Label> p6_freq;
    std::unique_ptr<juce::Label> p7_freq;
    std::unique_ptr<juce::Label> p8_freq;
    std::unique_ptr<CustomComboBox> type_box;
    std::unique_ptr<CustomComboBox> shape_box;
    std::unique_ptr<CustomButton> calculate_button;
    std::unique_ptr<CustomButton> multiply_phases_button;
    std::unique_ptr<CustomButton> divide_phases_button;
    std::unique_ptr<CustomButton> swap_button;
    std::unique_ptr<CustomButton> turn_on_button;
    std::unique_ptr<CustomButton> turn_off_button;
    std::unique_ptr<CustomComboBox> order_box;
    std::unique_ptr<juce::Label> design_frequency_label;
    std::unique_ptr<CustomToggleButton> ampDb_switch;
    std::unique_ptr<CustomSlider> frequency_design_slider;
    std::unique_ptr<juce::Label> frequency_label;
    std::unique_ptr<juce::Label> transition_width_label;
    std::unique_ptr<CustomSlider> transition_width_slider;
    std::unique_ptr<CustomSlider> passbandAmplitude_slider;
    std::unique_ptr<juce::Label> stopbandAmplitude_label;
    std::unique_ptr<CustomToggleButton> autoUpdate_button;
    std::unique_ptr<juce::Label> gain_1;
    std::unique_ptr<juce::Label> gain_2;
    std::unique_ptr<juce::Label> gain_3;
    std::unique_ptr<juce::Label> gain_4;
    std::unique_ptr<juce::Label> gain_5;
    std::unique_ptr<juce::Label> gain_6;
    std::unique_ptr<juce::Label> gain_7;
    std::unique_ptr<juce::Label> gain_8;
    std::unique_ptr<CustomButton> undo_button;
    std::unique_ptr<CustomButton> redo_button;
    std::unique_ptr<CustomButton> save_preset_button;
    std::unique_ptr<CustomButton> load_preset_button;
    std::unique_ptr<juce::Label> active_label2;
    juce::Image cachedImage_anticlockwise_arrow_png_1;
    juce::Image cachedImage_clockwise_arrow_png_2;
    juce::Image cachedImage_load_icon_png_3;
    juce::Image cachedImage_save_icon_png_4;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

