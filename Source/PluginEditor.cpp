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
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
PluginEditor::PluginEditor (PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processor (p), valueTreeState (vts)
{
    //[Constructor_pre] You can add your own custom stuff here..
    magnitudesAttachments.reserve(NUMBER_OF_ELEMENTS);
    phasesAttachments.reserve(NUMBER_OF_ELEMENTS);
    typesAttachments.reserve(NUMBER_OF_ELEMENTS);
    activeAttachments.reserve(NUMBER_OF_ELEMENTS);
    //[/Constructor_pre]

    frequency_response.reset (new FrequencyResponse (processor.getFilterSpectrum()));
    addAndMakeVisible (frequency_response.get());
    frequency_response->setName ("frequencyResponse");

    frequency_response->setBounds (990, 30, 490, 335);

    freq_response_label.reset (new juce::Label ("Frequency response",
                                                TRANS ("FREQUENCY RESPONSE")));
    addAndMakeVisible (freq_response_label.get());
    freq_response_label->setFont (juce::Font ("Poppins", 20.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    freq_response_label->setJustificationType (juce::Justification::centred);
    freq_response_label->setEditable (false, false, false);
    freq_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    freq_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    freq_response_label->setBounds (1150, 374, 181, 24);

    phase_response.reset (new PhaseResponse (processor.getFilterSpectrum()));
    addAndMakeVisible (phase_response.get());
    phase_response->setName ("phaseResponse");

    phase_response->setBounds (990, 460, 490, 335);

    ph_response_label.reset (new juce::Label ("Phase response",
                                              TRANS ("PHASE RESPONSE")));
    addAndMakeVisible (ph_response_label.get());
    ph_response_label->setFont (juce::Font ("Poppins", 20.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    ph_response_label->setJustificationType (juce::Justification::centred);
    ph_response_label->setEditable (false, false, false);
    ph_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    ph_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    ph_response_label->setBounds (1175, 803, 142, 24);

    gaussian_plane.reset (new GaussianPlane (processor.getFilterElementsChain()));
    addAndMakeVisible (gaussian_plane.get());
    gaussian_plane->setName ("gaussianPlane");

    gaussian_plane->setBounds (630, 30, 335, 335);

    m1_slider.reset (new juce::Slider ("Element 1 magnitude"));
    addAndMakeVisible (m1_slider.get());
    m1_slider->setRange (0, 10, 0);
    m1_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m1_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m1_slider->setBounds (150, 71, 120, 40);

    p1_slider.reset (new juce::Slider ("Element 1 phase"));
    addAndMakeVisible (p1_slider.get());
    p1_slider->setRange (0, 10, 0);
    p1_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p1_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p1_slider->setBounds (290, 71, 118, 40);

    m2_slider.reset (new juce::Slider ("Element 2 magnitude"));
    addAndMakeVisible (m2_slider.get());
    m2_slider->setRange (0, 10, 0);
    m2_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m2_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m2_slider->setBounds (150, 131, 120, 40);

    m3_slider.reset (new juce::Slider ("Element 3 magnitude"));
    addAndMakeVisible (m3_slider.get());
    m3_slider->setRange (0, 10, 0);
    m3_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m3_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m3_slider->setBounds (150, 191, 120, 40);

    m4_slider.reset (new juce::Slider ("Element 4 magnitude"));
    addAndMakeVisible (m4_slider.get());
    m4_slider->setRange (0, 10, 0);
    m4_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m4_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m4_slider->setBounds (150, 251, 120, 40);

    m5_slider.reset (new juce::Slider ("Element 5 magnitude"));
    addAndMakeVisible (m5_slider.get());
    m5_slider->setRange (0, 10, 0);
    m5_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m5_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m5_slider->setBounds (150, 311, 120, 40);

    elements_chain_label.reset (new juce::Label ("Elements chain",
                                                 TRANS ("\n"
                                                 "ELEMENTS CHAIN")));
    addAndMakeVisible (elements_chain_label.get());
    elements_chain_label->setFont (juce::Font ("Poppins", 17.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    elements_chain_label->setJustificationType (juce::Justification::centred);
    elements_chain_label->setEditable (false, false, false);
    elements_chain_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    elements_chain_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    elements_chain_label->setBounds (15, 20, 116, 24);

    element1_label.reset (new juce::Label ("Element 1",
                                           TRANS ("ELEMENT 1")));
    addAndMakeVisible (element1_label.get());
    element1_label->setFont (juce::Font ("Poppins", 15.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    element1_label->setJustificationType (juce::Justification::centred);
    element1_label->setEditable (false, false, false);
    element1_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element1_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element1_label->setBounds (38, 71, 70, 24);

    magnitudes_label.reset (new juce::Label ("Magnitudes",
                                             TRANS ("MAGNITUDES")));
    addAndMakeVisible (magnitudes_label.get());
    magnitudes_label->setFont (juce::Font ("Poppins", 17.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    magnitudes_label->setJustificationType (juce::Justification::centred);
    magnitudes_label->setEditable (false, false, false);
    magnitudes_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    magnitudes_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    magnitudes_label->setBounds (164, 20, 90, 24);

    phases_label.reset (new juce::Label ("Phases",
                                         TRANS ("PHASES")));
    addAndMakeVisible (phases_label.get());
    phases_label->setFont (juce::Font ("Poppins", 17.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    phases_label->setJustificationType (juce::Justification::centred);
    phases_label->setEditable (false, false, false);
    phases_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    phases_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    phases_label->setBounds (314, 20, 62, 24);

    zero_pole_label.reset (new juce::Label ("Zero / Pole",
                                            TRANS ("ZERO / POLE")));
    addAndMakeVisible (zero_pole_label.get());
    zero_pole_label->setFont (juce::Font ("Poppins", 17.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    zero_pole_label->setJustificationType (juce::Justification::centred);
    zero_pole_label->setEditable (false, false, false);
    zero_pole_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    zero_pole_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    zero_pole_label->setBounds (424, 20, 91, 24);

    element2_label.reset (new juce::Label ("Element 2",
                                           TRANS ("ELEMENT 2")));
    addAndMakeVisible (element2_label.get());
    element2_label->setFont (juce::Font ("Poppins", 15.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    element2_label->setJustificationType (juce::Justification::centred);
    element2_label->setEditable (false, false, false);
    element2_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element2_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element2_label->setBounds (38, 131, 70, 24);

    p2_slider.reset (new juce::Slider ("Element 2 phase"));
    addAndMakeVisible (p2_slider.get());
    p2_slider->setRange (0, 10, 0);
    p2_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p2_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p2_slider->setBounds (290, 131, 118, 40);

    active_label.reset (new juce::Label ("Active",
                                         TRANS ("ACTIVE")));
    addAndMakeVisible (active_label.get());
    active_label->setFont (juce::Font ("Poppins", 17.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    active_label->setJustificationType (juce::Justification::centred);
    active_label->setEditable (false, false, false);
    active_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    active_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    active_label->setBounds (544, 20, 55, 24);

    juce__label8.reset (new juce::Label ("new label",
                                         TRANS ("ELEMENT 3")));
    addAndMakeVisible (juce__label8.get());
    juce__label8->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    juce__label8->setJustificationType (juce::Justification::centred);
    juce__label8->setEditable (false, false, false);
    juce__label8->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    juce__label8->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    juce__label8->setBounds (38, 191, 70, 24);

    element3_label.reset (new juce::Label ("Element 3",
                                           TRANS ("ELEMENT 3")));
    addAndMakeVisible (element3_label.get());
    element3_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element3_label->setJustificationType (juce::Justification::centred);
    element3_label->setEditable (false, false, false);
    element3_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element3_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element3_label->setBounds (38, 191, 70, 24);

    element4_label.reset (new juce::Label ("Element 4",
                                           TRANS ("ELEMENT 4")));
    addAndMakeVisible (element4_label.get());
    element4_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element4_label->setJustificationType (juce::Justification::centred);
    element4_label->setEditable (false, false, false);
    element4_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element4_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element4_label->setBounds (38, 251, 70, 24);

    element5_label.reset (new juce::Label ("Element 5",
                                           TRANS ("ELEMENT 5")));
    addAndMakeVisible (element5_label.get());
    element5_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element5_label->setJustificationType (juce::Justification::centred);
    element5_label->setEditable (false, false, false);
    element5_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element5_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element5_label->setBounds (38, 311, 70, 24);

    element6_label.reset (new juce::Label ("Element 6",
                                           TRANS ("ELEMENT 6")));
    addAndMakeVisible (element6_label.get());
    element6_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element6_label->setJustificationType (juce::Justification::centred);
    element6_label->setEditable (false, false, false);
    element6_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element6_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element6_label->setBounds (38, 371, 70, 24);

    element7_label.reset (new juce::Label ("Element 7",
                                           TRANS ("ELEMENT 7")));
    addAndMakeVisible (element7_label.get());
    element7_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element7_label->setJustificationType (juce::Justification::centred);
    element7_label->setEditable (false, false, false);
    element7_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element7_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element7_label->setBounds (38, 431, 70, 24);

    element8_label.reset (new juce::Label ("Element 8",
                                           TRANS ("ELEMENT 8")));
    addAndMakeVisible (element8_label.get());
    element8_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element8_label->setJustificationType (juce::Justification::centred);
    element8_label->setEditable (false, false, false);
    element8_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element8_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element8_label->setBounds (38, 491, 70, 24);

    element9_label.reset (new juce::Label ("Element 9",
                                           TRANS ("ELEMENT 9")));
    addAndMakeVisible (element9_label.get());
    element9_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element9_label->setJustificationType (juce::Justification::centred);
    element9_label->setEditable (false, false, false);
    element9_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element9_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element9_label->setBounds (38, 551, 70, 24);

    element10_label.reset (new juce::Label ("Element 10",
                                            TRANS ("ELEMENT 10")));
    addAndMakeVisible (element10_label.get());
    element10_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element10_label->setJustificationType (juce::Justification::centred);
    element10_label->setEditable (false, false, false);
    element10_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element10_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element10_label->setBounds (38, 611, 70, 24);

    element11_label.reset (new juce::Label ("Element 11",
                                            TRANS ("ELEMENT 11")));
    addAndMakeVisible (element11_label.get());
    element11_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element11_label->setJustificationType (juce::Justification::centred);
    element11_label->setEditable (false, false, false);
    element11_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element11_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element11_label->setBounds (38, 671, 70, 24);

    element12_label.reset (new juce::Label ("Element 12",
                                            TRANS ("ELEMENT 12")));
    addAndMakeVisible (element12_label.get());
    element12_label->setFont (juce::Font (12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    element12_label->setJustificationType (juce::Justification::centred);
    element12_label->setEditable (false, false, false);
    element12_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    element12_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    element12_label->setBounds (38, 731, 70, 24);

    m6_slider.reset (new juce::Slider ("Element 6 magnitude"));
    addAndMakeVisible (m6_slider.get());
    m6_slider->setRange (0, 10, 0);
    m6_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m6_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m6_slider->setBounds (150, 371, 120, 40);

    m7_slider.reset (new juce::Slider ("Element 7 magnitude"));
    addAndMakeVisible (m7_slider.get());
    m7_slider->setRange (0, 10, 0);
    m7_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m7_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m7_slider->setBounds (150, 431, 120, 40);

    m8_slider.reset (new juce::Slider ("Element 8 magnitude"));
    addAndMakeVisible (m8_slider.get());
    m8_slider->setRange (0, 10, 0);
    m8_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m8_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m8_slider->setBounds (150, 491, 120, 40);

    m9_slider.reset (new juce::Slider ("Element 9 magnitude"));
    addAndMakeVisible (m9_slider.get());
    m9_slider->setRange (0, 10, 0);
    m9_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m9_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m9_slider->setBounds (150, 551, 120, 40);

    m10_slider.reset (new juce::Slider ("Element 10 magnitude"));
    addAndMakeVisible (m10_slider.get());
    m10_slider->setRange (0, 10, 0);
    m10_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m10_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m10_slider->setBounds (150, 611, 120, 40);

    m11_slider.reset (new juce::Slider ("Element 11 magnitude"));
    addAndMakeVisible (m11_slider.get());
    m11_slider->setRange (0, 10, 0);
    m11_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m11_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m11_slider->setBounds (150, 671, 120, 40);

    m12_slider.reset (new juce::Slider ("Element 12 magnitude"));
    addAndMakeVisible (m12_slider.get());
    m12_slider->setRange (0, 10, 0);
    m12_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m12_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    m12_slider->setBounds (150, 731, 120, 40);

    p3_slider.reset (new juce::Slider ("Element 3 phase"));
    addAndMakeVisible (p3_slider.get());
    p3_slider->setRange (0, 10, 0);
    p3_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p3_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p3_slider->setBounds (290, 191, 118, 40);

    p4_slider.reset (new juce::Slider ("Element 4 phase"));
    addAndMakeVisible (p4_slider.get());
    p4_slider->setRange (0, 10, 0);
    p4_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p4_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p4_slider->setBounds (290, 251, 118, 40);

    p5_slider.reset (new juce::Slider ("Element 5 phase"));
    addAndMakeVisible (p5_slider.get());
    p5_slider->setRange (0, 10, 0);
    p5_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p5_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p5_slider->setBounds (290, 311, 118, 40);

    p6_slider.reset (new juce::Slider ("Element 6 phase"));
    addAndMakeVisible (p6_slider.get());
    p6_slider->setRange (0, 10, 0);
    p6_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p6_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p6_slider->setBounds (290, 371, 118, 40);

    p7_slider.reset (new juce::Slider ("Element 7 phase"));
    addAndMakeVisible (p7_slider.get());
    p7_slider->setRange (0, 10, 0);
    p7_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p7_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p7_slider->setBounds (290, 431, 118, 40);

    p8_slider.reset (new juce::Slider ("Element 8 phase"));
    addAndMakeVisible (p8_slider.get());
    p8_slider->setRange (0, 10, 0);
    p8_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p8_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p8_slider->setBounds (290, 491, 118, 40);

    p9_slider.reset (new juce::Slider ("Element 9 phase"));
    addAndMakeVisible (p9_slider.get());
    p9_slider->setRange (0, 10, 0);
    p9_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p9_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p9_slider->setBounds (290, 551, 118, 40);

    p10_slider.reset (new juce::Slider ("Element 10 phase"));
    addAndMakeVisible (p10_slider.get());
    p10_slider->setRange (0, 10, 0);
    p10_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p10_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p10_slider->setBounds (290, 611, 118, 40);

    p11_slider.reset (new juce::Slider ("Element 11 phase"));
    addAndMakeVisible (p11_slider.get());
    p11_slider->setRange (0, 10, 0);
    p11_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p11_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p11_slider->setBounds (290, 671, 118, 40);

    p12_slider.reset (new juce::Slider ("Element 12 phase"));
    addAndMakeVisible (p12_slider.get());
    p12_slider->setRange (0, 10, 0);
    p12_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p12_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);

    p12_slider->setBounds (290, 731, 118, 40);

    e1_type.reset (new juce::ToggleButton ("Element 1 type"));
    addAndMakeVisible (e1_type.get());
    e1_type->setButtonText (juce::String());

    e1_type->setBounds (460, 70, 22, 24);

    e2_type.reset (new juce::ToggleButton ("Element 2 type"));
    addAndMakeVisible (e2_type.get());
    e2_type->setButtonText (juce::String());

    e2_type->setBounds (460, 130, 22, 24);

    e3_type.reset (new juce::ToggleButton ("Element 3 type"));
    addAndMakeVisible (e3_type.get());
    e3_type->setButtonText (juce::String());

    e3_type->setBounds (460, 190, 22, 24);

    e4_type.reset (new juce::ToggleButton ("Element 4 type"));
    addAndMakeVisible (e4_type.get());
    e4_type->setButtonText (juce::String());

    e4_type->setBounds (460, 250, 22, 24);

    e5_type.reset (new juce::ToggleButton ("Element 5 type"));
    addAndMakeVisible (e5_type.get());
    e5_type->setButtonText (juce::String());

    e5_type->setBounds (460, 310, 22, 24);

    e6_type.reset (new juce::ToggleButton ("Element 6 type"));
    addAndMakeVisible (e6_type.get());
    e6_type->setButtonText (juce::String());

    e6_type->setBounds (460, 370, 22, 24);

    e7_type.reset (new juce::ToggleButton ("Element 7 type"));
    addAndMakeVisible (e7_type.get());
    e7_type->setButtonText (juce::String());

    e7_type->setBounds (460, 430, 22, 24);

    e8_type.reset (new juce::ToggleButton ("Element 8 type"));
    addAndMakeVisible (e8_type.get());
    e8_type->setButtonText (juce::String());

    e8_type->setBounds (460, 490, 22, 24);

    e9_type.reset (new juce::ToggleButton ("Element 9 type"));
    addAndMakeVisible (e9_type.get());
    e9_type->setButtonText (juce::String());

    e9_type->setBounds (460, 550, 22, 24);

    e10_type.reset (new juce::ToggleButton ("Element 10 type"));
    addAndMakeVisible (e10_type.get());
    e10_type->setButtonText (juce::String());

    e10_type->setBounds (460, 610, 22, 24);

    e11_type.reset (new juce::ToggleButton ("Element 11 type"));
    addAndMakeVisible (e11_type.get());
    e11_type->setButtonText (juce::String());

    e11_type->setBounds (460, 670, 22, 24);

    e12_type.reset (new juce::ToggleButton ("Element 12 type"));
    addAndMakeVisible (e12_type.get());
    e12_type->setButtonText (juce::String());

    e12_type->setBounds (460, 730, 22, 24);

    e1_active.reset (new juce::ToggleButton ("Element 1 active"));
    addAndMakeVisible (e1_active.get());
    e1_active->setButtonText (juce::String());

    e1_active->setBounds (561, 70, 22, 24);

    e2_active.reset (new juce::ToggleButton ("Element 2 active"));
    addAndMakeVisible (e2_active.get());
    e2_active->setButtonText (juce::String());

    e2_active->setBounds (561, 130, 22, 24);

    e3_active.reset (new juce::ToggleButton ("Element 3 active"));
    addAndMakeVisible (e3_active.get());
    e3_active->setButtonText (juce::String());

    e3_active->setBounds (561, 190, 22, 24);

    e4_active.reset (new juce::ToggleButton ("Element 4 active"));
    addAndMakeVisible (e4_active.get());
    e4_active->setButtonText (juce::String());

    e4_active->setBounds (561, 250, 22, 24);

    e5_active.reset (new juce::ToggleButton ("Element 5 active"));
    addAndMakeVisible (e5_active.get());
    e5_active->setButtonText (juce::String());

    e5_active->setBounds (561, 310, 22, 24);

    e6_active.reset (new juce::ToggleButton ("Element 6 active"));
    addAndMakeVisible (e6_active.get());
    e6_active->setButtonText (juce::String());

    e6_active->setBounds (561, 370, 22, 24);

    e7_active.reset (new juce::ToggleButton ("Element 7 active"));
    addAndMakeVisible (e7_active.get());
    e7_active->setButtonText (juce::String());

    e7_active->setBounds (561, 430, 22, 24);

    e8_active.reset (new juce::ToggleButton ("Element 8 active"));
    addAndMakeVisible (e8_active.get());
    e8_active->setButtonText (juce::String());

    e8_active->setBounds (561, 490, 22, 24);

    e9_active.reset (new juce::ToggleButton ("Element 9 active"));
    addAndMakeVisible (e9_active.get());
    e9_active->setButtonText (juce::String());

    e9_active->setBounds (561, 550, 22, 24);

    e10_active.reset (new juce::ToggleButton ("Element 10 active"));
    addAndMakeVisible (e10_active.get());
    e10_active->setButtonText (juce::String());

    e10_active->setBounds (561, 610, 22, 24);

    e11_active.reset (new juce::ToggleButton ("Element 11 active"));
    addAndMakeVisible (e11_active.get());
    e11_active->setButtonText (juce::String());

    e11_active->setBounds (561, 670, 22, 24);

    e12_active.reset (new juce::ToggleButton ("Element 12 active"));
    addAndMakeVisible (e12_active.get());
    e12_active->setButtonText (juce::String());

    e12_active->setBounds (561, 730, 22, 24);

    gaussian_plane_label.reset (new juce::Label ("Gaussian plane",
                                                 TRANS ("GAUSSIAN PLANE")));
    addAndMakeVisible (gaussian_plane_label.get());
    gaussian_plane_label->setFont (juce::Font ("Poppins", 20.00f, juce::Font::plain).withTypefaceStyle ("Regular").withExtraKerningFactor (0.020f));
    gaussian_plane_label->setJustificationType (juce::Justification::centred);
    gaussian_plane_label->setEditable (false, false, false);
    gaussian_plane_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gaussian_plane_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    gaussian_plane_label->setBounds (727, 373, 141, 24);


    //[UserPreSize]
    magnitudesAttachments[0].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(1), *m1_slider));
    magnitudesAttachments[1].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(2), *m2_slider));
    magnitudesAttachments[2].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(3), *m3_slider));
    magnitudesAttachments[3].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(4), *m4_slider));
    magnitudesAttachments[4].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(5), *m5_slider));
    magnitudesAttachments[5].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(6), *m6_slider));
    magnitudesAttachments[6].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(7), *m7_slider));
    magnitudesAttachments[7].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(8), *m8_slider));
    magnitudesAttachments[8].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(9), *m9_slider));
    magnitudesAttachments[9].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(10), *m10_slider));
    magnitudesAttachments[10].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(11), *m11_slider));
    magnitudesAttachments[11].reset(new SliderAttachment(valueTreeState, MAGNITUDE_NAME + std::to_string(12), *m12_slider));

    phasesAttachments[0].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(1), *p1_slider));
    phasesAttachments[1].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(2), *p2_slider));
    phasesAttachments[2].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(3), *p3_slider));
    phasesAttachments[3].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(4), *p4_slider));
    phasesAttachments[4].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(5), *p5_slider));
    phasesAttachments[5].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(6), *p6_slider));
    phasesAttachments[6].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(7), *p7_slider));
    phasesAttachments[7].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(8), *p8_slider));
    phasesAttachments[8].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(9), *p9_slider));
    phasesAttachments[9].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(10), *p10_slider));
    phasesAttachments[10].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(11), *p11_slider));
    phasesAttachments[11].reset(new SliderAttachment(valueTreeState, PHASE_NAME + std::to_string(12), *p12_slider));

    typesAttachments[0].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(1), *e1_type));
    typesAttachments[1].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(2), *e2_type));
    typesAttachments[2].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(3), *e3_type));
    typesAttachments[3].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(4), *e4_type));
    typesAttachments[4].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(5), *e5_type));
    typesAttachments[5].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(6), *e6_type));
    typesAttachments[6].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(7), *e7_type));
    typesAttachments[7].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(8), *e8_type));
    typesAttachments[8].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(9), *e9_type));
    typesAttachments[9].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(10), *e10_type));
    typesAttachments[10].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(11), *e11_type));
    typesAttachments[11].reset(new ButtonAttachment(valueTreeState, TYPE_NAME + std::to_string(12), *e12_type));

    activeAttachments[0].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(1), *e1_active));
    activeAttachments[1].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(2), *e2_active));
    activeAttachments[2].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(3), *e3_active));
    activeAttachments[3].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(4), *e4_active));
    activeAttachments[4].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(5), *e5_active));
    activeAttachments[5].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(6), *e6_active));
    activeAttachments[6].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(7), *e7_active));
    activeAttachments[7].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(8), *e8_active));
    activeAttachments[8].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(9), *e9_active));
    activeAttachments[9].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(10), *e10_active));
    activeAttachments[10].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(11), *e11_active));
    activeAttachments[11].reset(new ButtonAttachment(valueTreeState, ACTIVE_NAME + std::to_string(12), *e12_active));
    //[/UserPreSize]

    setSize (1500, 850);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

PluginEditor::~PluginEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    for (int i = 0; i < NUMBER_OF_ELEMENTS; ++ i)
    {
        magnitudesAttachments[i].reset();
        phasesAttachments[i].reset();
        typesAttachments[i].reset();
        activeAttachments[i].reset();
    }
    //[/Destructor_pre]

    frequency_response = nullptr;
    freq_response_label = nullptr;
    phase_response = nullptr;
    ph_response_label = nullptr;
    gaussian_plane = nullptr;
    m1_slider = nullptr;
    p1_slider = nullptr;
    m2_slider = nullptr;
    m3_slider = nullptr;
    m4_slider = nullptr;
    m5_slider = nullptr;
    elements_chain_label = nullptr;
    element1_label = nullptr;
    magnitudes_label = nullptr;
    phases_label = nullptr;
    zero_pole_label = nullptr;
    element2_label = nullptr;
    p2_slider = nullptr;
    active_label = nullptr;
    juce__label8 = nullptr;
    element3_label = nullptr;
    element4_label = nullptr;
    element5_label = nullptr;
    element6_label = nullptr;
    element7_label = nullptr;
    element8_label = nullptr;
    element9_label = nullptr;
    element10_label = nullptr;
    element11_label = nullptr;
    element12_label = nullptr;
    m6_slider = nullptr;
    m7_slider = nullptr;
    m8_slider = nullptr;
    m9_slider = nullptr;
    m10_slider = nullptr;
    m11_slider = nullptr;
    m12_slider = nullptr;
    p3_slider = nullptr;
    p4_slider = nullptr;
    p5_slider = nullptr;
    p6_slider = nullptr;
    p7_slider = nullptr;
    p8_slider = nullptr;
    p9_slider = nullptr;
    p10_slider = nullptr;
    p11_slider = nullptr;
    p12_slider = nullptr;
    e1_type = nullptr;
    e2_type = nullptr;
    e3_type = nullptr;
    e4_type = nullptr;
    e5_type = nullptr;
    e6_type = nullptr;
    e7_type = nullptr;
    e8_type = nullptr;
    e9_type = nullptr;
    e10_type = nullptr;
    e11_type = nullptr;
    e12_type = nullptr;
    e1_active = nullptr;
    e2_active = nullptr;
    e3_active = nullptr;
    e4_active = nullptr;
    e5_active = nullptr;
    e6_active = nullptr;
    e7_active = nullptr;
    e8_active = nullptr;
    e9_active = nullptr;
    e10_active = nullptr;
    e11_active = nullptr;
    e12_active = nullptr;
    gaussian_plane_label = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

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



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
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
                 fixedSize="1" initialWidth="1500" initialHeight="850">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="frequencyResponse" id="161cb81e63dc8e46" memberName="frequency_response"
                    virtualName="" explicitFocusOrder="0" pos="990 30 490 335" class="FrequencyResponse"
                    params="processor.getFilterSpectrum()"/>
  <LABEL name="Frequency response" id="4c8fffb65e845bfc" memberName="freq_response_label"
         virtualName="" explicitFocusOrder="0" pos="1150 374 181 24" edTextCol="ff000000"
         edBkgCol="0" labelText="FREQUENCY RESPONSE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Poppins"
         fontsize="20.0" kerning="0.02" bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="phaseResponse" id="c9a48273dec25832" memberName="phase_response"
                    virtualName="" explicitFocusOrder="0" pos="990 460 490 335" class="PhaseResponse"
                    params="processor.getFilterSpectrum()"/>
  <LABEL name="Phase response" id="6d08c4e421703ed5" memberName="ph_response_label"
         virtualName="" explicitFocusOrder="0" pos="1175 803 142 24" edTextCol="ff000000"
         edBkgCol="0" labelText="PHASE RESPONSE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Poppins"
         fontsize="20.0" kerning="0.02" bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="gaussianPlane" id="f84485816497c4e3" memberName="gaussian_plane"
                    virtualName="" explicitFocusOrder="0" pos="630 30 335 335" class="GaussianPlane"
                    params="processor.getFilterElementsChain()"/>
  <SLIDER name="Element 1 magnitude" id="9107cda7959bc0ad" memberName="m1_slider"
          virtualName="" explicitFocusOrder="0" pos="150 71 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 1 phase" id="b9f95ed5c32caef9" memberName="p1_slider"
          virtualName="" explicitFocusOrder="0" pos="290 71 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 2 magnitude" id="699bb57e2da99f69" memberName="m2_slider"
          virtualName="" explicitFocusOrder="0" pos="150 131 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 3 magnitude" id="b08e7c8fd40afede" memberName="m3_slider"
          virtualName="" explicitFocusOrder="0" pos="150 191 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 4 magnitude" id="3e371d795c6990dc" memberName="m4_slider"
          virtualName="" explicitFocusOrder="0" pos="150 251 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 5 magnitude" id="b2dea2d2de8e606d" memberName="m5_slider"
          virtualName="" explicitFocusOrder="0" pos="150 311 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <LABEL name="Elements chain" id="b1d4cbae9aac5aab" memberName="elements_chain_label"
         virtualName="" explicitFocusOrder="0" pos="15 20 116 24" edTextCol="ff000000"
         edBkgCol="0" labelText="&#10;ELEMENTS CHAIN" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Poppins"
         fontsize="17.0" kerning="0.02" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 1" id="3628ed1851767efb" memberName="element1_label"
         virtualName="" explicitFocusOrder="0" pos="38 71 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 1" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Poppins" fontsize="15.0" kerning="0.02"
         bold="0" italic="0" justification="36"/>
  <LABEL name="Magnitudes" id="f2206057c7f672c6" memberName="magnitudes_label"
         virtualName="" explicitFocusOrder="0" pos="164 20 90 24" edTextCol="ff000000"
         edBkgCol="0" labelText="MAGNITUDES" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Poppins" fontsize="17.0" kerning="0.02"
         bold="0" italic="0" justification="36"/>
  <LABEL name="Phases" id="e0e34f187bbbdcdc" memberName="phases_label"
         virtualName="" explicitFocusOrder="0" pos="314 20 62 24" edTextCol="ff000000"
         edBkgCol="0" labelText="PHASES" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Poppins" fontsize="17.0" kerning="0.02"
         bold="0" italic="0" justification="36"/>
  <LABEL name="Zero / Pole" id="676532893526fbc7" memberName="zero_pole_label"
         virtualName="" explicitFocusOrder="0" pos="424 20 91 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ZERO / POLE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Poppins"
         fontsize="17.0" kerning="0.02" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 2" id="af8d6de4fc4b6a11" memberName="element2_label"
         virtualName="" explicitFocusOrder="0" pos="38 131 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 2" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Poppins" fontsize="15.0" kerning="0.02"
         bold="0" italic="0" justification="36"/>
  <SLIDER name="Element 2 phase" id="8cb0982862bc37e4" memberName="p2_slider"
          virtualName="" explicitFocusOrder="0" pos="290 131 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <LABEL name="Active" id="e82d0bcbcd7af0" memberName="active_label" virtualName=""
         explicitFocusOrder="0" pos="544 20 55 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ACTIVE" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Poppins" fontsize="17.0" kerning="0.02"
         bold="0" italic="0" justification="36"/>
  <LABEL name="new label" id="787fd92e44847890" memberName="juce__label8"
         virtualName="" explicitFocusOrder="0" pos="38 191 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 3" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 3" id="906ed592f45dce2f" memberName="element3_label"
         virtualName="" explicitFocusOrder="0" pos="38 191 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 3" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 4" id="f147064a90a33be8" memberName="element4_label"
         virtualName="" explicitFocusOrder="0" pos="38 251 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 4" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 5" id="886ad072eb80f56c" memberName="element5_label"
         virtualName="" explicitFocusOrder="0" pos="38 311 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 5" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 6" id="80f5db5f40a49d28" memberName="element6_label"
         virtualName="" explicitFocusOrder="0" pos="38 371 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 6" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 7" id="8bd43117312e4da7" memberName="element7_label"
         virtualName="" explicitFocusOrder="0" pos="38 431 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 7" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 8" id="be4672030a5e5534" memberName="element8_label"
         virtualName="" explicitFocusOrder="0" pos="38 491 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 8" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 9" id="f6c90fe26bf087c3" memberName="element9_label"
         virtualName="" explicitFocusOrder="0" pos="38 551 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 9" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 10" id="ed3fc40146c04b10" memberName="element10_label"
         virtualName="" explicitFocusOrder="0" pos="38 611 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 10" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 11" id="e3a5b4a8785ec108" memberName="element11_label"
         virtualName="" explicitFocusOrder="0" pos="38 671 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 11" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <LABEL name="Element 12" id="1b95fdd79ef6b468" memberName="element12_label"
         virtualName="" explicitFocusOrder="0" pos="38 731 70 24" edTextCol="ff000000"
         edBkgCol="0" labelText="ELEMENT 12" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="12.0"
         kerning="0.0" bold="0" italic="0" justification="36"/>
  <SLIDER name="Element 6 magnitude" id="7a079132a8702ba4" memberName="m6_slider"
          virtualName="" explicitFocusOrder="0" pos="150 371 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 7 magnitude" id="8d2a139c27fc609" memberName="m7_slider"
          virtualName="" explicitFocusOrder="0" pos="150 431 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 8 magnitude" id="ac450bbbbd6c4725" memberName="m8_slider"
          virtualName="" explicitFocusOrder="0" pos="150 491 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 9 magnitude" id="7d6e778c5b16f466" memberName="m9_slider"
          virtualName="" explicitFocusOrder="0" pos="150 551 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 10 magnitude" id="cf241975fd79a900" memberName="m10_slider"
          virtualName="" explicitFocusOrder="0" pos="150 611 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 11 magnitude" id="2f47546b6d97c414" memberName="m11_slider"
          virtualName="" explicitFocusOrder="0" pos="150 671 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 12 magnitude" id="66d9a904240d550e" memberName="m12_slider"
          virtualName="" explicitFocusOrder="0" pos="150 731 120 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 3 phase" id="f51ba826593a2a4e" memberName="p3_slider"
          virtualName="" explicitFocusOrder="0" pos="290 191 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 4 phase" id="1b9bdd01d2bca60a" memberName="p4_slider"
          virtualName="" explicitFocusOrder="0" pos="290 251 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 5 phase" id="d5f5e23eb0ca7d71" memberName="p5_slider"
          virtualName="" explicitFocusOrder="0" pos="290 311 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 6 phase" id="75116789633c614" memberName="p6_slider"
          virtualName="" explicitFocusOrder="0" pos="290 371 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 7 phase" id="74b1a3eb3bbef581" memberName="p7_slider"
          virtualName="" explicitFocusOrder="0" pos="290 431 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 8 phase" id="7525cf6a8952b731" memberName="p8_slider"
          virtualName="" explicitFocusOrder="0" pos="290 491 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 9 phase" id="7304c296874cbca3" memberName="p9_slider"
          virtualName="" explicitFocusOrder="0" pos="290 551 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 10 phase" id="bb13454c1eb2ebc8" memberName="p10_slider"
          virtualName="" explicitFocusOrder="0" pos="290 611 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 11 phase" id="aab61a5f91de8fa1" memberName="p11_slider"
          virtualName="" explicitFocusOrder="0" pos="290 671 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <SLIDER name="Element 12 phase" id="ca0f3879f176f26" memberName="p12_slider"
          virtualName="" explicitFocusOrder="0" pos="290 731 118 40" min="0.0"
          max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <TOGGLEBUTTON name="Element 1 type" id="cb026ef1068db639" memberName="e1_type"
                virtualName="" explicitFocusOrder="0" pos="460 70 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 2 type" id="56addf679899dfb" memberName="e2_type"
                virtualName="" explicitFocusOrder="0" pos="460 130 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 3 type" id="5abda647a887dc8e" memberName="e3_type"
                virtualName="" explicitFocusOrder="0" pos="460 190 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 4 type" id="3136ab6a98246edb" memberName="e4_type"
                virtualName="" explicitFocusOrder="0" pos="460 250 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 5 type" id="4d6d7aa8e46a31c6" memberName="e5_type"
                virtualName="" explicitFocusOrder="0" pos="460 310 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 6 type" id="6a190298ad04abf" memberName="e6_type"
                virtualName="" explicitFocusOrder="0" pos="460 370 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 7 type" id="57fd1da9881e7a6a" memberName="e7_type"
                virtualName="" explicitFocusOrder="0" pos="460 430 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 8 type" id="a0b1086b7f717581" memberName="e8_type"
                virtualName="" explicitFocusOrder="0" pos="460 490 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 9 type" id="dafe526db1b167f9" memberName="e9_type"
                virtualName="" explicitFocusOrder="0" pos="460 550 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 10 type" id="2d062dcad4b0663" memberName="e10_type"
                virtualName="" explicitFocusOrder="0" pos="460 610 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 11 type" id="86cee1ca201dd195" memberName="e11_type"
                virtualName="" explicitFocusOrder="0" pos="460 670 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 12 type" id="da051b1d0f987089" memberName="e12_type"
                virtualName="" explicitFocusOrder="0" pos="460 730 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 1 active" id="84bb86bdbd9ccede" memberName="e1_active"
                virtualName="" explicitFocusOrder="0" pos="561 70 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 2 active" id="176e3fb309f12ef5" memberName="e2_active"
                virtualName="" explicitFocusOrder="0" pos="561 130 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 3 active" id="d6d9c0542e41b9c3" memberName="e3_active"
                virtualName="" explicitFocusOrder="0" pos="561 190 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 4 active" id="72d6ebba0023e28e" memberName="e4_active"
                virtualName="" explicitFocusOrder="0" pos="561 250 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 5 active" id="d8c38a2ce324a73d" memberName="e5_active"
                virtualName="" explicitFocusOrder="0" pos="561 310 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 6 active" id="1b42bda33e1c947c" memberName="e6_active"
                virtualName="" explicitFocusOrder="0" pos="561 370 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 7 active" id="3405e0694e2b063d" memberName="e7_active"
                virtualName="" explicitFocusOrder="0" pos="561 430 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 8 active" id="b60e0b9c2104a2bd" memberName="e8_active"
                virtualName="" explicitFocusOrder="0" pos="561 490 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 9 active" id="d1d96ca0950dde6a" memberName="e9_active"
                virtualName="" explicitFocusOrder="0" pos="561 550 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 10 active" id="663dbbc6ddde4163" memberName="e10_active"
                virtualName="" explicitFocusOrder="0" pos="561 610 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 11 active" id="bf7163acf9aabb69" memberName="e11_active"
                virtualName="" explicitFocusOrder="0" pos="561 670 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="Element 12 active" id="c502882a7d65a176" memberName="e12_active"
                virtualName="" explicitFocusOrder="0" pos="561 730 22 24" buttonText=""
                connectedEdges="0" needsCallback="0" radioGroupId="0" state="0"/>
  <LABEL name="Gaussian plane" id="32980deccea32218" memberName="gaussian_plane_label"
         virtualName="" explicitFocusOrder="0" pos="727 373 141 24" edTextCol="ff000000"
         edBkgCol="0" labelText="GAUSSIAN PLANE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Poppins"
         fontsize="20.0" kerning="0.02" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

