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

#define GRAPHS_QUALITY                          2048
#define FREQUENCY_FLOOR                         10.0
#define QUALITY_FLOOR                           0.1
#define QUALITY_CEILING                         100.0
#define DESIGN_GAIN_FLOOR                       -24.0
#define DESIGN_GAIN_CEILING                     24.0
#define NUMBER_OF_REFERENCE_FREQUENCIES         8
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
        frequency_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate());
        phase_response->updateValues(phases, referenceFrequencies, processor.getSampleRate());
        gaussian_plane->updateElements(processor.getFilterElementsChain());
    });

    getSpectrum();
    updateReferenceFrequencies();

    magnitudesAttachments.reserve(NUMBER_OF_FILTER_ELEMENTS);
    phasesAttachments.reserve(NUMBER_OF_FILTER_ELEMENTS);
    typesAttachments.reserve(NUMBER_OF_FILTER_ELEMENTS);
    activeAttachments.reserve(NUMBER_OF_FILTER_ELEMENTS);
    //[/Constructor_pre]

    reset_button.reset (new juce::TextButton ("Reset"));
    addAndMakeVisible (reset_button.get());
    reset_button->setButtonText (juce::String());
    reset_button->addListener (this);
    reset_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff505050));
    reset_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    reset_button->setBounds (1010, 520, 70, 30);

    frequency_response.reset (new GraphicResponse (magnitudes, referenceFrequencies, processor.getSampleRate()));
    addAndMakeVisible (frequency_response.get());
    frequency_response->setName ("frequencyResponse");

    frequency_response->setBounds (520, 35, 430, 270);

    freq_response_label.reset (new juce::Label ("Frequency response",
                                                TRANS ("SPECTRUM MAGNITUDE\n")));
    addAndMakeVisible (freq_response_label.get());
    freq_response_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    freq_response_label->setJustificationType (juce::Justification::centred);
    freq_response_label->setEditable (false, false, false);
    freq_response_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    freq_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freq_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    freq_response_label->setBounds (665, 310, 140, 24);

    phase_response.reset (new GraphicResponse (phases, referenceFrequencies, processor.getSampleRate()));
    addAndMakeVisible (phase_response.get());
    phase_response->setName ("phaseResponse");

    phase_response->setBounds (540, 415, 390, 260);

    ph_response_label.reset (new juce::Label ("Phase response",
                                              TRANS ("SPECTRUM PHASE")));
    addAndMakeVisible (ph_response_label.get());
    ph_response_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    ph_response_label->setJustificationType (juce::Justification::centred);
    ph_response_label->setEditable (false, false, false);
    ph_response_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    ph_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    ph_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    ph_response_label->setBounds (675, 680, 110, 24);

    gaussian_plane.reset (new GaussianPlane (processor.getFilterElementsChain()));
    addAndMakeVisible (gaussian_plane.get());
    gaussian_plane->setName ("gaussianPlane");

    gaussian_plane->setBounds (35, 415, 260, 260);

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
                                         TRANS ("PHASE\n")));
    addAndMakeVisible (phases_label.get());
    phases_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    phases_label->setJustificationType (juce::Justification::centred);
    phases_label->setEditable (false, false, false);
    phases_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    phases_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    phases_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    phases_label->setBounds (197, 20, 62, 24);

    zero_pole_label.reset (new juce::Label ("Zero / Pole",
                                            TRANS ("ZERO  /  POLE")));
    addAndMakeVisible (zero_pole_label.get());
    zero_pole_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    zero_pole_label->setJustificationType (juce::Justification::centred);
    zero_pole_label->setEditable (false, false, false);
    zero_pole_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    zero_pole_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    zero_pole_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    zero_pole_label->setBounds (322, 20, 91, 24);

    active_label.reset (new juce::Label ("Active",
                                         TRANS ("ACTIVE")));
    addAndMakeVisible (active_label.get());
    active_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    active_label->setJustificationType (juce::Justification::centred);
    active_label->setEditable (false, false, false);
    active_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    active_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    active_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    active_label->setBounds (423, 20, 55, 24);

    e1_type.reset (new juce::ToggleButton ("Element 1 type"));
    addAndMakeVisible (e1_type.get());
    e1_type->setButtonText (juce::String());

    e1_type->setBounds (343, 56, 52, 21);

    e2_type.reset (new juce::ToggleButton ("Element 2 type"));
    addAndMakeVisible (e2_type.get());
    e2_type->setButtonText (juce::String());

    e2_type->setBounds (343, 101, 52, 21);

    e3_type.reset (new juce::ToggleButton ("Element 3 type"));
    addAndMakeVisible (e3_type.get());
    e3_type->setButtonText (juce::String());

    e3_type->setBounds (343, 146, 52, 21);

    e4_type.reset (new juce::ToggleButton ("Element 4 type"));
    addAndMakeVisible (e4_type.get());
    e4_type->setButtonText (juce::String());

    e4_type->setBounds (343, 191, 52, 21);

    e5_type.reset (new juce::ToggleButton ("Element 5 type"));
    addAndMakeVisible (e5_type.get());
    e5_type->setButtonText (juce::String());

    e5_type->setBounds (343, 236, 52, 21);

    e6_type.reset (new juce::ToggleButton ("Element 6 type"));
    addAndMakeVisible (e6_type.get());
    e6_type->setButtonText (juce::String());

    e6_type->setBounds (343, 281, 52, 21);

    e7_type.reset (new juce::ToggleButton ("Element 7 type"));
    addAndMakeVisible (e7_type.get());
    e7_type->setButtonText (juce::String());

    e7_type->setBounds (343, 326, 52, 21);

    e8_type.reset (new juce::ToggleButton ("Element 8 type"));
    addAndMakeVisible (e8_type.get());
    e8_type->setButtonText (juce::String());

    e8_type->setBounds (343, 371, 52, 21);

    e1_active.reset (new juce::ToggleButton ("Element 1 active"));
    addAndMakeVisible (e1_active.get());
    e1_active->setButtonText (juce::String());
    e1_active->addListener (this);

    e1_active->setBounds (420, 56, 35, 21);

    e2_active.reset (new juce::ToggleButton ("Element 2 active"));
    addAndMakeVisible (e2_active.get());
    e2_active->setButtonText (juce::String());
    e2_active->addListener (this);

    e2_active->setBounds (420, 101, 35, 21);

    e3_active.reset (new juce::ToggleButton ("Element 3 active"));
    addAndMakeVisible (e3_active.get());
    e3_active->setButtonText (juce::String());
    e3_active->addListener (this);

    e3_active->setBounds (420, 146, 35, 21);

    e4_active.reset (new juce::ToggleButton ("Element 4 active"));
    addAndMakeVisible (e4_active.get());
    e4_active->setButtonText (juce::String());
    e4_active->addListener (this);

    e4_active->setBounds (420, 191, 35, 21);

    e5_active.reset (new juce::ToggleButton ("Element 5 active"));
    addAndMakeVisible (e5_active.get());
    e5_active->setButtonText (juce::String());
    e5_active->addListener (this);

    e5_active->setBounds (420, 236, 35, 21);

    e6_active.reset (new juce::ToggleButton ("Element 6 active"));
    addAndMakeVisible (e6_active.get());
    e6_active->setButtonText (juce::String());
    e6_active->addListener (this);

    e6_active->setBounds (420, 281, 35, 21);

    e7_active.reset (new juce::ToggleButton ("Element 7 active"));
    addAndMakeVisible (e7_active.get());
    e7_active->setButtonText (juce::String());
    e7_active->addListener (this);

    e7_active->setBounds (420, 326, 35, 21);

    e8_active.reset (new juce::ToggleButton ("Element 8 active"));
    addAndMakeVisible (e8_active.get());
    e8_active->setButtonText (juce::String());
    e8_active->addListener (this);

    e8_active->setBounds (420, 371, 35, 21);

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

    e1_led.reset (new LEDComponent (*e1_active));
    addAndMakeVisible (e1_led.get());
    e1_led->setName ("Led 1");

    e1_led->setBounds (465, 59, 17, 17);

    e2_led.reset (new LEDComponent (*e2_active));
    addAndMakeVisible (e2_led.get());
    e2_led->setName ("Led 2");

    e2_led->setBounds (465, 104, 17, 17);

    e3_led.reset (new LEDComponent (*e3_active));
    addAndMakeVisible (e3_led.get());
    e3_led->setName ("Led 3");

    e3_led->setBounds (465, 149, 17, 17);

    e4_led.reset (new LEDComponent (*e4_active));
    addAndMakeVisible (e4_led.get());
    e4_led->setName ("Led 4");

    e4_led->setBounds (465, 194, 17, 17);

    e5_led.reset (new LEDComponent (*e5_active));
    addAndMakeVisible (e5_led.get());
    e5_led->setName ("Led 5");

    e5_led->setBounds (465, 239, 17, 17);

    e6_led.reset (new LEDComponent (*e6_active
                                    ));
    addAndMakeVisible (e6_led.get());
    e6_led->setName ("Led 6");

    e6_led->setBounds (465, 284, 17, 17);

    e7_led.reset (new LEDComponent (*e7_active));
    addAndMakeVisible (e7_led.get());
    e7_led->setName ("Led 7");

    e7_led->setBounds (465, 329, 17, 17);

    e8_led.reset (new LEDComponent (*e8_active));
    addAndMakeVisible (e8_led.get());
    e8_led->setName ("Led 8");

    e8_led->setBounds (465, 374, 17, 17);

    bypass.reset (new juce::ToggleButton ("Bypass"));
    addAndMakeVisible (bypass.get());
    bypass->setButtonText (juce::String());

    bypass->setBounds (1010, 600, 70, 30);

    gain_slider.reset (new juce::Slider ("Gain"));
    addAndMakeVisible (gain_slider.get());
    gain_slider->setRange (0, 10, 0);
    gain_slider->setSliderStyle (juce::Slider::LinearVertical);
    gain_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    gain_slider->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    gain_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    gain_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    gain_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x008e989b));

    gain_slider->setBounds (1115, 495, 45, 180);

    linLog_switch.reset (new juce::ToggleButton ("Linear / Logarithmic"));
    addAndMakeVisible (linLog_switch.get());
    linLog_switch->setButtonText (juce::String());
    linLog_switch->addListener (this);

    linLog_switch->setBounds (901, 312, 52, 21);

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

    type_box.reset (new juce::ComboBox ("Type"));
    addAndMakeVisible (type_box.get());
    type_box->setEditableText (false);
    type_box->setJustificationType (juce::Justification::centredLeft);
    type_box->setTextWhenNothingSelected (TRANS ("-"));
    type_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    type_box->addItem (TRANS ("butterworth"), 1);
    type_box->addItem (TRANS ("chebyshev"), 2);
    type_box->addItem (TRANS ("elliptic"), 3);
    type_box->addItem (TRANS ("bessel"), 4);
    type_box->addItem (TRANS ("biquad"), 5);
    type_box->addItem (TRANS ("comb"), 6);
    type_box->addListener (this);

    type_box->setBounds (1010, 65, 140, 25);

    shape_box.reset (new juce::ComboBox ("Shape"));
    addAndMakeVisible (shape_box.get());
    shape_box->setEditableText (false);
    shape_box->setJustificationType (juce::Justification::centredLeft);
    shape_box->setTextWhenNothingSelected (TRANS ("-"));
    shape_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    shape_box->addItem (TRANS ("lowpass"), 1);
    shape_box->addItem (TRANS ("highpass"), 2);
    shape_box->addItem (TRANS ("bandpass"), 3);
    shape_box->addItem (TRANS ("bandstop"), 4);
    shape_box->addListener (this);

    shape_box->setBounds (1010, 120, 140, 25);

    frequency_label.reset (new juce::Label ("Frequency",
                                            juce::String()));
    addAndMakeVisible (frequency_label.get());
    frequency_label->setFont (juce::Font ("Gill Sans", 15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    frequency_label->setJustificationType (juce::Justification::centredLeft);
    frequency_label->setEditable (true, true, false);
    frequency_label->setColour (juce::Label::backgroundColourId, juce::Colour (0xffa6acaf));
    frequency_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    frequency_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    frequency_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff263238));
    frequency_label->addListener (this);

    frequency_label->setBounds (1010, 175, 60, 25);

    quality_label.reset (new juce::Label ("Quality",
                                          juce::String()));
    addAndMakeVisible (quality_label.get());
    quality_label->setFont (juce::Font ("Gill Sans", 15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    quality_label->setJustificationType (juce::Justification::centredLeft);
    quality_label->setEditable (true, true, false);
    quality_label->setColour (juce::Label::backgroundColourId, juce::Colour (0xffa6acaf));
    quality_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    quality_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    quality_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff263238));
    quality_label->addListener (this);

    quality_label->setBounds (1010, 225, 60, 25);

    gain_label.reset (new juce::Label ("Gain",
                                       juce::String()));
    addAndMakeVisible (gain_label.get());
    gain_label->setFont (juce::Font ("Gill Sans", 15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain_label->setJustificationType (juce::Justification::centredLeft);
    gain_label->setEditable (true, true, false);
    gain_label->setColour (juce::Label::backgroundColourId, juce::Colour (0xffa6acaf));
    gain_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0xff263238));
    gain_label->addListener (this);

    gain_label->setBounds (1010, 275, 60, 25);

    calculate_button.reset (new juce::TextButton ("Calculate"));
    addAndMakeVisible (calculate_button.get());
    calculate_button->setButtonText (juce::String());
    calculate_button->addListener (this);
    calculate_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff505050));
    calculate_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    calculate_button->setBounds (1070, 352, 90, 30);

    multiply_phases_button.reset (new juce::TextButton ("Multiply phases"));
    addAndMakeVisible (multiply_phases_button.get());
    multiply_phases_button->setButtonText (juce::String());
    multiply_phases_button->addListener (this);
    multiply_phases_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff505050));
    multiply_phases_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    multiply_phases_button->setBounds (355, 462, 90, 30);

    divide_phases_button.reset (new juce::TextButton ("Divide phases"));
    addAndMakeVisible (divide_phases_button.get());
    divide_phases_button->setButtonText (juce::String());
    divide_phases_button->addListener (this);
    divide_phases_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff505050));
    divide_phases_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    divide_phases_button->setBounds (355, 532, 90, 30);

    swap_button.reset (new juce::TextButton ("Swap poles/zeros"));
    addAndMakeVisible (swap_button.get());
    swap_button->setButtonText (juce::String());
    swap_button->addListener (this);
    swap_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff505050));
    swap_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    swap_button->setBounds (355, 602, 90, 30);


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

    e1_type->setLookAndFeel(&typeSwitchesTheme);
    e2_type->setLookAndFeel(&typeSwitchesTheme);
    e3_type->setLookAndFeel(&typeSwitchesTheme);
    e4_type->setLookAndFeel(&typeSwitchesTheme);
    e5_type->setLookAndFeel(&typeSwitchesTheme);
    e6_type->setLookAndFeel(&typeSwitchesTheme);
    e7_type->setLookAndFeel(&typeSwitchesTheme);
    e8_type->setLookAndFeel(&typeSwitchesTheme);

    e1_active->setLookAndFeel(&activeSwitchesTheme);
    e2_active->setLookAndFeel(&activeSwitchesTheme);
    e3_active->setLookAndFeel(&activeSwitchesTheme);
    e4_active->setLookAndFeel(&activeSwitchesTheme);
    e5_active->setLookAndFeel(&activeSwitchesTheme);
    e6_active->setLookAndFeel(&activeSwitchesTheme);
    e7_active->setLookAndFeel(&activeSwitchesTheme);
    e8_active->setLookAndFeel(&activeSwitchesTheme);

    reset_button->setLookAndFeel(&resetButtonTheme);
    gain_slider->setLookAndFeel(&gainSliderTheme);
    bypass->setLookAndFeel(&bypassSwitchTheme);
    linLog_switch->setLookAndFeel(&linLogTheme);
    calculate_button->setLookAndFeel(&calculateButtonTheme);
    multiply_phases_button->setLookAndFeel(&multiplyButtonTheme);
    divide_phases_button->setLookAndFeel(&divideButtonTheme);
    swap_button->setLookAndFeel(&swapButtonTheme);

    double sampleRate = processor.getSampleRate();

    updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);
    updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);
    updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);
    updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);
    updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);
    updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);
    updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);
    updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);
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

    reset_button = nullptr;
    frequency_response = nullptr;
    freq_response_label = nullptr;
    phase_response = nullptr;
    ph_response_label = nullptr;
    gaussian_plane = nullptr;
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
    e1_led = nullptr;
    e2_led = nullptr;
    e3_led = nullptr;
    e4_led = nullptr;
    e5_led = nullptr;
    e6_led = nullptr;
    e7_led = nullptr;
    e8_led = nullptr;
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
    frequency_label = nullptr;
    quality_label = nullptr;
    gain_label = nullptr;
    calculate_button = nullptr;
    multiply_phases_button = nullptr;
    divide_phases_button = nullptr;
    swap_button = nullptr;


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
        float x = 975.0f, y = 15.0f, width = 210.0f, height = 720.0f;
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
        float x = 15.0f, y = 15.0f, width = 480.0f, height = 720.0f;
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
        float x = 495.0f, y = 15.0f, width = 480.0f, height = 720.0f;
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
        int x = 1092, y = 708, width = 90, height = 30;
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
        int x = 15, y = 90, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 365, y = 708, width = 116, height = 30;
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
        int x = 800, y = 708, width = 186, height = 30;
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
        int x = 15, y = 135, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 180, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 225, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 270, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 360, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 315, width = 480, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 975, y = 415, width = 210, height = 1;
        juce::Colour fillColour = juce::Colour (0x39909497);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 1035, y = 420, width = 90, height = 24;
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
        int x = 1025, y = 20, width = 110, height = 24;
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
        int x = 1116, y = 475, width = 40, height = 20;
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
        int x = 1085, y = 178, width = 81, height = 20;
        juce::String text (TRANS ("FREQUENCY"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 1085, y = 228, width = 81, height = 20;
        juce::String text (TRANS ("QUALITY"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
    }

    {
        int x = 1085, y = 278, width = 81, height = 20;
        juce::String text (TRANS ("GAIN"));
        juce::Colour fillColour = juce::Colour (0xff333333);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centredLeft, true);
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
    else if (buttonThatWasClicked == e1_active.get())
    {
        //[UserButtonCode_e1_active] -- add your button handler code here..
        e1_led->repaint();
        //[/UserButtonCode_e1_active]
    }
    else if (buttonThatWasClicked == e2_active.get())
    {
        //[UserButtonCode_e2_active] -- add your button handler code here..
        e2_led->repaint();
        //[/UserButtonCode_e2_active]
    }
    else if (buttonThatWasClicked == e3_active.get())
    {
        //[UserButtonCode_e3_active] -- add your button handler code here..
        e3_led->repaint();
        //[/UserButtonCode_e3_active]
    }
    else if (buttonThatWasClicked == e4_active.get())
    {
        //[UserButtonCode_e4_active] -- add your button handler code here..
        e4_led->repaint();
        //[/UserButtonCode_e4_active]
    }
    else if (buttonThatWasClicked == e5_active.get())
    {
        //[UserButtonCode_e5_active] -- add your button handler code here..
        e5_led->repaint();
        //[/UserButtonCode_e5_active]
    }
    else if (buttonThatWasClicked == e6_active.get())
    {
        //[UserButtonCode_e6_active] -- add your button handler code here..
        e6_led->repaint();
        //[/UserButtonCode_e6_active]
    }
    else if (buttonThatWasClicked == e7_active.get())
    {
        //[UserButtonCode_e7_active] -- add your button handler code here..
        e7_led->repaint();
        //[/UserButtonCode_e7_active]
    }
    else if (buttonThatWasClicked == e8_active.get())
    {
        //[UserButtonCode_e8_active] -- add your button handler code here..
        e8_led->repaint();
        //[/UserButtonCode_e8_active]
    }
    else if (buttonThatWasClicked == linLog_switch.get())
    {
        //[UserButtonCode_linLog_switch] -- add your button handler code here..
        linLog = linLog_switch->getToggleState();
        getSpectrum();
        updateReferenceFrequencies();
        frequency_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate());
        phase_response->updateValues(phases, referenceFrequencies, processor.getSampleRate());
        //[/UserButtonCode_linLog_switch]
    }
    else if (buttonThatWasClicked == calculate_button.get())
    {
        //[UserButtonCode_calculate_button] -- add your button handler code here..
        filterDesignCalculation();
        //[/UserButtonCode_calculate_button]
    }
    else if (buttonThatWasClicked == multiply_phases_button.get())
    {
        //[UserButtonCode_multiply_phases_button] -- add your button handler code here..
        processor.multiplyPhases();
        //[/UserButtonCode_multiply_phases_button]
    }
    else if (buttonThatWasClicked == divide_phases_button.get())
    {
        //[UserButtonCode_divide_phases_button] -- add your button handler code here..
        processor.dividePhases();
        //[/UserButtonCode_divide_phases_button]
    }
    else if (buttonThatWasClicked == swap_button.get())
    {
        //[UserButtonCode_swap_button] -- add your button handler code here..
        processor.swapElements();
        //[/UserButtonCode_swap_button]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void PluginEditor::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    const double sampleRate = processor.getSampleRate();
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == p1_slider.get())
    {
        //[UserSliderCode_p1_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);
        //[/UserSliderCode_p1_slider]
    }
    else if (sliderThatWasMoved == p2_slider.get())
    {
        //[UserSliderCode_p2_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);
        //[/UserSliderCode_p2_slider]
    }
    else if (sliderThatWasMoved == p3_slider.get())
    {
        //[UserSliderCode_p3_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);
        //[/UserSliderCode_p3_slider]
    }
    else if (sliderThatWasMoved == p4_slider.get())
    {
        //[UserSliderCode_p4_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);
        //[/UserSliderCode_p4_slider]
    }
    else if (sliderThatWasMoved == p5_slider.get())
    {
        //[UserSliderCode_p5_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);
        //[/UserSliderCode_p5_slider]
    }
    else if (sliderThatWasMoved == p6_slider.get())
    {
        //[UserSliderCode_p6_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);
        //[/UserSliderCode_p6_slider]
    }
    else if (sliderThatWasMoved == p7_slider.get())
    {
        //[UserSliderCode_p7_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);
        //[/UserSliderCode_p7_slider]
    }
    else if (sliderThatWasMoved == p8_slider.get())
    {
        //[UserSliderCode_p8_slider] -- add your slider handling code here..
        updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);
        //[/UserSliderCode_p8_slider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void PluginEditor::labelTextChanged (juce::Label* labelThatHasChanged)
{
    //[UserlabelTextChanged_Pre]
    const double sampleRate = processor.getSampleRate();
    int newFrequency = labelThatHasChanged->getText().getIntValue();
    //[/UserlabelTextChanged_Pre]

    if (labelThatHasChanged == p1_freq.get())
    {
        //[UserLabelCode_p1_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p1_slider.get(), sampleRate);
        updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);
        //[/UserLabelCode_p1_freq]
    }
    else if (labelThatHasChanged == p2_freq.get())
    {
        //[UserLabelCode_p2_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p2_slider.get(), sampleRate);
        updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);
        //[/UserLabelCode_p2_freq]
    }
    else if (labelThatHasChanged == p3_freq.get())
    {
        //[UserLabelCode_p3_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p3_slider.get(), sampleRate);
        updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);
        //[/UserLabelCode_p3_freq]
    }
    else if (labelThatHasChanged == p4_freq.get())
    {
        //[UserLabelCode_p4_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p4_slider.get(), sampleRate);
        updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);
        //[/UserLabelCode_p4_freq]
    }
    else if (labelThatHasChanged == p5_freq.get())
    {
        //[UserLabelCode_p5_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p5_slider.get(), sampleRate);
        updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);
        //[/UserLabelCode_p5_freq]
    }
    else if (labelThatHasChanged == p6_freq.get())
    {
        //[UserLabelCode_p6_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p6_slider.get(), sampleRate);
        updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);
        //[/UserLabelCode_p6_freq]
    }
    else if (labelThatHasChanged == p7_freq.get())
    {
        //[UserLabelCode_p7_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p7_slider.get(), sampleRate);
        updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);
        //[/UserLabelCode_p7_freq]
    }
    else if (labelThatHasChanged == p8_freq.get())
    {
        //[UserLabelCode_p8_freq] -- add your label text handling code here..
        updateSliderFromFrequency(newFrequency, p8_slider.get(), sampleRate);
        updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);
        //[/UserLabelCode_p8_freq]
    }
    else if (labelThatHasChanged == frequency_label.get())
    {
        //[UserLabelCode_frequency_label] -- add your label text handling code here..
        formatFrequencyInput(newFrequency, frequency_label.get(), sampleRate);
        //[/UserLabelCode_frequency_label]
    }
    else if (labelThatHasChanged == quality_label.get())
    {
        //[UserLabelCode_quality_label] -- add your label text handling code here..
        formatQualityInput(labelThatHasChanged->getText().getDoubleValue(), quality_label.get());
        //[/UserLabelCode_quality_label]
    }
    else if (labelThatHasChanged == gain_label.get())
    {
        //[UserLabelCode_gain_label] -- add your label text handling code here..
        formatGainInput(labelThatHasChanged->getText().getDoubleValue(), gain_label.get());
        //[/UserLabelCode_gain_label]
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
        //[/UserComboBoxCode_type_box]
    }
    else if (comboBoxThatHasChanged == shape_box.get())
    {
        //[UserComboBoxCode_shape_box] -- add your combo box handling code here..
        design_shape = comboBoxThatHasChanged->getSelectedId();
        //[/UserComboBoxCode_shape_box]
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
    auto n2 = log(0.5) - log(FREQUENCY_FLOOR / sampleRate);
    std::complex<double> spectrum;

    for (int i = 0; i < GRAPHS_QUALITY; ++ i)
    {
        if (linLog)
            phi = static_cast<double>(i) / static_cast<double>(2 * (GRAPHS_QUALITY - 1)); // Linear spectrum
        else
            phi = exp(n1 + (n2 * (static_cast<double>(i) / (static_cast<double>(GRAPHS_QUALITY - 1))))); // Log spectrum

        spectrum = processor.getFilterSpectrum(phi);
        magnitudes[i] = processor.getCurrentGain() * std::abs(spectrum);
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
    double phaseValue = slider->getValue();
    double frequency = (phaseValue * sampleRate) / 2.0;

    label->setText(juce::String(juce::roundToInt(frequency)) + " Hz", juce::dontSendNotification);
}

void PluginEditor::updateSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate)
{
    double phaseValue = (frequency * 2.0) / sampleRate;
    slider->setValue(phaseValue, juce::sendNotificationSync);
}

void PluginEditor::formatFrequencyInput(int frequency, juce::Label *label, double sampleRate)
{
    double maxFrequency = sampleRate / 2.0;

    if (frequency < FREQUENCY_FLOOR)
    {
        frequency = FREQUENCY_FLOOR;
    }
    else if (frequency > maxFrequency)
    {
        frequency = maxFrequency;
    }
    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);
    design_frequency = frequency;
}

void PluginEditor::formatQualityInput(double quality, juce::Label *label)
{
    if (quality < QUALITY_FLOOR)
    {
        quality = QUALITY_FLOOR;
    }
    else if (quality > QUALITY_CEILING)
    {
        quality = QUALITY_CEILING;
    }
    quality = std::round(quality * 10) / 10;
    label->setText(juce::String(quality), juce::dontSendNotification);
    design_quality = quality;
}

void PluginEditor::formatGainInput(double gain, juce::Label *label)
{
    if (gain < DESIGN_GAIN_FLOOR)
    {
        gain = DESIGN_GAIN_FLOOR;
    }
    else if (gain > DESIGN_GAIN_CEILING)
    {
        gain = DESIGN_GAIN_CEILING;
    }
    gain = std::round(gain * 10) / 10;
    label->setText(juce::String(gain) + " dB", juce::dontSendNotification);
    design_gain = gain;
}

void PluginEditor::filterDesignCalculation()
{

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
    <ROUNDRECT pos="975 15 210 720" cornerSize="14.5" fill="solid: 19656565"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <ROUNDRECT pos="15 15 480 720" cornerSize="14.5" fill="solid: 17b1b1b1"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <ROUNDRECT pos="495 15 480 720" cornerSize="14.5" fill="solid: 17b1b1b1"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <TEXT pos="1092 708 90 30" fill="solid: ff333333" hasStroke="0" text="TOOLS"
          fontname="Gill Sans" fontsize="16.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <RECT pos="15 90 480 1" fill="solid: 25909497" hasStroke="0"/>
    <TEXT pos="365 708 116 30" fill="solid: ff333333" hasStroke="0" text="FILTER SETUP"
          fontname="Gill Sans" fontsize="16.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="800 708 186 30" fill="solid: ff333333" hasStroke="0" text="FILTER RESPONSE"
          fontname="Gill Sans" fontsize="16.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <RECT pos="15 135 480 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 180 480 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 225 480 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 270 480 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 360 480 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 315 480 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="975 415 210 1" fill="solid: 39909497" hasStroke="0"/>
    <TEXT pos="1035 420 90 24" fill="solid: ff333333" hasStroke="0" text="CONTROLS"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1025 20 110 24" fill="solid: ff333333" hasStroke="0" text="FILTER DESIGN"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1116 475 40 20" fill="solid: ff333333" hasStroke="0" text="GAIN"
          fontname="Gill Sans" fontsize="10.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1085 178 81 20" fill="solid: ff333333" hasStroke="0" text="FREQUENCY"
          fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
          justification="33" typefaceStyle="SemiBold"/>
    <TEXT pos="1085 228 81 20" fill="solid: ff333333" hasStroke="0" text="QUALITY"
          fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
          justification="33" typefaceStyle="SemiBold"/>
    <TEXT pos="1085 278 81 20" fill="solid: ff333333" hasStroke="0" text="GAIN"
          fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
          justification="33" typefaceStyle="SemiBold"/>
  </BACKGROUND>
  <TEXTBUTTON name="Reset" id="2581837dc85daae9" memberName="reset_button"
              virtualName="" explicitFocusOrder="0" pos="1010 520 70 30" bgColOff="ff505050"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <GENERICCOMPONENT name="frequencyResponse" id="161cb81e63dc8e46" memberName="frequency_response"
                    virtualName="" explicitFocusOrder="0" pos="520 35 430 270" class="GraphicResponse"
                    params="magnitudes, referenceFrequencies, processor.getSampleRate()"/>
  <LABEL name="Frequency response" id="4c8fffb65e845bfc" memberName="freq_response_label"
         virtualName="" explicitFocusOrder="0" pos="665 310 140 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="SPECTRUM MAGNITUDE&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <GENERICCOMPONENT name="phaseResponse" id="c9a48273dec25832" memberName="phase_response"
                    virtualName="" explicitFocusOrder="0" pos="540 415 390 260" class="GraphicResponse"
                    params="phases, referenceFrequencies, processor.getSampleRate()"/>
  <LABEL name="Phase response" id="6d08c4e421703ed5" memberName="ph_response_label"
         virtualName="" explicitFocusOrder="0" pos="675 680 110 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="SPECTRUM PHASE"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <GENERICCOMPONENT name="gaussianPlane" id="f84485816497c4e3" memberName="gaussian_plane"
                    virtualName="" explicitFocusOrder="0" pos="35 415 260 260" class="GaussianPlane"
                    params="processor.getFilterElementsChain()"/>
  <SLIDER name="Element 1 magnitude" id="9107cda7959bc0ad" memberName="m1_slider"
          virtualName="" explicitFocusOrder="0" pos="20 55 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
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
         virtualName="" explicitFocusOrder="0" pos="197 20 62 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="PHASE&#10;" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <LABEL name="Zero / Pole" id="676532893526fbc7" memberName="zero_pole_label"
         virtualName="" explicitFocusOrder="0" pos="322 20 91 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="ZERO  /  POLE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <LABEL name="Active" id="e82d0bcbcd7af0" memberName="active_label" virtualName=""
         explicitFocusOrder="0" pos="423 20 55 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="ACTIVE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Element 1 type" id="cb026ef1068db639" memberName="e1_type"
                virtualName="" explicitFocusOrder="0" pos="343 56 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 2 type" id="56addf679899dfb" memberName="e2_type"
                virtualName="" explicitFocusOrder="0" pos="343 101 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 3 type" id="5abda647a887dc8e" memberName="e3_type"
                virtualName="" explicitFocusOrder="0" pos="343 146 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 4 type" id="3136ab6a98246edb" memberName="e4_type"
                virtualName="" explicitFocusOrder="0" pos="343 191 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 5 type" id="4d6d7aa8e46a31c6" memberName="e5_type"
                virtualName="" explicitFocusOrder="0" pos="343 236 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 6 type" id="6a190298ad04abf" memberName="e6_type"
                virtualName="" explicitFocusOrder="0" pos="343 281 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 7 type" id="57fd1da9881e7a6a" memberName="e7_type"
                virtualName="" explicitFocusOrder="0" pos="343 326 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 8 type" id="a0b1086b7f717581" memberName="e8_type"
                virtualName="" explicitFocusOrder="0" pos="343 371 52 21" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 1 active" id="84bb86bdbd9ccede" memberName="e1_active"
                virtualName="" explicitFocusOrder="0" pos="420 56 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 2 active" id="176e3fb309f12ef5" memberName="e2_active"
                virtualName="" explicitFocusOrder="0" pos="420 101 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 3 active" id="d6d9c0542e41b9c3" memberName="e3_active"
                virtualName="" explicitFocusOrder="0" pos="420 146 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 4 active" id="72d6ebba0023e28e" memberName="e4_active"
                virtualName="" explicitFocusOrder="0" pos="420 191 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 5 active" id="d8c38a2ce324a73d" memberName="e5_active"
                virtualName="" explicitFocusOrder="0" pos="420 236 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 6 active" id="1b42bda33e1c947c" memberName="e6_active"
                virtualName="" explicitFocusOrder="0" pos="420 281 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 7 active" id="3405e0694e2b063d" memberName="e7_active"
                virtualName="" explicitFocusOrder="0" pos="420 326 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 8 active" id="b60e0b9c2104a2bd" memberName="e8_active"
                virtualName="" explicitFocusOrder="0" pos="420 371 35 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <LABEL name="Gaussian plane" id="32980deccea32218" memberName="gaussian_plane_label"
         virtualName="" explicitFocusOrder="0" pos="95 680 140 24" textCol="ff333333"
         edTextCol="ff000000" edBkgCol="0" labelText="GAUSSIAN PLANE"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <GENERICCOMPONENT name="Led 1" id="33ac2e372b117cf2" memberName="e1_led" virtualName=""
                    explicitFocusOrder="0" pos="465 59 17 17" class="LEDComponent"
                    params="*e1_active"/>
  <GENERICCOMPONENT name="Led 2" id="dcdf09a978cf18a" memberName="e2_led" virtualName=""
                    explicitFocusOrder="0" pos="465 104 17 17" class="LEDComponent"
                    params="*e2_active"/>
  <GENERICCOMPONENT name="Led 3" id="18dec958a156d78e" memberName="e3_led" virtualName=""
                    explicitFocusOrder="0" pos="465 149 17 17" class="LEDComponent"
                    params="*e3_active"/>
  <GENERICCOMPONENT name="Led 4" id="7d8ac1c3c871f7f2" memberName="e4_led" virtualName=""
                    explicitFocusOrder="0" pos="465 194 17 17" class="LEDComponent"
                    params="*e4_active"/>
  <GENERICCOMPONENT name="Led 5" id="3755e40ecb2571a4" memberName="e5_led" virtualName=""
                    explicitFocusOrder="0" pos="465 239 17 17" class="LEDComponent"
                    params="*e5_active"/>
  <GENERICCOMPONENT name="Led 6" id="90777192516955ee" memberName="e6_led" virtualName=""
                    explicitFocusOrder="0" pos="465 284 17 17" class="LEDComponent"
                    params="*e6_active&#10;"/>
  <GENERICCOMPONENT name="Led 7" id="19ab27e27b8109fa" memberName="e7_led" virtualName=""
                    explicitFocusOrder="0" pos="465 329 17 17" class="LEDComponent"
                    params="*e7_active"/>
  <GENERICCOMPONENT name="Led 8" id="f329431bc31bbc97" memberName="e8_led" virtualName=""
                    explicitFocusOrder="0" pos="465 374 17 17" class="LEDComponent"
                    params="*e8_active"/>
  <TOGGLEBUTTON name="Bypass" id="4daf2a36bc47407c" memberName="bypass" virtualName=""
                explicitFocusOrder="0" pos="1010 600 70 30" buttonText="" connectedEdges="0"
                needsCallback="0" radioGroupId="0" state="0"/>
  <SLIDER name="Gain" id="7e880f1fc774c2af" memberName="gain_slider" virtualName=""
          explicitFocusOrder="0" pos="1115 495 45 180" thumbcol="ffffffff"
          textboxtext="ff383838" textboxhighlight="66686868" textboxoutline="8e989b"
          min="0.0" max="10.0" int="0.0" style="LinearVertical" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="100" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <TOGGLEBUTTON name="Linear / Logarithmic" id="269b07022e58d862" memberName="linLog_switch"
                virtualName="" explicitFocusOrder="0" pos="901 312 52 21" buttonText=""
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="Element 2 magnitude" id="e1a6d1b02cc8ab80" memberName="m2_slider"
          virtualName="" explicitFocusOrder="0" pos="20 100 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <SLIDER name="Element 3 magnitude" id="b0094bd01977f340" memberName="m3_slider"
          virtualName="" explicitFocusOrder="0" pos="20 145 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <SLIDER name="Element 4 magnitude" id="8f433acd768ec1d9" memberName="m4_slider"
          virtualName="" explicitFocusOrder="0" pos="20 190 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <SLIDER name="Element 5 magnitude" id="fe0864ae3596b10d" memberName="m5_slider"
          virtualName="" explicitFocusOrder="0" pos="20 235 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <SLIDER name="Element 6 magnitude" id="e6371565d3e4bb47" memberName="m6_slider"
          virtualName="" explicitFocusOrder="0" pos="20 280 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <SLIDER name="Element 7 magnitude" id="3661ca4111317400" memberName="m7_slider"
          virtualName="" explicitFocusOrder="0" pos="20 325 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <SLIDER name="Element 8 magnitude" id="8e83a0a6b9d1e264" memberName="m8_slider"
          virtualName="" explicitFocusOrder="0" pos="20 370 120 25" thumbcol="ffffffff"
          textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
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
  <COMBOBOX name="Type" id="5a16af79e3a09d2b" memberName="type_box" virtualName=""
            explicitFocusOrder="0" pos="1010 65 140 25" editable="0" layout="33"
            items="butterworth&#10;chebyshev&#10;elliptic&#10;bessel&#10;biquad&#10;comb"
            textWhenNonSelected="-" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Shape" id="fab26cb579c24549" memberName="shape_box" virtualName=""
            explicitFocusOrder="0" pos="1010 120 140 25" editable="0" layout="33"
            items="lowpass&#10;highpass&#10;bandpass&#10;bandstop" textWhenNonSelected="-"
            textWhenNoItems="(no choices)"/>
  <LABEL name="Frequency" id="ddaa2a20ba2e76a5" memberName="frequency_label"
         virtualName="" explicitFocusOrder="0" pos="1010 175 60 25" bkgCol="ffa6acaf"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff263238" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="15.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Quality" id="c37d7b01efb8bd68" memberName="quality_label"
         virtualName="" explicitFocusOrder="0" pos="1010 225 60 25" bkgCol="ffa6acaf"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff263238" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="15.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Gain" id="b9d8ae0f9bd417f8" memberName="gain_label" virtualName=""
         explicitFocusOrder="0" pos="1010 275 60 25" bkgCol="ffa6acaf"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff263238" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="15.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <TEXTBUTTON name="Calculate" id="6b0929d790004858" memberName="calculate_button"
              virtualName="" explicitFocusOrder="0" pos="1070 352 90 30" bgColOff="ff505050"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Multiply phases" id="fd9508a1dc4c09ae" memberName="multiply_phases_button"
              virtualName="" explicitFocusOrder="0" pos="355 462 90 30" bgColOff="ff505050"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Divide phases" id="a62537b9345044f3" memberName="divide_phases_button"
              virtualName="" explicitFocusOrder="0" pos="355 532 90 30" bgColOff="ff505050"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="Swap poles/zeros" id="938b23da9ff326b0" memberName="swap_button"
              virtualName="" explicitFocusOrder="0" pos="355 602 90 30" bgColOff="ff505050"
              bgColOn="ff505050" buttonText="" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

