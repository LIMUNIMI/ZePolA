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

//[Headers] You can add your own extra header files here...
#include "Parameters.h"
#include <cmath>
#include <JuceHeader.h>
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PluginEditor::PluginEditor (PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor (p), valueTreeState (vts)
{
    //[Constructor_pre] You can add your own custom stuff here..
    p.setEditorCallback([this]()
                        {
        getSpectrum();
        frequency_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb);
        phase_response->updateValues(phases, referenceFrequencies, processor.getSampleRate(), true);
        gaussian_plane->updateElements(processor.getFilterElementsChain());
    });

    selectable_filter_types = SELECTABLE_FILTER_TYPES;
    selectable_orders_butterworth = SELECTABLE_ORDERS_BUTTERWORTH;

    getSpectrum();
    updateReferenceFrequencies();

    magnitudesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    phasesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    typesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    activeAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    //[/Constructor_pre]

    passbandAmplitude_label.reset (new juce::Label ("Passband Amplitude",
                                                    TRANS ("PASSBAND AMPLITUDE (DB)")));
    addAndMakeVisible (passbandAmplitude_label.get());
    passbandAmplitude_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    passbandAmplitude_label->setJustificationType (juce::Justification::centred);
    passbandAmplitude_label->setEditable (false, false, false);
    passbandAmplitude_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    passbandAmplitude_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    passbandAmplitude_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    passbandAmplitude_label->setBounds (1020, 320, 159, 20);

    stopbandAmplitude_slider.reset (new juce::Slider ("Stopband Amplitude"));
    addAndMakeVisible (stopbandAmplitude_slider.get());
    stopbandAmplitude_slider->setRange (-35, -21, 0.1);
    stopbandAmplitude_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    stopbandAmplitude_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    stopbandAmplitude_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    stopbandAmplitude_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    stopbandAmplitude_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    stopbandAmplitude_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    stopbandAmplitude_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    stopbandAmplitude_slider->addListener (this);

    stopbandAmplitude_slider->setBounds (1022, 326, 135, 25);

    gaussian_plane.reset (new GaussianPlane (processor.getFilterElementsChain()));
    addAndMakeVisible (gaussian_plane.get());
    gaussian_plane->setName ("gaussianPlane");

    gaussian_plane->setBounds (30, 415, 260, 260);

    reset_button.reset (new juce::TextButton ("Reset"));
    addAndMakeVisible (reset_button.get());
    reset_button->setButtonText (juce::String());
    reset_button->addListener (this);
    reset_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    reset_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    reset_button->setBounds (360, 568, 70, 30);

    frequency_response.reset (new FrequencyResponse (magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb));
    addAndMakeVisible (frequency_response.get());
    frequency_response->setName ("frequencyResponse");

    frequency_response->setBounds (550, 35, 430, 285);

    freq_response_label.reset (new juce::Label ("Frequency response",
                                                TRANS ("SPECTRUM MAGNITUDE\n")));
    addAndMakeVisible (freq_response_label.get());
    freq_response_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    freq_response_label->setJustificationType (juce::Justification::centred);
    freq_response_label->setEditable (false, false, false);
    freq_response_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    freq_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freq_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    freq_response_label->setBounds (695, 325, 140, 24);

    phase_response.reset (new PhaseResponse (phases, referenceFrequencies, processor.getSampleRate(), ampDb));
    addAndMakeVisible (phase_response.get());
    phase_response->setName ("phaseResponse");

    phase_response->setBounds (550, 390, 430, 285);

    ph_response_label.reset (new juce::Label ("Phase response",
                                              TRANS ("SPECTRUM PHASE")));
    addAndMakeVisible (ph_response_label.get());
    ph_response_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    ph_response_label->setJustificationType (juce::Justification::centred);
    ph_response_label->setEditable (false, false, false);
    ph_response_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    ph_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    ph_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    ph_response_label->setBounds (705, 680, 110, 24);

    m1_slider.reset (new juce::Slider ("Element 1 magnitude"));
    addAndMakeVisible (m1_slider.get());
    m1_slider->setRange (0, 10, 0);
    m1_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m1_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m1_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m1_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m1_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m1_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m1_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m1_slider->addListener (this);

    m1_slider->setBounds (20, 55, 120, 25);

    p1_slider.reset (new juce::Slider ("Element 1 phase"));
    addAndMakeVisible (p1_slider.get());
    p1_slider->setRange (0, 10, 0);
    p1_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p1_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p1_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    p1_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p1_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p1_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p1_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p1_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p1_slider->addListener (this);

    p1_slider->setBounds (144, 55, 120, 25);

    magnitudes_label.reset (new juce::Label ("Magnitudes",
                                             TRANS ("MAGNITUDE\n")));
    addAndMakeVisible (magnitudes_label.get());
    magnitudes_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    magnitudes_label->setJustificationType (juce::Justification::centred);
    magnitudes_label->setEditable (false, false, false);
    magnitudes_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    magnitudes_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    magnitudes_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    magnitudes_label->setBounds (33, 20, 90, 24);

    phases_label.reset (new juce::Label ("Phases",
                                         TRANS ("FREQUENCY\n")));
    addAndMakeVisible (phases_label.get());
    phases_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    phases_label->setJustificationType (juce::Justification::centred);
    phases_label->setEditable (false, false, false);
    phases_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    phases_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    phases_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    phases_label->setBounds (198, 20, 80, 24);

    zero_pole_label.reset (new juce::Label ("Filter Type",
                                            TRANS ("FILTER TYPE")));
    addAndMakeVisible (zero_pole_label.get());
    zero_pole_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    zero_pole_label->setJustificationType (juce::Justification::centred);
    zero_pole_label->setEditable (false, false, false);
    zero_pole_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    zero_pole_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    zero_pole_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    zero_pole_label->setBounds (317, 20, 91, 24);

    active_label.reset (new juce::Label ("Active",
                                         TRANS ("ACTIVE")));
    addAndMakeVisible (active_label.get());
    active_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    active_label->setJustificationType (juce::Justification::centred);
    active_label->setEditable (false, false, false);
    active_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    active_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    active_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    active_label->setBounds (408, 20, 55, 24);

    e1_type.reset (new juce::ToggleButton ("Element 1 type"));
    addAndMakeVisible (e1_type.get());
    e1_type->setButtonText (juce::String());
    e1_type->addListener (this);

    e1_type->setBounds (338, 56, 52, 21);

    e2_type.reset (new juce::ToggleButton ("Element 2 type"));
    addAndMakeVisible (e2_type.get());
    e2_type->setButtonText (juce::String());
    e2_type->addListener (this);

    e2_type->setBounds (338, 101, 52, 21);

    e3_type.reset (new juce::ToggleButton ("Element 3 type"));
    addAndMakeVisible (e3_type.get());
    e3_type->setButtonText (juce::String());
    e3_type->addListener (this);

    e3_type->setBounds (338, 146, 52, 21);

    e4_type.reset (new juce::ToggleButton ("Element 4 type"));
    addAndMakeVisible (e4_type.get());
    e4_type->setButtonText (juce::String());
    e4_type->addListener (this);

    e4_type->setBounds (338, 191, 52, 21);

    e5_type.reset (new juce::ToggleButton ("Element 5 type"));
    addAndMakeVisible (e5_type.get());
    e5_type->setButtonText (juce::String());
    e5_type->addListener (this);

    e5_type->setBounds (338, 236, 52, 21);

    e6_type.reset (new juce::ToggleButton ("Element 6 type"));
    addAndMakeVisible (e6_type.get());
    e6_type->setButtonText (juce::String());
    e6_type->addListener (this);

    e6_type->setBounds (338, 281, 52, 21);

    e7_type.reset (new juce::ToggleButton ("Element 7 type"));
    addAndMakeVisible (e7_type.get());
    e7_type->setButtonText (juce::String());
    e7_type->addListener (this);

    e7_type->setBounds (338, 326, 52, 21);

    e8_type.reset (new juce::ToggleButton ("Element 8 type"));
    addAndMakeVisible (e8_type.get());
    e8_type->setButtonText (juce::String());
    e8_type->addListener (this);

    e8_type->setBounds (338, 371, 52, 21);

    e1_active.reset (new juce::ToggleButton ("Element 1 active"));
    addAndMakeVisible (e1_active.get());
    e1_active->setButtonText (juce::String());
    e1_active->addListener (this);

    e1_active->setBounds (411, 56, 49, 21);

    e2_active.reset (new juce::ToggleButton ("Element 2 active"));
    addAndMakeVisible (e2_active.get());
    e2_active->setButtonText (juce::String());
    e2_active->addListener (this);

    e2_active->setBounds (411, 101, 49, 21);

    e3_active.reset (new juce::ToggleButton ("Element 3 active"));
    addAndMakeVisible (e3_active.get());
    e3_active->setButtonText (juce::String());
    e3_active->addListener (this);

    e3_active->setBounds (411, 146, 49, 21);

    e4_active.reset (new juce::ToggleButton ("Element 4 active"));
    addAndMakeVisible (e4_active.get());
    e4_active->setButtonText (juce::String());
    e4_active->addListener (this);

    e4_active->setBounds (411, 191, 49, 21);

    e5_active.reset (new juce::ToggleButton ("Element 5 active"));
    addAndMakeVisible (e5_active.get());
    e5_active->setButtonText (juce::String());
    e5_active->addListener (this);

    e5_active->setBounds (411, 236, 49, 21);

    e6_active.reset (new juce::ToggleButton ("Element 6 active"));
    addAndMakeVisible (e6_active.get());
    e6_active->setButtonText (juce::String());
    e6_active->addListener (this);

    e6_active->setBounds (411, 281, 49, 21);

    e7_active.reset (new juce::ToggleButton ("Element 7 active"));
    addAndMakeVisible (e7_active.get());
    e7_active->setButtonText (juce::String());
    e7_active->addListener (this);

    e7_active->setBounds (411, 326, 49, 21);

    e8_active.reset (new juce::ToggleButton ("Element 8 active"));
    addAndMakeVisible (e8_active.get());
    e8_active->setButtonText (juce::String());
    e8_active->addListener (this);

    e8_active->setBounds (411, 371, 49, 21);

    gaussian_plane_label.reset (new juce::Label ("Gaussian plane",
                                                 TRANS ("GAUSSIAN PLANE")));
    addAndMakeVisible (gaussian_plane_label.get());
    gaussian_plane_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    gaussian_plane_label->setJustificationType (juce::Justification::centred);
    gaussian_plane_label->setEditable (false, false, false);
    gaussian_plane_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gaussian_plane_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gaussian_plane_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    gaussian_plane_label->setBounds (95, 680, 140, 24);

    bypass.reset (new juce::ToggleButton ("Bypass"));
    addAndMakeVisible (bypass.get());
    bypass->setButtonText (juce::String());

    bypass->setBounds (1067, 672, 60, 25);

    gain_slider.reset (new juce::Slider ("Gain"));
    addAndMakeVisible (gain_slider.get());
    gain_slider->setRange (0, 10, 0);
    gain_slider->setSliderStyle (juce::Slider::LinearVertical);
    gain_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    gain_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    gain_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    gain_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    gain_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x008e989b));

    gain_slider->setBounds (1074, 510, 45, 150);

    linLog_switch.reset (new juce::ToggleButton ("Linear / Logarithmic"));
    addAndMakeVisible (linLog_switch.get());
    linLog_switch->setButtonText (juce::String());
    linLog_switch->addListener (this);

    linLog_switch->setBounds (931, 327, 52, 21);

    m2_slider.reset (new juce::Slider ("Element 2 magnitude"));
    addAndMakeVisible (m2_slider.get());
    m2_slider->setRange (0, 10, 0);
    m2_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m2_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m2_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m2_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m2_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m2_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m2_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m2_slider->addListener (this);

    m2_slider->setBounds (20, 100, 120, 25);

    m3_slider.reset (new juce::Slider ("Element 3 magnitude"));
    addAndMakeVisible (m3_slider.get());
    m3_slider->setRange (0, 10, 0);
    m3_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m3_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m3_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m3_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m3_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m3_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m3_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m3_slider->addListener (this);

    m3_slider->setBounds (20, 145, 120, 25);

    m4_slider.reset (new juce::Slider ("Element 4 magnitude"));
    addAndMakeVisible (m4_slider.get());
    m4_slider->setRange (0, 10, 0);
    m4_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m4_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m4_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m4_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m4_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m4_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m4_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m4_slider->addListener (this);

    m4_slider->setBounds (20, 190, 120, 25);

    m5_slider.reset (new juce::Slider ("Element 5 magnitude"));
    addAndMakeVisible (m5_slider.get());
    m5_slider->setRange (0, 10, 0);
    m5_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m5_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m5_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m5_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m5_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m5_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m5_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m5_slider->addListener (this);

    m5_slider->setBounds (20, 235, 120, 25);

    m6_slider.reset (new juce::Slider ("Element 6 magnitude"));
    addAndMakeVisible (m6_slider.get());
    m6_slider->setRange (0, 10, 0);
    m6_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m6_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m6_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m6_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m6_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m6_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m6_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m6_slider->addListener (this);

    m6_slider->setBounds (20, 280, 120, 25);

    m7_slider.reset (new juce::Slider ("Element 7 magnitude"));
    addAndMakeVisible (m7_slider.get());
    m7_slider->setRange (0, 10, 0);
    m7_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m7_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m7_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m7_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m7_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m7_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m7_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m7_slider->addListener (this);

    m7_slider->setBounds (20, 325, 120, 25);

    m8_slider.reset (new juce::Slider ("Element 8 magnitude"));
    addAndMakeVisible (m8_slider.get());
    m8_slider->setRange (0, 10, 0);
    m8_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m8_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m8_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    m8_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    m8_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m8_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m8_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m8_slider->addListener (this);

    m8_slider->setBounds (20, 370, 120, 25);

    p2_slider.reset (new juce::Slider ("Element 2 phase"));
    addAndMakeVisible (p2_slider.get());
    p2_slider->setRange (0, 10, 0);
    p2_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p2_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p2_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p2_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p2_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p2_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p2_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p2_slider->addListener (this);

    p2_slider->setBounds (144, 100, 120, 25);

    p3_slider.reset (new juce::Slider ("Element 3 phase"));
    addAndMakeVisible (p3_slider.get());
    p3_slider->setRange (0, 10, 0);
    p3_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p3_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p3_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p3_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p3_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p3_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p3_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p3_slider->addListener (this);

    p3_slider->setBounds (144, 145, 120, 25);

    p4_slider.reset (new juce::Slider ("Element 4 phase"));
    addAndMakeVisible (p4_slider.get());
    p4_slider->setRange (0, 10, 0);
    p4_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p4_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p4_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p4_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p4_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p4_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p4_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p4_slider->addListener (this);

    p4_slider->setBounds (144, 190, 120, 25);

    p5_slider.reset (new juce::Slider ("Element 5 phase"));
    addAndMakeVisible (p5_slider.get());
    p5_slider->setRange (0, 10, 0);
    p5_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p5_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p5_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p5_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p5_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p5_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p5_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p5_slider->addListener (this);

    p5_slider->setBounds (144, 235, 120, 25);

    p6_slider.reset (new juce::Slider ("Element 6 phase"));
    addAndMakeVisible (p6_slider.get());
    p6_slider->setRange (0, 10, 0);
    p6_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p6_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p6_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p6_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p6_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p6_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p6_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p6_slider->addListener (this);

    p6_slider->setBounds (144, 280, 120, 25);

    p7_slider.reset (new juce::Slider ("Element 7 phase"));
    addAndMakeVisible (p7_slider.get());
    p7_slider->setRange (0, 10, 0);
    p7_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p7_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p7_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p7_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p7_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p7_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p7_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p7_slider->addListener (this);

    p7_slider->setBounds (144, 325, 120, 25);

    p8_slider.reset (new juce::Slider ("Element 8 phase"));
    addAndMakeVisible (p8_slider.get());
    p8_slider->setRange (0, 10, 0);
    p8_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p8_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p8_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    p8_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    p8_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p8_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p8_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p8_slider->addListener (this);

    p8_slider->setBounds (144, 370, 120, 25);

    p1_freq.reset (new juce::Label ("Element 1 frequency",
                                    juce::String()));
    addAndMakeVisible (p1_freq.get());
    p1_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p1_freq->setJustificationType (juce::Justification::centredLeft);
    p1_freq->setEditable (true, true, false);
    p1_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p1_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p1_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p1_freq->addListener (this);

    p1_freq->setBounds (270, 55, 60, 25);

    p2_freq.reset (new juce::Label ("Element 2 frequency",
                                    juce::String()));
    addAndMakeVisible (p2_freq.get());
    p2_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p2_freq->setJustificationType (juce::Justification::centredLeft);
    p2_freq->setEditable (true, true, false);
    p2_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p2_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p2_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p2_freq->addListener (this);

    p2_freq->setBounds (270, 100, 60, 25);

    p3_freq.reset (new juce::Label ("Element 3 frequency",
                                    juce::String()));
    addAndMakeVisible (p3_freq.get());
    p3_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p3_freq->setJustificationType (juce::Justification::centredLeft);
    p3_freq->setEditable (true, true, false);
    p3_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p3_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p3_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p3_freq->addListener (this);

    p3_freq->setBounds (270, 145, 60, 25);

    p4_freq.reset (new juce::Label ("Element 4 frequency",
                                    juce::String()));
    addAndMakeVisible (p4_freq.get());
    p4_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p4_freq->setJustificationType (juce::Justification::centredLeft);
    p4_freq->setEditable (true, true, false);
    p4_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p4_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p4_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p4_freq->addListener (this);

    p4_freq->setBounds (270, 190, 60, 25);

    p5_freq.reset (new juce::Label ("Element 5 frequency",
                                    juce::String()));
    addAndMakeVisible (p5_freq.get());
    p5_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p5_freq->setJustificationType (juce::Justification::centredLeft);
    p5_freq->setEditable (true, true, false);
    p5_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p5_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p5_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p5_freq->addListener (this);

    p5_freq->setBounds (270, 235, 60, 25);

    p6_freq.reset (new juce::Label ("Element 6 frequency",
                                    juce::String()));
    addAndMakeVisible (p6_freq.get());
    p6_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p6_freq->setJustificationType (juce::Justification::centredLeft);
    p6_freq->setEditable (true, true, false);
    p6_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p6_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p6_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p6_freq->addListener (this);

    p6_freq->setBounds (270, 280, 60, 25);

    p7_freq.reset (new juce::Label ("Element 7 frequency",
                                    juce::String()));
    addAndMakeVisible (p7_freq.get());
    p7_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p7_freq->setJustificationType (juce::Justification::centredLeft);
    p7_freq->setEditable (true, true, false);
    p7_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p7_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p7_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p7_freq->addListener (this);

    p7_freq->setBounds (270, 325, 60, 25);

    p8_freq.reset (new juce::Label ("Element 8 frequency",
                                    juce::String()));
    addAndMakeVisible (p8_freq.get());
    p8_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p8_freq->setJustificationType (juce::Justification::centredLeft);
    p8_freq->setEditable (true, true, false);
    p8_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p8_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p8_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p8_freq->addListener (this);

    p8_freq->setBounds (270, 370, 60, 25);

    type_box.reset (new juce::ComboBox ("Design type"));
    addAndMakeVisible (type_box.get());
    type_box->setEditableText (false);
    type_box->setJustificationType (juce::Justification::centredLeft);
    type_box->setTextWhenNothingSelected (TRANS ("TYPE"));
    type_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    type_box->addListener (this);

    type_box->setBounds (1025, 110, 140, 25);

    shape_box.reset (new juce::ComboBox ("Design shape"));
    addAndMakeVisible (shape_box.get());
    shape_box->setEditableText (false);
    shape_box->setJustificationType (juce::Justification::centredLeft);
    shape_box->setTextWhenNothingSelected (TRANS ("SHAPE"));
    shape_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    shape_box->addItem (TRANS ("LOWPASS"), 1);
    shape_box->addItem (TRANS ("HIGHPASS"), 2);
    shape_box->addListener (this);

    shape_box->setBounds (1025, 65, 140, 25);

    calculate_button.reset (new juce::TextButton ("Calculate"));
    addAndMakeVisible (calculate_button.get());
    calculate_button->setButtonText (juce::String());
    calculate_button->addListener (this);
    calculate_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    calculate_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    calculate_button->setBounds (1098, 390, 80, 30);

    multiply_phases_button.reset (new juce::TextButton ("Multiply phases"));
    addAndMakeVisible (multiply_phases_button.get());
    multiply_phases_button->setButtonText (juce::String());
    multiply_phases_button->addListener (this);
    multiply_phases_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    multiply_phases_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    multiply_phases_button->setBounds (319, 477, 72, 22);

    divide_phases_button.reset (new juce::TextButton ("Divide phases"));
    addAndMakeVisible (divide_phases_button.get());
    divide_phases_button->setButtonText (juce::String());
    divide_phases_button->addListener (this);
    divide_phases_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    divide_phases_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff727272));

    divide_phases_button->setBounds (409, 477, 72, 22);

    swap_button.reset (new juce::TextButton ("Swap poles/zeros"));
    addAndMakeVisible (swap_button.get());
    swap_button->setButtonText (juce::String());
    swap_button->addListener (this);
    swap_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    swap_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    swap_button->setBounds (359, 512, 82, 22);

    turn_on_button.reset (new juce::TextButton ("Turn on all the elements"));
    addAndMakeVisible (turn_on_button.get());
    turn_on_button->setButtonText (juce::String());
    turn_on_button->addListener (this);
    turn_on_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff73cc81));
    turn_on_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    turn_on_button->setBounds (319, 442, 72, 22);

    turn_off_button.reset (new juce::TextButton ("Turn off all the elements"));
    addAndMakeVisible (turn_off_button.get());
    turn_off_button->setButtonText (juce::String());
    turn_off_button->addListener (this);
    turn_off_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xffe86d5c));
    turn_off_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    turn_off_button->setBounds (409, 442, 72, 22);

    order_box.reset (new juce::ComboBox ("Design order"));
    addAndMakeVisible (order_box.get());
    order_box->setEditableText (false);
    order_box->setJustificationType (juce::Justification::centredLeft);
    order_box->setTextWhenNothingSelected (TRANS ("ORDER"));
    order_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    order_box->addListener (this);

    order_box->setBounds (1025, 155, 140, 25);

    design_frequency_label.reset (new juce::Label ("Design frequency",
                                                   TRANS ("FREQUENCY\n")));
    addAndMakeVisible (design_frequency_label.get());
    design_frequency_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    design_frequency_label->setJustificationType (juce::Justification::centred);
    design_frequency_label->setEditable (false, false, false);
    design_frequency_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    design_frequency_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    design_frequency_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    design_frequency_label->setBounds (1063, 193, 72, 20);

    ampDb_switch.reset (new juce::ToggleButton ("Amplitude / dB"));
    addAndMakeVisible (ampDb_switch.get());
    ampDb_switch->setButtonText (juce::String());
    ampDb_switch->addListener (this);

    ampDb_switch->setBounds (546, 327, 52, 21);

    frequency_design_slider.reset (new juce::Slider ("Frequency design slider"));
    addAndMakeVisible (frequency_design_slider.get());
    frequency_design_slider->setRange (0.0001, 1000, 0.0001);
    frequency_design_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    frequency_design_slider->setTextBoxStyle (juce::Slider::NoTextBox, true, 50, 20);
    frequency_design_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    frequency_design_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    frequency_design_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    frequency_design_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    frequency_design_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    frequency_design_slider->addListener (this);

    frequency_design_slider->setBounds (1022, 215, 83, 25);

    frequency_label.reset (new juce::Label ("Frequency Label",
                                            juce::String()));
    addAndMakeVisible (frequency_label.get());
    frequency_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    frequency_label->setJustificationType (juce::Justification::centredLeft);
    frequency_label->setEditable (true, true, false);
    frequency_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    frequency_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    frequency_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    frequency_label->addListener (this);

    frequency_label->setBounds (1110, 215, 60, 25);

    transition_width_label.reset (new juce::Label ("Transition width",
                                                   TRANS ("TRANSITION WIDTH")));
    addAndMakeVisible (transition_width_label.get());
    transition_width_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    transition_width_label->setJustificationType (juce::Justification::centred);
    transition_width_label->setEditable (false, false, false);
    transition_width_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    transition_width_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    transition_width_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    transition_width_label->setBounds (1031, 256, 129, 20);

    transition_width_slider.reset (new juce::Slider ("Transition width slider"));
    addAndMakeVisible (transition_width_slider.get());
    transition_width_slider->setRange (0.001, 0.5, 1e-05);
    transition_width_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    transition_width_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    transition_width_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    transition_width_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    transition_width_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    transition_width_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    transition_width_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    transition_width_slider->addListener (this);

    transition_width_slider->setBounds (1022, 280, 135, 25);

    passbandAmplitude_slider.reset (new juce::Slider ("Passband Amplitude"));
    addAndMakeVisible (passbandAmplitude_slider.get());
    passbandAmplitude_slider->setRange (-10, -0.1, 0.1);
    passbandAmplitude_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    passbandAmplitude_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    passbandAmplitude_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    passbandAmplitude_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    passbandAmplitude_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    passbandAmplitude_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    passbandAmplitude_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    passbandAmplitude_slider->addListener (this);

    passbandAmplitude_slider->setBounds (1022, 344, 135, 25);

    stopbandAmplitude_label.reset (new juce::Label ("Stopband Amplitude",
                                                    TRANS ("STOPBAND AMPLITUDE (DB)")));
    addAndMakeVisible (stopbandAmplitude_label.get());
    stopbandAmplitude_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    stopbandAmplitude_label->setJustificationType (juce::Justification::centred);
    stopbandAmplitude_label->setEditable (false, false, false);
    stopbandAmplitude_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    stopbandAmplitude_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    stopbandAmplitude_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    stopbandAmplitude_label->setBounds (1020, 302, 159, 20);

    autoUpdate_button.reset (new juce::ToggleButton ("Auto update"));
    addAndMakeVisible (autoUpdate_button.get());
    autoUpdate_button->setButtonText (juce::String());
    autoUpdate_button->addListener (this);

    autoUpdate_button->setBounds (1013, 390, 80, 30);

    gain_1.reset (new juce::Label ("Gain 1 label",
                                   juce::String()));
    addAndMakeVisible (gain_1.get());
    gain_1->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_1->setJustificationType (juce::Justification::centredLeft);
    gain_1->setEditable (false, false, false);
    gain_1->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_1->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_1->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_1->setBounds (470, 55, 45, 25);

    gain_2.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_2.get());
    gain_2->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_2->setJustificationType (juce::Justification::centredLeft);
    gain_2->setEditable (false, false, false);
    gain_2->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_2->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_2->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_2->setBounds (470, 100, 45, 25);

    gain_3.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_3.get());
    gain_3->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_3->setJustificationType (juce::Justification::centredLeft);
    gain_3->setEditable (false, false, false);
    gain_3->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_3->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_3->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_3->setBounds (470, 145, 45, 25);

    gain_4.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_4.get());
    gain_4->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_4->setJustificationType (juce::Justification::centredLeft);
    gain_4->setEditable (false, false, false);
    gain_4->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_4->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_4->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_4->setBounds (470, 190, 45, 25);

    gain_5.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_5.get());
    gain_5->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_5->setJustificationType (juce::Justification::centredLeft);
    gain_5->setEditable (false, false, false);
    gain_5->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_5->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_5->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_5->setBounds (470, 235, 45, 25);

    gain_6.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_6.get());
    gain_6->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_6->setJustificationType (juce::Justification::centredLeft);
    gain_6->setEditable (false, false, false);
    gain_6->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_6->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_6->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_6->setBounds (470, 280, 45, 25);

    gain_7.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_7.get());
    gain_7->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_7->setJustificationType (juce::Justification::centredLeft);
    gain_7->setEditable (false, false, false);
    gain_7->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_7->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_7->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_7->setBounds (470, 325, 45, 25);

    gain_8.reset (new juce::Label ("Gain 2 label",
                                   juce::String()));
    addAndMakeVisible (gain_8.get());
    gain_8->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_8->setJustificationType (juce::Justification::centredLeft);
    gain_8->setEditable (false, false, false);
    gain_8->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_8->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_8->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);

    gain_8->setBounds (470, 370, 45, 25);


    //[UserPreSize]
    magnitudesAttachments[0].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(1), *m1_slider));
    magnitudesAttachments[1].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(2), *m2_slider));
    magnitudesAttachments[2].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(3), *m3_slider));
    magnitudesAttachments[3].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(4), *m4_slider));
    magnitudesAttachments[4].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(5), *m5_slider));
    magnitudesAttachments[5].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(6), *m6_slider));
    magnitudesAttachments[6].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(7), *m7_slider));
    magnitudesAttachments[7].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(8), *m8_slider));

    phasesAttachments[0].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(1), *p1_slider));
    phasesAttachments[1].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(2), *p2_slider));
    phasesAttachments[2].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(3), *p3_slider));
    phasesAttachments[3].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(4), *p4_slider));
    phasesAttachments[4].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(5), *p5_slider));
    phasesAttachments[5].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(6), *p6_slider));
    phasesAttachments[6].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(7), *p7_slider));
    phasesAttachments[7].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(8), *p8_slider));

    typesAttachments[0].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(1), *e1_type));
    typesAttachments[1].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(2), *e2_type));
    typesAttachments[2].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(3), *e3_type));
    typesAttachments[3].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(4), *e4_type));
    typesAttachments[4].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(5), *e5_type));
    typesAttachments[5].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(6), *e6_type));
    typesAttachments[6].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(7), *e7_type));
    typesAttachments[7].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(8), *e8_type));

    activeAttachments[0].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(1), *e1_active));
    activeAttachments[1].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(2), *e2_active));
    activeAttachments[2].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(3), *e3_active));
    activeAttachments[3].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(4), *e4_active));
    activeAttachments[4].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(5), *e5_active));
    activeAttachments[5].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(6), *e6_active));
    activeAttachments[6].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(7), *e7_active));
    activeAttachments[7].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(8), *e8_active));

    gainAttachment.reset(new SliderAttachment(valueTreeState, GAIN_NAME, *gain_slider));
    bypassAttachment.reset(new ButtonAttachment(valueTreeState, FILTER_BYPASS_NAME, *bypass));

    m1_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m2_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m3_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m4_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m5_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m6_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m7_slider->setLookAndFeel(&magnitudeSlidersTheme);
    m8_slider->setLookAndFeel(&magnitudeSlidersTheme);

    p1_slider->setLookAndFeel(&phaseSlidersTheme);
    p2_slider->setLookAndFeel(&phaseSlidersTheme);
    p3_slider->setLookAndFeel(&phaseSlidersTheme);
    p4_slider->setLookAndFeel(&phaseSlidersTheme);
    p5_slider->setLookAndFeel(&phaseSlidersTheme);
    p6_slider->setLookAndFeel(&phaseSlidersTheme);
    p7_slider->setLookAndFeel(&phaseSlidersTheme);
    p8_slider->setLookAndFeel(&phaseSlidersTheme);

    gain_slider->setLookAndFeel(&gainSliderTheme);

    e1_type->setLookAndFeel(&typeSwitchesTheme);
    e2_type->setLookAndFeel(&typeSwitchesTheme);
    e3_type->setLookAndFeel(&typeSwitchesTheme);
    e4_type->setLookAndFeel(&typeSwitchesTheme);
    e5_type->setLookAndFeel(&typeSwitchesTheme);
    e6_type->setLookAndFeel(&typeSwitchesTheme);
    e7_type->setLookAndFeel(&typeSwitchesTheme);
    e8_type->setLookAndFeel(&typeSwitchesTheme);

    linLog_switch->setLookAndFeel(&linLogTheme);
    ampDb_switch->setLookAndFeel(&ampDbTheme);

    e1_active->setLookAndFeel(&activeSwitchesTheme);
    e2_active->setLookAndFeel(&activeSwitchesTheme);
    e3_active->setLookAndFeel(&activeSwitchesTheme);
    e4_active->setLookAndFeel(&activeSwitchesTheme);
    e5_active->setLookAndFeel(&activeSwitchesTheme);
    e6_active->setLookAndFeel(&activeSwitchesTheme);
    e7_active->setLookAndFeel(&activeSwitchesTheme);
    e8_active->setLookAndFeel(&activeSwitchesTheme);

    bypass->setLookAndFeel(&bypassSwitchTheme);

    resetButtonTheme.setTextToDisplay("RESET");
    resetButtonTheme.setFontSize(12.0f);
    reset_button->setLookAndFeel(&resetButtonTheme);

    calculateButtonTheme.setTextToDisplay("CALCULATE");
    calculateButtonTheme.setFontSize(12.0f);
    calculate_button->setLookAndFeel(&calculateButtonTheme);

    swapButtonTheme.setTextToDisplay("SWAP POLES/ZEROS");
    swap_button->setLookAndFeel(&swapButtonTheme);

    multiplyPhasesTheme.setTextToDisplay("DOUBLE PHASES");
    multiply_phases_button->setLookAndFeel(&multiplyPhasesTheme);

    dividePhasesTheme.setTextToDisplay("HALF PHASES");
    divide_phases_button->setLookAndFeel(&dividePhasesTheme);

    turnAllOnTheme.setTextToDisplay("ALL ON");
    turn_on_button->setLookAndFeel(&turnAllOnTheme);

    turnAllOffTheme.setTextToDisplay("ALL OFF");
    turn_off_button->setLookAndFeel(&turnAllOffTheme);

    type_box->setLookAndFeel(&comboBoxTheme);
    order_box->setLookAndFeel(&comboBoxTheme);
    shape_box->setLookAndFeel(&comboBoxTheme);

    frequency_design_slider->setLookAndFeel(&magnitudeSlidersTheme);
    frequency_design_slider->setSkewFactor(0.2);
    transition_width_slider->setLookAndFeel(&magnitudeSlidersTheme);
    passbandAmplitude_slider->setLookAndFeel(&magnitudeSlidersTheme);
    stopbandAmplitude_slider->setLookAndFeel(&magnitudeSlidersTheme);

    double sampleRate = processor.getSampleRate();

    updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);
    updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);
    updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);
    updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);
    updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);
    updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);
    updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);
    updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);

    linLog = false;
    linLog_switch->setToggleState(false, juce::dontSendNotification);

    transition_width_label->setVisible(false);
    transition_width_slider->setVisible(false);

    passbandAmplitude_label->setVisible(false);
    passbandAmplitude_slider->setVisible(false);

    stopbandAmplitude_label->setVisible(false);
    stopbandAmplitude_slider->setVisible(false);

    calculate_button->setEnabled(false);

    updateDesignSliderFromFrequency(DESIGN_FREQUENCY_FLOOR, frequency_design_slider.get(), sampleRate);

    autoUpdate_button->setLookAndFeel(&autoUpdateSwitchTheme);

    gain_1->setVisible(e1_active->getToggleState());
    gain_2->setVisible(e2_active->getToggleState());
    gain_3->setVisible(e3_active->getToggleState());
    gain_4->setVisible(e4_active->getToggleState());
    gain_5->setVisible(e5_active->getToggleState());
    gain_6->setVisible(e6_active->getToggleState());
    gain_7->setVisible(e7_active->getToggleState());
    gain_8->setVisible(e8_active->getToggleState());

    //[/UserPreSize]

    setSize (1200, 750);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

PluginEditor::~PluginEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    for (int i = 0; i < NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        magnitudesAttachments[i].reset();
        phasesAttachments[i].reset();
        typesAttachments[i].reset();
        activeAttachments[i].reset();
    }
    gainAttachment.reset();
    bypassAttachment.reset();
    //[/Destructor_pre]

    passbandAmplitude_label = nullptr;
    stopbandAmplitude_slider = nullptr;
    gaussian_plane = nullptr;
    reset_button = nullptr;
    frequency_response = nullptr;
    freq_response_label = nullptr;
    phase_response = nullptr;
    ph_response_label = nullptr;
    m1_slider = nullptr;
    p1_slider = nullptr;
    magnitudes_label = nullptr;
    phases_label = nullptr;
    zero_pole_label = nullptr;
    active_label = nullptr;
    e1_type = nullptr;
    e2_type = nullptr;
    e3_type = nullptr;
    e4_type = nullptr;
    e5_type = nullptr;
    e6_type = nullptr;
    e7_type = nullptr;
    e8_type = nullptr;
    e1_active = nullptr;
    e2_active = nullptr;
    e3_active = nullptr;
    e4_active = nullptr;
    e5_active = nullptr;
    e6_active = nullptr;
    e7_active = nullptr;
    e8_active = nullptr;
    gaussian_plane_label = nullptr;
    bypass = nullptr;
    gain_slider = nullptr;
    linLog_switch = nullptr;
    m2_slider = nullptr;
    m3_slider = nullptr;
    m4_slider = nullptr;
    m5_slider = nullptr;
    m6_slider = nullptr;
    m7_slider = nullptr;
    m8_slider = nullptr;
    p2_slider = nullptr;
    p3_slider = nullptr;
    p4_slider = nullptr;
    p5_slider = nullptr;
    p6_slider = nullptr;
    p7_slider = nullptr;
    p8_slider = nullptr;
    p1_freq = nullptr;
    p2_freq = nullptr;
    p3_freq = nullptr;
    p4_freq = nullptr;
    p5_freq = nullptr;
    p6_freq = nullptr;
    p7_freq = nullptr;
    p8_freq = nullptr;
    type_box = nullptr;
    shape_box = nullptr;
    calculate_button = nullptr;
    multiply_phases_button = nullptr;
    divide_phases_button = nullptr;
    swap_button = nullptr;
    turn_on_button = nullptr;
    turn_off_button = nullptr;
    order_box = nullptr;
    design_frequency_label = nullptr;
    ampDb_switch = nullptr;
    frequency_design_slider = nullptr;
    frequency_label = nullptr;
    transition_width_label = nullptr;
    transition_width_slider = nullptr;
    passbandAmplitude_slider = nullptr;
    stopbandAmplitude_label = nullptr;
    autoUpdate_button = nullptr;
    gain_1 = nullptr;
    gain_2 = nullptr;
    gain_3 = nullptr;
    gain_4 = nullptr;
    gain_5 = nullptr;
    gain_6 = nullptr;
    gain_7 = nullptr;
    gain_8 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xffecf0f1));

    {
        float x = 15.0f, y = 15.0f, width = 510.0f, height = 720.0f;
        juce::Colour fillColour = juce::Colour (0x17b1b1b1);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 1.500f);
    }

    {
        float x = 1005.0f, y = 15.0f, width = 180.0f, height = 720.0f;
        juce::Colour fillColour = juce::Colour (0x19656565);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 1.500f);
    }

    {
        float x = 525.0f, y = 15.0f, width = 480.0f, height = 720.0f;
        juce::Colour fillColour = juce::Colour (0x17b1b1b1);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 1.500f);
    }

    {
        int x = 1097, y = 708, width = 90, height = 30;
        juce::String text (TRANS ("TOOLS"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 16.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 15, y = 90, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 395, y = 708, width = 116, height = 30;
        juce::String text (TRANS ("FILTER SETUP"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 16.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 810, y = 708, width = 186, height = 30;
        juce::String text (TRANS ("FILTER RESPONSE"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 16.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 15, y = 135, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 180, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 225, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 270, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 360, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 315, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 1005, y = 445, width = 180, height = 1;
        juce::Colour fillColour = juce::Colour (0x4a909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 1050, y = 450, width = 90, height = 24;
        juce::String text (TRANS ("CONTROLS"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 1040, y = 20, width = 110, height = 24;
        juce::String text (TRANS ("FILTER DESIGN"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 1075, y = 490, width = 40, height = 20;
        juce::String text (TRANS ("GAIN"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 10.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        float x = 305.0f, y = 415.0f, width = 190.0f, height = 130.0f;
        juce::Colour fillColour = juce::Colour (0x11b1b1b1);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 0.500f);
    }

    {
        int x = 355, y = 416, width = 90, height = 20;
        juce::String text (TRANS ("SETUP SHORTCUTS"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 10.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 1120, y = 641, width = 23, height = 20;
        juce::String text (TRANS ("dB"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void PluginEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void PluginEditor::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    const double sampleRate = processor.getSampleRate();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == stopbandAmplitude_slider.get())
    {
        //[UserSliderCode_stopbandAmplitude_slider] -- add your slider handling code here..
        stopbandAmplitudedB = sliderThatWasMoved->getValue();
        setTransitionWidthRange();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
        //[/UserSliderCode_stopbandAmplitude_slider]
    }
    else if (sliderThatWasMoved == m1_slider.get())
    {
        //[UserSliderCode_m1_slider] -- add your slider handling code here..
        auto element = processor.getElementState(1);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(1));
        gain_1->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m1_slider]
    }
    else if (sliderThatWasMoved == p1_slider.get())
    {
        //[UserSliderCode_p1_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(1));
        gain_1->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p1_slider]
    }
    else if (sliderThatWasMoved == m2_slider.get())
    {
        //[UserSliderCode_m2_slider] -- add your slider handling code here..
        auto element = processor.getElementState(2);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(2));
        gain_2->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m2_slider]
    }
    else if (sliderThatWasMoved == m3_slider.get())
    {
        //[UserSliderCode_m3_slider] -- add your slider handling code here..
        auto element = processor.getElementState(3);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(3));
        gain_3->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m3_slider]
    }
    else if (sliderThatWasMoved == m4_slider.get())
    {
        //[UserSliderCode_m4_slider] -- add your slider handling code here..
        auto element = processor.getElementState(4);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(4));
        gain_4->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m4_slider]
    }
    else if (sliderThatWasMoved == m5_slider.get())
    {
        //[UserSliderCode_m5_slider] -- add your slider handling code here..
        auto element = processor.getElementState(5);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(5));
        gain_5->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m5_slider]
    }
    else if (sliderThatWasMoved == m6_slider.get())
    {
        //[UserSliderCode_m6_slider] -- add your slider handling code here..
        auto element = processor.getElementState(6);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(6));
        gain_6->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m6_slider]
    }
    else if (sliderThatWasMoved == m7_slider.get())
    {
        //[UserSliderCode_m7_slider] -- add your slider handling code here..
        auto element = processor.getElementState(7);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(7));
        gain_7->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m7_slider]
    }
    else if (sliderThatWasMoved == m8_slider.get())
    {
        //[UserSliderCode_m8_slider] -- add your slider handling code here..
        auto element = processor.getElementState(8);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(8));
        gain_8->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_m8_slider]
    }
    else if (sliderThatWasMoved == p2_slider.get())
    {
        //[UserSliderCode_p2_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(2));
        gain_2->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p2_slider]
    }
    else if (sliderThatWasMoved == p3_slider.get())
    {
        //[UserSliderCode_p3_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(3));
        gain_3->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p3_slider]
    }
    else if (sliderThatWasMoved == p4_slider.get())
    {
        //[UserSliderCode_p4_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(4));
        gain_4->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p4_slider]
    }
    else if (sliderThatWasMoved == p5_slider.get())
    {
        //[UserSliderCode_p5_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(5));
        gain_5->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p5_slider]
    }
    else if (sliderThatWasMoved == p6_slider.get())
    {
        //[UserSliderCode_p6_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);
        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(6));
        gain_6->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p6_slider]
    }
    else if (sliderThatWasMoved == p7_slider.get())
    {
        //[UserSliderCode_p7_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(7));
        gain_7->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p7_slider]
    }
    else if (sliderThatWasMoved == p8_slider.get())
    {
        //[UserSliderCode_p8_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);

        auto dBGain = Decibels::gainToDecibels(processor.getElementGain(8));
        gain_8->setText(juce::String(dBGain, 2) + " dB", NotificationType::sendNotificationAsync);
        //[/UserSliderCode_p8_slider]
    }
    else if (sliderThatWasMoved == frequency_design_slider.get())
    {
        //[UserSliderCode_frequency_design_slider] -- add your slider handling code here..
        design_frequency = updateFrequencyFromDesignSlider(frequency_design_slider.get(), frequency_label.get(), sampleRate);
        if (type_box->getSelectedId() > 1)
            setTransitionWidthRange();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
        //[/UserSliderCode_frequency_design_slider]
    }
    else if (sliderThatWasMoved == transition_width_slider.get())
    {
        //[UserSliderCode_transition_width_slider] -- add your slider handling code here..
        normalisedTransitionWidth = sliderThatWasMoved->getValue();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
        //[/UserSliderCode_transition_width_slider]
    }
    else if (sliderThatWasMoved == passbandAmplitude_slider.get())
    {
        //[UserSliderCode_passbandAmplitude_slider] -- add your slider handling code here..
        passbandAmplitudedB = sliderThatWasMoved->getValue();
        setTransitionWidthRange();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
        //[/UserSliderCode_passbandAmplitude_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void PluginEditor::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == reset_button.get())
    {
        //[UserButtonCode_reset_button] -- add your button handler code here..
        processor.resetFilter();
        //[/UserButtonCode_reset_button]
    }
    else if (buttonThatWasClicked == e1_type.get())
    {
        //[UserButtonCode_e1_type] -- add your button handler code here..
        auto element = processor.getElementState(1);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(1)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m1_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m1_slider->repaint();
        //[/UserButtonCode_e1_type]
    }
    else if (buttonThatWasClicked == e2_type.get())
    {
        //[UserButtonCode_e2_type] -- add your button handler code here..
        auto element = processor.getElementState(2);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(2)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m2_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m2_slider->repaint();
        //[/UserButtonCode_e2_type]
    }
    else if (buttonThatWasClicked == e3_type.get())
    {
        //[UserButtonCode_e3_type] -- add your button handler code here..
        auto element = processor.getElementState(3);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(3)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m3_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m3_slider->repaint();
        //[/UserButtonCode_e3_type]
    }
    else if (buttonThatWasClicked == e4_type.get())
    {
        //[UserButtonCode_e4_type] -- add your button handler code here..
        auto element = processor.getElementState(4);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(4)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m4_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m4_slider->repaint();
        //[/UserButtonCode_e4_type]
    }
    else if (buttonThatWasClicked == e5_type.get())
    {
        //[UserButtonCode_e5_type] -- add your button handler code here..
        auto element = processor.getElementState(5);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(5)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m5_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m5_slider->repaint();
        //[/UserButtonCode_e5_type]
    }
    else if (buttonThatWasClicked == e6_type.get())
    {
        //[UserButtonCode_e6_type] -- add your button handler code here..
        auto element = processor.getElementState(6);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(6)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m6_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m6_slider->repaint();
        //[/UserButtonCode_e6_type]
    }
    else if (buttonThatWasClicked == e7_type.get())
    {
        //[UserButtonCode_e7_type] -- add your button handler code here..
        auto element = processor.getElementState(7);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(7)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m7_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m7_slider->repaint();
        //[/UserButtonCode_e7_type]
    }
    else if (buttonThatWasClicked == e8_type.get())
    {
        //[UserButtonCode_e8_type] -- add your button handler code here..
        auto element = processor.getElementState(8);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(8)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m8_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m8_slider->repaint();
        //[/UserButtonCode_e8_type]
    }
    else if (buttonThatWasClicked == e1_active.get())
    {
        //[UserButtonCode_e1_active] -- add your button handler code here..
        gain_1->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e1_active]
    }
    else if (buttonThatWasClicked == e2_active.get())
    {
        //[UserButtonCode_e2_active] -- add your button handler code here..
        gain_2->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e2_active]
    }
    else if (buttonThatWasClicked == e3_active.get())
    {
        //[UserButtonCode_e3_active] -- add your button handler code here..
        gain_3->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e3_active]
    }
    else if (buttonThatWasClicked == e4_active.get())
    {
        //[UserButtonCode_e4_active] -- add your button handler code here..
        gain_4->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e4_active]
    }
    else if (buttonThatWasClicked == e5_active.get())
    {
        //[UserButtonCode_e5_active] -- add your button handler code here..
        gain_5->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e5_active]
    }
    else if (buttonThatWasClicked == e6_active.get())
    {
        //[UserButtonCode_e6_active] -- add your button handler code here..
        gain_6->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e6_active]
    }
    else if (buttonThatWasClicked == e7_active.get())
    {
        //[UserButtonCode_e7_active] -- add your button handler code here..
        gain_7->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e7_active]
    }
    else if (buttonThatWasClicked == e8_active.get())
    {
        //[UserButtonCode_e8_active] -- add your button handler code here..
        gain_8->setVisible(buttonThatWasClicked->getToggleState());
        //[/UserButtonCode_e8_active]
    }
    else if (buttonThatWasClicked == linLog_switch.get())
    {
        //[UserButtonCode_linLog_switch] -- add your button handler code here..
        linLog = linLog_switch->getToggleState();
        getSpectrum();
        updateReferenceFrequencies();
        frequency_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb);
        phase_response->updateValues(phases, referenceFrequencies, processor.getSampleRate(), true);
        //[/UserButtonCode_linLog_switch]
    }
    else if (buttonThatWasClicked == calculate_button.get())
    {
        //[UserButtonCode_calculate_button] -- add your button handler code here..
        filterDesignAndSetup();
        //[/UserButtonCode_calculate_button]
    }
    else if (buttonThatWasClicked == multiply_phases_button.get())
    {
        //[UserButtonCode_multiply_phases_button] -- add your button handler code here..
        processor.doublePhases();
        //[/UserButtonCode_multiply_phases_button]
    }
    else if (buttonThatWasClicked == divide_phases_button.get())
    {
        //[UserButtonCode_divide_phases_button] -- add your button handler code here..
        processor.halfPhases();
        //[/UserButtonCode_divide_phases_button]
    }
    else if (buttonThatWasClicked == swap_button.get())
    {
        //[UserButtonCode_swap_button] -- add your button handler code here..
        processor.swapPolesAndZeros();
        //[/UserButtonCode_swap_button]
    }
    else if (buttonThatWasClicked == turn_on_button.get())
    {
        //[UserButtonCode_turn_on_button] -- add your button handler code here..
        processor.turnOnOffAllElements(1);
        //[/UserButtonCode_turn_on_button]
    }
    else if (buttonThatWasClicked == turn_off_button.get())
    {
        //[UserButtonCode_turn_off_button] -- add your button handler code here..
        processor.turnOnOffAllElements(0);
        //[/UserButtonCode_turn_off_button]
    }
    else if (buttonThatWasClicked == ampDb_switch.get())
    {
        //[UserButtonCode_ampDb_switch] -- add your button handler code here..
        ampDb = ampDb_switch->getToggleState();
        frequency_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb);
        //[/UserButtonCode_ampDb_switch]
    }
    else if (buttonThatWasClicked == autoUpdate_button.get())
    {
        //[UserButtonCode_autoUpdate_button] -- add your button handler code here..
        if (autoUpdate_button->getToggleState())
        {
            autoUpdate = true;
            autoUpdateCheckAndSetup();
        }
        else
            autoUpdate = false;
        //[/UserButtonCode_autoUpdate_button]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void PluginEditor::labelTextChanged (juce::Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    const double sampleRate = processor.getSampleRate();
    int newFrequency = labelThatHasChanged->getText().getIntValue();

    if (labelThatHasChanged == frequency_label.get())
        formatDesignFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
    else
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == p1_freq.get())
    {
        //[UserLabelCode_p1_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p1_slider.get(), sampleRate);
        //[/UserLabelCode_p1_freq]
    }
    else if (labelThatHasChanged == p2_freq.get())
    {
        //[UserLabelCode_p2_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p2_slider.get(), sampleRate);
        //[/UserLabelCode_p2_freq]
    }
    else if (labelThatHasChanged == p3_freq.get())
    {
        //[UserLabelCode_p3_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p3_slider.get(), sampleRate);
        //[/UserLabelCode_p3_freq]
    }
    else if (labelThatHasChanged == p4_freq.get())
    {
        //[UserLabelCode_p4_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p4_slider.get(), sampleRate);
        //[/UserLabelCode_p4_freq]
    }
    else if (labelThatHasChanged == p5_freq.get())
    {
        //[UserLabelCode_p5_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p5_slider.get(), sampleRate);
        //[/UserLabelCode_p5_freq]
    }
    else if (labelThatHasChanged == p6_freq.get())
    {
        //[UserLabelCode_p6_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p6_slider.get(), sampleRate);
        //[/UserLabelCode_p6_freq]
    }
    else if (labelThatHasChanged == p7_freq.get())
    {
        //[UserLabelCode_p7_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p7_slider.get(), sampleRate);
        //[/UserLabelCode_p7_freq]
    }
    else if (labelThatHasChanged == p8_freq.get())
    {
        //[UserLabelCode_p8_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p8_slider.get(), sampleRate);
        //[/UserLabelCode_p8_freq]
    }
    else if (labelThatHasChanged == frequency_label.get())
    {
        //[UserLabelCode_frequency_label] -- add your label text handling code here..
        design_frequency = updateDesignSliderFromFrequency(newFrequency, frequency_design_slider.get(), sampleRate);
        //[/UserLabelCode_frequency_label]
    }

    //[UserlabelTextChanged_Post]
    //[/UserlabelTextChanged_Post]
}

void PluginEditor::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == type_box.get())
    {
        //[UserComboBoxCode_type_box] -- add your combo box handling code here..
        design_type = comboBoxThatHasChanged->getSelectedId();
        calculate_button->setEnabled(false);
        if (!design_type)
            return;
        if (design_type == 1)
            updateGUIButterworth();
        else
            updateGUIEllipticChebyshevIandII();
        //[/UserComboBoxCode_type_box]
    }
    else if (comboBoxThatHasChanged == shape_box.get())
    {
        //[UserComboBoxCode_shape_box] -- add your combo box handling code here..
        design_shape = comboBoxThatHasChanged->getSelectedId();
        updateGUIGivenShape();
        //[/UserComboBoxCode_shape_box]
    }
    else if (comboBoxThatHasChanged == order_box.get())
    {
        //[UserComboBoxCode_order_box] -- add your combo box handling code here..
        design_filters_to_activate = comboBoxThatHasChanged->getSelectedId();
        if (design_filters_to_activate)
        {
            calculate_button->setEnabled(true);
            if (autoUpdate)
                autoUpdateCheckAndSetup();
        }
        //[/UserComboBoxCode_order_box]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void PluginEditor::getSpectrum()
{
    double phi;
    const auto sampleRate = processor.getSampleRate();

    magnitudes.resize(GRAPHS_QUALITY);
    phases.resize(GRAPHS_QUALITY);

    const double pi = MathConstants<double>::pi;
    const double twoPi = MathConstants<double>::twoPi;

    auto n1 = log(FREQUENCY_FLOOR / sampleRate);
    auto n2 = log(0.5) - n1;
    std::complex<double> spectrum;

    const double gain = processor.getCurrentGain();

    for (int i = 0; i < GRAPHS_QUALITY; ++ i)
    {
        if (linLog)
            phi = static_cast<double>(i) / static_cast<double>(2 * (GRAPHS_QUALITY - 1)); // Linear spectrum
        else
            phi = exp(n1 + (n2 * (static_cast<double>(i) / (static_cast<double>(GRAPHS_QUALITY - 1))))); // Log spectrum

        spectrum = processor.getFilterSpectrum(phi);
        magnitudes[i] = gain * std::abs(spectrum);
        phases[i] = (pi + std::arg(spectrum)) / twoPi;
    }
}

void PluginEditor::updateReferenceFrequencies()
{
    double phi;
    const auto sampleRate = processor.getSampleRate();
    referenceFrequencies.resize(0);

    auto n1 = log(FREQUENCY_FLOOR / sampleRate);
    auto n2 = log(0.5) - log(FREQUENCY_FLOOR / sampleRate);

    for (int i = 0; i < GRAPHS_QUALITY; ++ i)
    {
        if (linLog)
            phi = static_cast<double>(i) / static_cast<double>(2 * (GRAPHS_QUALITY - 1)); // Linear spectrum
        else
            phi = exp(n1 + (n2 * (static_cast<double>(i) / (static_cast<double>(GRAPHS_QUALITY - 1))))); // Log spectrum

        if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
            referenceFrequencies.push_back(phi);
    }
}

void PluginEditor::updateFrequencyFromSlider(juce::Slider* slider, juce::Label* label, double sampleRate)
{
    double sliderValue = slider->getValue();
    int frequency = std::ceil((sliderValue * sampleRate) / 2.0);

    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);
}

void PluginEditor::updateSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate)
{
    double sliderValue = (frequency * 2.0) / sampleRate;
    slider->setValue(sliderValue, juce::sendNotificationSync);
}

double PluginEditor::updateFrequencyFromDesignSlider(juce::Slider* slider, juce::Label* label, double sampleRate)
{
    double sliderValue = slider->getValue();
    int frequency = std::ceil((sliderValue * sampleRate) / 2000.0);
    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);

    return frequency;
}

double PluginEditor::updateDesignSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate)
{
    double sliderValue = (frequency * 2000.0) / sampleRate;
    slider->setValue(sliderValue, juce::sendNotificationSync);

    return std::ceil((slider->getValue() * sampleRate) / 2000.0);
}

void PluginEditor::formatFrequencyInput(int& frequency, juce::Label *label, double sampleRate)
{
    double maxFrequency = sampleRate / 2.0;

    if (frequency < 0)
    {
        frequency = 0;
    }
    else if (frequency > maxFrequency)
    {
        frequency = maxFrequency;
    }
    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);
}

void PluginEditor::formatDesignFrequencyInput(int& frequency, juce::Label *label, double sampleRate)
{
    double maxFrequency = sampleRate / 2.0;

    if (frequency < DESIGN_FREQUENCY_FLOOR)
    {
        frequency = DESIGN_FREQUENCY_FLOOR;
    }
    else if (frequency > maxFrequency)
    {
        frequency = maxFrequency;
    }
    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);
}

void PluginEditor::updateDesignFrequencySlider(short int option)
{
    const double sampleRate = processor.getSampleRate();
    switch (option)
    {
        case 1:
        {
            design_frequency_label->setBounds (1058, 208, 72, 20);
            frequency_design_slider->setBounds (1027, 230, 83, 25);
            frequency_design_slider->setRange (0.0001, 999, 0.0001);
            updateDesignSliderFromFrequency(design_frequency, frequency_design_slider.get(), sampleRate);
            frequency_label->setBounds (1110, 230, 60, 25);
        } break;

        case 2:
        {
            design_frequency_label->setBounds (1058, 148, 72, 20);
            frequency_design_slider->setBounds (1027, 170, 83, 25);
            auto minValue = std::ceil(0.00005 * 2000.0);
            auto maxValue = std::floor(0.499945 * 2000.0);
            frequency_design_slider->setRange(minValue, maxValue);

            auto sliderValue = (design_frequency * 2000.0) / sampleRate;

            if (minValue <= sliderValue && sliderValue <= maxValue)
                frequency_design_slider->setValue(sliderValue);
            else
            {
                frequency_design_slider->setValue(2000.0 * 0.25);
                design_frequency = processor.getSampleRate() * 0.25;
            }
        } break;
    }
}

void PluginEditor::setTransitionWidthRange ()
{
    double sampleRate = processor.getSampleRate();
    double normalisedFrequency = design_frequency / sampleRate;
    double minValue = 0.0001;
    double maxValue = jmin(2 * normalisedFrequency - minValue, 2 * (0.5 - normalisedFrequency) - minValue);
    const double interval = 0.00001;
    const int maxOrder = 8;

    auto Gp = Decibels::decibelsToGain (passbandAmplitudedB, -300.0);
    auto Gs = Decibels::decibelsToGain (stopbandAmplitudedB, -300.0);

    double X = acosh( sqrt(1 / (Gs * Gs) - 1.0)  /  sqrt(1 / (Gp * Gp) - 1.0));

    double Y = acosh( tan(MathConstants<double>::pi * (normalisedFrequency + minValue / 2)) / tan(MathConstants<double>::pi * (normalisedFrequency - minValue / 2)));

    while (roundToInt(std::ceil(X / Y)) > maxOrder)
    {
        minValue += interval;
        Y = acosh( tan(MathConstants<double>::pi * (normalisedFrequency + minValue / 2)) / tan(MathConstants<double>::pi * (normalisedFrequency - minValue / 2)));
    }

    transition_width_slider->setRange(minValue, maxValue);
    if (transition_width_slider->getValue() < minValue)
        transition_width_slider->setValue(minValue);
    normalisedTransitionWidth = transition_width_slider->getValue();
}

void PluginEditor::updateGUIGivenShape()
{
    if (!type_box->getSelectedId())
    {
        int i = 1;
        for (juce::String& type : selectable_filter_types)
        {
            type_box->addItem(type, i);
            ++ i;
        }
    }
    autoUpdateCheckAndSetup();
}

void PluginEditor::updateGUIButterworth()
{
    calculate_button->setEnabled(false);

    order_box->clear();
    order_box->setVisible(true);

    transition_width_label->setVisible(false);
    transition_width_slider->setVisible(false);

    passbandAmplitude_label->setVisible(false);
    passbandAmplitude_slider->setVisible(false);

    stopbandAmplitude_label->setVisible(false);
    stopbandAmplitude_slider->setVisible(false);

    updateDesignFrequencySlider(1);

    for (juce::String order : SELECTABLE_ORDERS_BUTTERWORTH)
    {
        int int_order = order.getIntValue();
        int attenuation = 6 * int_order;
        juce::String stringToVisualize = order + "  (-" + juce::String(attenuation) + " db / octave";
        order_box->addItem(stringToVisualize, int_order);
    }
}

void PluginEditor::updateGUIEllipticChebyshevIandII()
{
    order_box->setVisible(false);

    updateDesignFrequencySlider(2);

    passbandAmplitude_slider->setValue(passbandAmplitudedB);
    stopbandAmplitude_slider->setValue(stopbandAmplitudedB);
    setTransitionWidthRange();

    frequency_label->setBounds (1110, 170, 60, 25);

    passbandAmplitude_label->setVisible(true);
    passbandAmplitude_label->setBounds (1020, 195, 159, 20);

    passbandAmplitude_slider->setVisible(true);
    passbandAmplitude_slider->setBounds (1027, 219, 135, 25);

    stopbandAmplitude_label->setVisible(true);
    stopbandAmplitude_label->setBounds(1022, 250, 159, 20);

    stopbandAmplitude_slider->setVisible(true);
    stopbandAmplitude_slider->setBounds(1027, 274, 135, 25);

    transition_width_label->setVisible(true);
    transition_width_label->setBounds(1030, 305, 129, 20);

    transition_width_slider->setVisible(true);
    transition_width_slider->setBounds (1027, 329, 135, 25);

    calculate_button->setEnabled(true);
}

void PluginEditor::coefficientsNormalization (double& c0, double& c1, double& c2)
{
    c1 /= c0;
    c2 /= c0;
    c0 = 1.0;
}

void PluginEditor::fromCoefficientsToMagnitudeAndPhase (double& mg, double& ph, double c1, double c2)
{
    mg = sqrt(c2);
    ph = (1 / MathConstants<double>::pi) * acos(-c1 / (2 * mg));
}

void PluginEditor::filterDesignAndSetup()
{
    const double sampleRate = processor.getSampleRate();
    const int order = design_filters_to_activate;

    juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<double>> iirCoefficients;

    double cutoff = design_frequency;

    if (design_shape == 2)
        cutoff = sampleRate * 0.5 - design_frequency;

    switch (design_type)
    {
        case 1:
        {
            iirCoefficients = juce::dsp::FilterDesign<double>::designIIRLowpassHighOrderButterworthMethod(cutoff, sampleRate, order);
        } break;

        case 2:
        {
            iirCoefficients = juce::dsp::FilterDesign<double>::designIIRLowpassHighOrderChebyshev1Method(cutoff, sampleRate, normalisedTransitionWidth, passbandAmplitudedB, stopbandAmplitudedB);
        } break;

        case 3:
        {
            iirCoefficients = juce::dsp::FilterDesign<double>::designIIRLowpassHighOrderChebyshev2Method(cutoff, sampleRate, normalisedTransitionWidth, passbandAmplitudedB, stopbandAmplitudedB);
        } break;

        case 4:
        {
            iirCoefficients = juce::dsp::FilterDesign<double>::designIIRLowpassHighOrderEllipticMethod(cutoff, sampleRate, normalisedTransitionWidth, passbandAmplitudedB, stopbandAmplitudedB);
        } break;
    }

    double b0, b1, b2, a1, a2;
    double magnitude, phase;
    int elementNr = 1;

    processor.setBypass(true);

    for (int i = 0; i < iirCoefficients.size(); ++ i)
    {
        const auto& coeffs = iirCoefficients[i];

        if (coeffs->coefficients.size() == 3)
        {
            std::complex<double> zero;
            std::complex<double> pole;

            if (design_shape == 2)
            {
                zero = - (- coeffs->coefficients[1]) / coeffs->coefficients[0];
                pole = - (- coeffs->coefficients[2]);
            }
            else
            {
                zero = - coeffs->coefficients[1] / coeffs->coefficients[0];
                pole = - coeffs->coefficients[2];
            }

            processor.setFilter(std::abs(zero), std::arg(zero), FilterElement::ZERO, elementNr);
            ++ elementNr;

            processor.setFilter(std::abs(pole), std::arg(pole), FilterElement::POLE, elementNr);
            ++ elementNr;
        }
        else
        {
            b0 = coeffs->coefficients[0];
            b1 = coeffs->coefficients[1];
            b2 = coeffs->coefficients[2];

            a1 = coeffs->coefficients[3];
            a2 = coeffs->coefficients[4];

            if (design_shape == 2)
            {
                b1 = - b1;
                a1 = - a1;
            }

            coefficientsNormalization(b0, b1, b2);

            fromCoefficientsToMagnitudeAndPhase(magnitude, phase, b1, b2);
            std::complex<double> zero = std::polar(magnitude, phase);
            processor.setFilter(std::abs(zero), std::arg(zero), FilterElement::ZERO, elementNr);
            ++ elementNr;

            fromCoefficientsToMagnitudeAndPhase(magnitude, phase, a1, a2);
            std::complex<double> pole = std::polar(magnitude, phase);
            processor.setFilter(std::abs(pole), std::arg(pole), FilterElement::POLE, elementNr);
            ++ elementNr;
        }
    }

    for (; elementNr <= NUMBER_OF_FILTER_ELEMENTS; ++ elementNr)
            processor.setUnactive(elementNr);
    processor.setBypass(false);
}

void PluginEditor::autoUpdateCheckAndSetup ()
{
    if (design_type == 1)
        if (order_box->getSelectedId() != 0)
            filterDesignAndSetup();
        else
            return;
    else
        filterDesignAndSetup();
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="PluginEditor" componentName=""
                 parentClasses="public juce::AudioProcessorEditor" constructorParams="PolesAndZerosEQAudioProcessor&amp; p, AudioProcessorValueTreeState&amp; vts"
                 variableInitialisers="AudioProcessorEditor(&amp;p), processor (p), valueTreeState (vts)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="1200" initialHeight="750">
  <BACKGROUND backgroundColour="ffecf0f1">
    <ROUNDRECT pos="15 15 510 720" cornerSize="14.5" fill="solid: 17b1b1b1"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <ROUNDRECT pos="1005 15 180 720" cornerSize="14.5" fill="solid: 19656565"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <ROUNDRECT pos="525 15 480 720" cornerSize="14.5" fill="solid: 17b1b1b1"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <TEXT pos="1097 708 90 30" fill="solid: ff333333" hasStroke="0" text="TOOLS"
          fontname="Gill Sans" fontsize="16.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <RECT pos="15 90 510 1" fill="solid: 25909497" hasStroke="0"/>
    <TEXT pos="395 708 116 30" fill="solid: ff333333" hasStroke="0" text="FILTER SETUP"
          fontname="Gill Sans" fontsize="16.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="810 708 186 30" fill="solid: ff333333" hasStroke="0" text="FILTER RESPONSE"
          fontname="Gill Sans" fontsize="16.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <RECT pos="15 135 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 180 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 225 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 270 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 360 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 315 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="1005 445 180 1" fill="solid: 4a909497" hasStroke="0"/>
    <TEXT pos="1050 450 90 24" fill="solid: ff333333" hasStroke="0" text="CONTROLS"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1040 20 110 24" fill="solid: ff333333" hasStroke="0" text="FILTER DESIGN"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1075 490 40 20" fill="solid: ff333333" hasStroke="0" text="GAIN"
          fontname="Gill Sans" fontsize="10.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <ROUNDRECT pos="305 415 190 130" cornerSize="14.5" fill="solid: 11b1b1b1"
               hasStroke="1" stroke="0.5, mitered, butt" strokeColour="solid: ff383838"/>
    <TEXT pos="355 416 90 20" fill="solid: ff333333" hasStroke="0" text="SETUP SHORTCUTS"
          fontname="Gill Sans" fontsize="10.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1120 641 23 20" fill="solid: ff333333" hasStroke="0" text="dB"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
  </BACKGROUND>
  <LABEL name="Passband Amplitude" id="a57ca268c3802211" memberName="passbandAmplitude_label"
         virtualName="" explicitFocusOrder="0" pos="1020 320 159 20" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="PASSBAND AMPLITUDE (DB)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <SLIDER name="Stopband Amplitude" id="308fcfe400b70b27" memberName="stopbandAmplitude_slider"
          virtualName="" explicitFocusOrder="0" pos="1022 326 135 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="-35.0" max="-21.0" int="0.1" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <GENERICCOMPONENT name="gaussianPlane" id="f84485816497c4e3" memberName="gaussian_plane"
                    virtualName="" explicitFocusOrder="0" pos="30 415 260 260" class="GaussianPlane"
                    params="processor.getFilterElementsChain()"/>
  <TEXTBUTTON name="Reset" id="2581837dc85daae9" memberName="reset_button"
              virtualName="" explicitFocusOrder="0" pos="360 568 70 30" bgColOff="ff909497"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <GENERICCOMPONENT name="frequencyResponse" id="161cb81e63dc8e46" memberName="frequency_response"
                    virtualName="" explicitFocusOrder="0" pos="550 35 430 285" class="FrequencyResponse"
                    params="magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb"/>
  <LABEL name="Frequency response" id="4c8fffb65e845bfc" memberName="freq_response_label"
         virtualName="" explicitFocusOrder="0" pos="695 325 140 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="SPECTRUM MAGNITUDE&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <GENERICCOMPONENT name="phaseResponse" id="c9a48273dec25832" memberName="phase_response"
                    virtualName="" explicitFocusOrder="0" pos="550 390 430 285" class="PhaseResponse"
                    params="phases, referenceFrequencies, processor.getSampleRate(), ampDb"/>
  <LABEL name="Phase response" id="6d08c4e421703ed5" memberName="ph_response_label"
         virtualName="" explicitFocusOrder="0" pos="705 680 110 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="SPECTRUM PHASE"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <SLIDER name="Element 1 magnitude" id="9107cda7959bc0ad" memberName="m1_slider"
          virtualName="" explicitFocusOrder="0" pos="20 55 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 1 phase" id="b9f95ed5c32caef9" memberName="p1_slider"
          virtualName="" explicitFocusOrder="0" pos="144 55 120 25" bkgcol="ff383838"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Magnitudes" id="f2206057c7f672c6" memberName="magnitudes_label"
         virtualName="" explicitFocusOrder="0" pos="33 20 90 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="MAGNITUDE&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <LABEL name="Phases" id="e0e34f187bbbdcdc" memberName="phases_label"
         virtualName="" explicitFocusOrder="0" pos="198 20 80 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="FREQUENCY&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <LABEL name="Filter Type" id="676532893526fbc7" memberName="zero_pole_label"
         virtualName="" explicitFocusOrder="0" pos="317 20 91 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="FILTER TYPE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <LABEL name="Active" id="e82d0bcbcd7af0" memberName="active_label" virtualName=""
         explicitFocusOrder="0" pos="408 20 55 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="ACTIVE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Element 1 type" id="cb026ef1068db639" memberName="e1_type"
                virtualName="" explicitFocusOrder="0" pos="338 56 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 2 type" id="56addf679899dfb" memberName="e2_type"
                virtualName="" explicitFocusOrder="0" pos="338 101 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 3 type" id="5abda647a887dc8e" memberName="e3_type"
                virtualName="" explicitFocusOrder="0" pos="338 146 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 4 type" id="3136ab6a98246edb" memberName="e4_type"
                virtualName="" explicitFocusOrder="0" pos="338 191 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 5 type" id="4d6d7aa8e46a31c6" memberName="e5_type"
                virtualName="" explicitFocusOrder="0" pos="338 236 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 6 type" id="6a190298ad04abf" memberName="e6_type"
                virtualName="" explicitFocusOrder="0" pos="338 281 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 7 type" id="57fd1da9881e7a6a" memberName="e7_type"
                virtualName="" explicitFocusOrder="0" pos="338 326 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 8 type" id="a0b1086b7f717581" memberName="e8_type"
                virtualName="" explicitFocusOrder="0" pos="338 371 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 1 active" id="84bb86bdbd9ccede" memberName="e1_active"
                virtualName="" explicitFocusOrder="0" pos="411 56 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 2 active" id="176e3fb309f12ef5" memberName="e2_active"
                virtualName="" explicitFocusOrder="0" pos="411 101 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 3 active" id="d6d9c0542e41b9c3" memberName="e3_active"
                virtualName="" explicitFocusOrder="0" pos="411 146 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 4 active" id="72d6ebba0023e28e" memberName="e4_active"
                virtualName="" explicitFocusOrder="0" pos="411 191 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 5 active" id="d8c38a2ce324a73d" memberName="e5_active"
                virtualName="" explicitFocusOrder="0" pos="411 236 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 6 active" id="1b42bda33e1c947c" memberName="e6_active"
                virtualName="" explicitFocusOrder="0" pos="411 281 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 7 active" id="3405e0694e2b063d" memberName="e7_active"
                virtualName="" explicitFocusOrder="0" pos="411 326 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 8 active" id="b60e0b9c2104a2bd" memberName="e8_active"
                virtualName="" explicitFocusOrder="0" pos="411 371 49 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="Gaussian plane" id="32980deccea32218" memberName="gaussian_plane_label"
         virtualName="" explicitFocusOrder="0" pos="95 680 140 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="GAUSSIAN PLANE"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Bypass" id="4daf2a36bc47407c" memberName="bypass" virtualName=""
                explicitFocusOrder="0" pos="1067 672 60 25" buttonText="" connectedEdges="0"
                needsCallback="0" radioGroupId="0" state="0"/>
  <SLIDER name="Gain" id="7e880f1fc774c2af" memberName="gain_slider" virtualName=""
          explicitFocusOrder="0" pos="1074 510 45 150" thumbcol="ffffffff"
          textboxtext="ff383838" textboxhighlight="66686868" textboxoutline="8e989b"
          min="0.0" max="10.0" int="0.0" style="LinearVertical" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="100" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <TOGGLEBUTTON name="Linear / Logarithmic" id="269b07022e58d862" memberName="linLog_switch"
                virtualName="" explicitFocusOrder="0" pos="931 327 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="Element 2 magnitude" id="e1a6d1b02cc8ab80" memberName="m2_slider"
          virtualName="" explicitFocusOrder="0" pos="20 100 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 3 magnitude" id="b0094bd01977f340" memberName="m3_slider"
          virtualName="" explicitFocusOrder="0" pos="20 145 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 4 magnitude" id="8f433acd768ec1d9" memberName="m4_slider"
          virtualName="" explicitFocusOrder="0" pos="20 190 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 5 magnitude" id="fe0864ae3596b10d" memberName="m5_slider"
          virtualName="" explicitFocusOrder="0" pos="20 235 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 6 magnitude" id="e6371565d3e4bb47" memberName="m6_slider"
          virtualName="" explicitFocusOrder="0" pos="20 280 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 7 magnitude" id="3661ca4111317400" memberName="m7_slider"
          virtualName="" explicitFocusOrder="0" pos="20 325 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 8 magnitude" id="8e83a0a6b9d1e264" memberName="m8_slider"
          virtualName="" explicitFocusOrder="0" pos="20 370 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 2 phase" id="60007c00fdd0e234" memberName="p2_slider"
          virtualName="" explicitFocusOrder="0" pos="144 100 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 3 phase" id="dad16f547c32eed" memberName="p3_slider"
          virtualName="" explicitFocusOrder="0" pos="144 145 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 4 phase" id="2baa73d9d72e7d2a" memberName="p4_slider"
          virtualName="" explicitFocusOrder="0" pos="144 190 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 5 phase" id="6ba360671cfe4646" memberName="p5_slider"
          virtualName="" explicitFocusOrder="0" pos="144 235 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 6 phase" id="6fb200ac73072a45" memberName="p6_slider"
          virtualName="" explicitFocusOrder="0" pos="144 280 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 7 phase" id="481e9546dc440bf4" memberName="p7_slider"
          virtualName="" explicitFocusOrder="0" pos="144 325 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 8 phase" id="d587f0ce08864b4e" memberName="p8_slider"
          virtualName="" explicitFocusOrder="0" pos="144 370 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Element 1 frequency" id="79f5387b07d9c483" memberName="p1_freq"
         virtualName="" explicitFocusOrder="0" pos="270 55 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 2 frequency" id="5b5fee371d8a9052" memberName="p2_freq"
         virtualName="" explicitFocusOrder="0" pos="270 100 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 3 frequency" id="a15e9da2c1f0e36b" memberName="p3_freq"
         virtualName="" explicitFocusOrder="0" pos="270 145 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 4 frequency" id="d6a7e59b3d26f91e" memberName="p4_freq"
         virtualName="" explicitFocusOrder="0" pos="270 190 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 5 frequency" id="f0a2b500cc7bc03" memberName="p5_freq"
         virtualName="" explicitFocusOrder="0" pos="270 235 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 6 frequency" id="5dfdb03e7a654b13" memberName="p6_freq"
         virtualName="" explicitFocusOrder="0" pos="270 280 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 7 frequency" id="600a01c1726873b3" memberName="p7_freq"
         virtualName="" explicitFocusOrder="0" pos="270 325 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Element 8 frequency" id="f85a4dea8bdf47f1" memberName="p8_freq"
         virtualName="" explicitFocusOrder="0" pos="270 370 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="Design type" id="5a16af79e3a09d2b" memberName="type_box"
            virtualName="" explicitFocusOrder="0" pos="1025 110 140 25" editable="0"
            layout="33" items="" textWhenNonSelected="TYPE" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Design shape" id="fab26cb579c24549" memberName="shape_box"
            virtualName="" explicitFocusOrder="0" pos="1025 65 140 25" editable="0"
            layout="33" items="LOWPASS&#10;HIGHPASS" textWhenNonSelected="SHAPE"
            textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="Calculate" id="6b0929d790004858" memberName="calculate_button"
              virtualName="" explicitFocusOrder="0" pos="1098 390 80 30" bgColOff="ff909497"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Multiply phases" id="fd9508a1dc4c09ae" memberName="multiply_phases_button"
              virtualName="" explicitFocusOrder="0" pos="319 477 72 22" bgColOff="ff909497"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Divide phases" id="a62537b9345044f3" memberName="divide_phases_button"
              virtualName="" explicitFocusOrder="0" pos="409 477 72 22" bgColOff="ff909497"
              bgColOn="ff727272" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Swap poles/zeros" id="938b23da9ff326b0" memberName="swap_button"
              virtualName="" explicitFocusOrder="0" pos="359 512 82 22" bgColOff="ff909497"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Turn on all the elements" id="e4b8377926241c51" memberName="turn_on_button"
              virtualName="" explicitFocusOrder="0" pos="319 442 72 22" bgColOff="ff73cc81"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Turn off all the elements" id="f6c36c1c53a4a06d" memberName="turn_off_button"
              virtualName="" explicitFocusOrder="0" pos="409 442 72 22" bgColOff="ffe86d5c"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <COMBOBOX name="Design order" id="a7c23e76d01914d5" memberName="order_box"
            virtualName="" explicitFocusOrder="0" pos="1025 155 140 25" editable="0"
            layout="33" items="" textWhenNonSelected="ORDER" textWhenNoItems="(no choices)"/>
  <LABEL name="Design frequency" id="bc37557b2c8cc2ce" memberName="design_frequency_label"
         virtualName="" explicitFocusOrder="0" pos="1063 193 72 20" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="FREQUENCY&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Amplitude / dB" id="b9198764b9daa498" memberName="ampDb_switch"
                virtualName="" explicitFocusOrder="0" pos="546 327 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="Frequency design slider" id="805d44cec628c3" memberName="frequency_design_slider"
          virtualName="" explicitFocusOrder="0" pos="1022 215 83 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0001" max="1000.0" int="0.0001" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Frequency Label" id="bb3cf5d37607187f" memberName="frequency_label"
         virtualName="" explicitFocusOrder="0" pos="1110 215 60 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="1"
         editableDoubleClick="1" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Transition width" id="b5068a6d459942e" memberName="transition_width_label"
         virtualName="" explicitFocusOrder="0" pos="1031 256 129 20" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="TRANSITION WIDTH"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <SLIDER name="Transition width slider" id="a9239ce032310b32" memberName="transition_width_slider"
          virtualName="" explicitFocusOrder="0" pos="1022 280 135 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.001" max="0.5" int="1.0e-5" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Passband Amplitude" id="ef6299d4c0c90b49" memberName="passbandAmplitude_slider"
          virtualName="" explicitFocusOrder="0" pos="1022 344 135 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="-10.0" max="-0.1" int="0.1" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Stopband Amplitude" id="586783be637d3c53" memberName="stopbandAmplitude_label"
         virtualName="" explicitFocusOrder="0" pos="1020 302 159 20" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="STOPBAND AMPLITUDE (DB)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Auto update" id="8d6ebbb17b01b41e" memberName="autoUpdate_button"
                virtualName="" explicitFocusOrder="0" pos="1013 390 80 30" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="Gain 1 label" id="d2af3168bb3295ad" memberName="gain_1"
         virtualName="" explicitFocusOrder="0" pos="470 55 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="a88f42871a57d930" memberName="gain_2"
         virtualName="" explicitFocusOrder="0" pos="470 100 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="b82703d20aa7b910" memberName="gain_3"
         virtualName="" explicitFocusOrder="0" pos="470 145 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="b879fa1c09d8070c" memberName="gain_4"
         virtualName="" explicitFocusOrder="0" pos="470 190 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="c3065c046eecdd9f" memberName="gain_5"
         virtualName="" explicitFocusOrder="0" pos="470 235 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="2b04579391c20b6b" memberName="gain_6"
         virtualName="" explicitFocusOrder="0" pos="470 280 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="3d0555b13f6503a2" memberName="gain_7"
         virtualName="" explicitFocusOrder="0" pos="470 325 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Gain 2 label" id="967f80d9a172f0a7" memberName="gain_8"
         virtualName="" explicitFocusOrder="0" pos="470 370 45 25" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="ff000000" labelText="" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="12.0" kerning="0.0" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

