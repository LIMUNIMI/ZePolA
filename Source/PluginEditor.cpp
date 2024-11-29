#include "Parameters.h"
#include <cmath>
#include <JuceHeader.h>

#include "PluginEditor.h"

EditorComponent::EditorComponent(PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts)
: processor(p), valueTreeState(vts)
{
    warning_label.reset(new juce::Label("Warning label", TRANS ("Caution! The current plugin configuration has caused an excessively high output, resulting in the audio stream being stopped. Please reset the plugin parameters to values that allow for a lower output volume.")));
    addAndMakeVisible(warning_label.get());
    warning_label->setFont (juce::Font ("Gill Sans", 18.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    warning_label->setJustificationType (juce::Justification::centred);
    warning_label->setEditable (false, false, false);
    warning_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    warning_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    warning_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));
    warning_label->setBounds (595, 370, 340, 120);
    warning_label->setVisible(false);
    
    p.setEditorCallback([this]()
                        {
        getFrequencyResponse();
        magnitude_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb);
        phase_response->updateValues(phases, referenceFrequencies, processor.getSampleRate(), true);
        updateElements();
        gaussian_plane->updateConjugate(processor.getFilterElementsChain());
    });
    
    warningRectangle.reset (new WarningRectangle());
    addAndMakeVisible(warningRectangle.get());
    warningRectangle->setBounds(525, 55, 480, 720);
    warningRectangle->setVisible(false);
    
    startTimer(50);

    selectable_filter_types = SELECTABLE_FILTER_TYPES;
    selectable_orders_butterworth = SELECTABLE_ORDERS_BUTTERWORTH;

    getFrequencyResponse();
    updateReferenceFrequencies();

    magnitudesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    phasesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    typesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    activeAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    gainsAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);

    e1_gain.reset (new CustomSlider ("Element 1 gain"));
    addAndMakeVisible (e1_gain.get());
    e1_gain->setRange (0, 10, 0);
    e1_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e1_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 30, 10);
    e1_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e1_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e1_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e1_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e1_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e1_gain->addListener (this);

    e1_gain->setBounds (310, 5, 97, 25);

    gaussian_plane.reset (new GaussianPlane (processor.getFilterElementsChain()));
    addAndMakeVisible (gaussian_plane.get());
    gaussian_plane->setName ("gaussianPlane");

    gaussian_plane->setBounds (30, 480, 260, 260);

    e2.reset (new DraggableElement (processor.getElementState(2), 2, gaussian_plane.get(), &processor
                                    ));
    addAndMakeVisible (e2.get());
    e2->setName ("Element 2");

    e2->setBounds (152, 602, 16, 16);

    e3.reset (new DraggableElement (processor.getElementState(3), 3, gaussian_plane.get(), &processor));
    addAndMakeVisible (e3.get());
    e3->setName ("Element 3");

    e3->setBounds (152, 602, 16, 16);

    e4.reset (new DraggableElement (processor.getElementState(4), 4, gaussian_plane.get(), &processor));
    addAndMakeVisible (e4.get());
    e4->setName ("Element 4");

    e4->setBounds (152, 602, 16, 16);

    e5.reset (new DraggableElement (processor.getElementState(5), 5, gaussian_plane.get(), &processor));
    addAndMakeVisible (e5.get());
    e5->setName ("Element 5");

    e5->setBounds (152, 602, 16, 16);

    e6.reset (new DraggableElement (processor.getElementState(6), 6, gaussian_plane.get(), &processor));
    addAndMakeVisible (e6.get());
    e6->setName ("Element 6");

    e6->setBounds (152, 602, 16, 16);

    e7.reset (new DraggableElement (processor.getElementState(7), 7, gaussian_plane.get(), &processor));
    addAndMakeVisible (e7.get());
    e7->setName ("Element 7");

    e7->setBounds (152, 602, 16, 16);

    e8.reset (new DraggableElement (processor.getElementState(8), 8, gaussian_plane.get(), &processor));
    addAndMakeVisible (e8.get());
    e8->setName ("Element 8");

    e8->setBounds (152, 602, 16, 16);

    e1.reset (new DraggableElement (processor.getElementState(1), 1, gaussian_plane.get(), &processor));
    addAndMakeVisible (e1.get());
    e1->setName ("Element 1");

    e1->setBounds (152, 602, 16, 16);

    passbandAmplitude_label.reset (new juce::Label ("Passband Amplitude",
                                                    TRANS ("PASSBAND AMPLITUDE (DB)")));
    addAndMakeVisible (passbandAmplitude_label.get());
    passbandAmplitude_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    passbandAmplitude_label->setJustificationType (juce::Justification::centred);
    passbandAmplitude_label->setEditable (false, false, false);
    passbandAmplitude_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    passbandAmplitude_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    passbandAmplitude_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    passbandAmplitude_label->setBounds (1020, 360, 159, 20);

    stopbandAmplitude_slider.reset (new CustomSlider ("Stopband Amplitude"));
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

    stopbandAmplitude_slider->setBounds (1022, 366, 135, 25);

    magnitude_response.reset (new MagnitudeResponse (magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb));
    addAndMakeVisible (magnitude_response.get());
    magnitude_response->setName ("frequencyResponse");

    magnitude_response->setBounds (540, 70, 450, 285);

    mg_response_label.reset (new juce::Label ("Magnitude response",
                                                TRANS ("MAGNITUDE RESPONSE\n")));
    addAndMakeVisible (mg_response_label.get());
    mg_response_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    mg_response_label->setJustificationType (juce::Justification::centred);
    mg_response_label->setEditable (false, false, false);
    mg_response_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    mg_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    mg_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    mg_response_label->setBounds (695, 360, 140, 24);

    phase_response.reset (new PhaseResponse (phases, referenceFrequencies, processor.getSampleRate(), ampDb));
    addAndMakeVisible (phase_response.get());
    phase_response->setName ("phaseResponse");

    phase_response->setBounds (540, 455, 450, 285);

    ph_response_label.reset (new juce::Label ("Phase response",
                                              TRANS ("PHASE RESPONSE")));
    addAndMakeVisible (ph_response_label.get());
    ph_response_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    ph_response_label->setJustificationType (juce::Justification::centred);
    ph_response_label->setEditable (false, false, false);
    ph_response_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    ph_response_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    ph_response_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    ph_response_label->setBounds (710, 745, 110, 24);

    m1_slider.reset (new CustomSlider ("Element 1 magnitude"));
    addAndMakeVisible (m1_slider.get());
    m1_slider->setRange (0, 10, 0);
    m1_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m1_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m1_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    m1_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    m1_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    m1_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    m1_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m1_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m1_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m1_slider->addListener (this);

    m1_slider->setBounds (20, 92, 120, 25);

    p1_slider.reset (new CustomSlider ("Element 1 phase"));
    addAndMakeVisible (p1_slider.get());
    p1_slider->setRange (0, 10, 0);
    p1_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p1_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p1_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    p1_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    p1_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    p1_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    p1_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p1_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p1_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p1_slider->addListener (this);

    p1_slider->setBounds (144, 92, 120, 25);

    radius_label.reset (new juce::Label ("Radius",
                                             TRANS ("RADIUS\n")));
    addAndMakeVisible (radius_label.get());
    radius_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    radius_label->setJustificationType (juce::Justification::centred);
    radius_label->setEditable (false, false, false);
    radius_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    radius_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    radius_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    radius_label->setBounds (35, 57, 90, 24);

    angles_label.reset (new juce::Label ("Angle",
                                         TRANS ("ANGLE\n")));
    addAndMakeVisible (angles_label.get());
    angles_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    angles_label->setJustificationType (juce::Justification::centred);
    angles_label->setEditable (false, false, false);
    angles_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    angles_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    angles_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    angles_label->setBounds (195, 57, 80, 24);

    type_label.reset (new juce::Label ("Type",
                                            TRANS ("TYPE")));
    addAndMakeVisible (type_label.get());
    type_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    type_label->setJustificationType (juce::Justification::centred);
    type_label->setEditable (false, false, false);
    type_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    type_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    type_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    type_label->setBounds (310, 57, 91, 24);

    active_label.reset (new juce::Label ("Active",
                                         TRANS ("ACTIVE")));
    addAndMakeVisible (active_label.get());
    active_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    active_label->setJustificationType (juce::Justification::centred);
    active_label->setEditable (false, false, false);
    active_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    active_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    active_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    active_label->setBounds (401, 57, 55, 24);

    e1_type.reset (new CustomToggleButton ("Element 1 type"));
    addAndMakeVisible (e1_type.get());
    e1_type->setButtonText (juce::String());
    e1_type->addListener (this);

    e1_type->setBounds (331, 93, 52, 21);

    e2_type.reset (new CustomToggleButton ("Element 2 type"));
    addAndMakeVisible (e2_type.get());
    e2_type->setButtonText (juce::String());
    e2_type->addListener (this);

    e2_type->setBounds (331, 138, 52, 21);

    e3_type.reset (new CustomToggleButton ("Element 3 type"));
    addAndMakeVisible (e3_type.get());
    e3_type->setButtonText (juce::String());
    e3_type->addListener (this);

    e3_type->setBounds (331, 183, 52, 21);

    e4_type.reset (new CustomToggleButton ("Element 4 type"));
    addAndMakeVisible (e4_type.get());
    e4_type->setButtonText (juce::String());
    e4_type->addListener (this);

    e4_type->setBounds (331, 228, 52, 21);

    e5_type.reset (new CustomToggleButton ("Element 5 type"));
    addAndMakeVisible (e5_type.get());
    e5_type->setButtonText (juce::String());
    e5_type->addListener (this);

    e5_type->setBounds (331, 273, 52, 21);

    e6_type.reset (new CustomToggleButton ("Element 6 type"));
    addAndMakeVisible (e6_type.get());
    e6_type->setButtonText (juce::String());
    e6_type->addListener (this);

    e6_type->setBounds (331, 318, 52, 21);

    e7_type.reset (new CustomToggleButton ("Element 7 type"));
    addAndMakeVisible (e7_type.get());
    e7_type->setButtonText (juce::String());
    e7_type->addListener (this);

    e7_type->setBounds (331, 363, 52, 21);

    e8_type.reset (new CustomToggleButton ("Element 8 type"));
    addAndMakeVisible (e8_type.get());
    e8_type->setButtonText (juce::String());
    e8_type->addListener (this);

    e8_type->setBounds (331, 408, 52, 21);

    e1_active.reset (new CustomToggleButton ("Element 1 active"));
    addAndMakeVisible (e1_active.get());
    e1_active->setButtonText (juce::String());
    e1_active->addListener (this);

    e1_active->setBounds (404, 93, 49, 21);

    e2_active.reset (new CustomToggleButton ("Element 2 active"));
    addAndMakeVisible (e2_active.get());
    e2_active->setButtonText (juce::String());
    e2_active->addListener (this);

    e2_active->setBounds (404, 138, 49, 21);

    e3_active.reset (new CustomToggleButton ("Element 3 active"));
    addAndMakeVisible (e3_active.get());
    e3_active->setButtonText (juce::String());
    e3_active->addListener (this);

    e3_active->setBounds (404, 183, 49, 21);

    e4_active.reset (new CustomToggleButton ("Element 4 active"));
    addAndMakeVisible (e4_active.get());
    e4_active->setButtonText (juce::String());
    e4_active->addListener (this);

    e4_active->setBounds (404, 228, 49, 21);

    e5_active.reset (new CustomToggleButton ("Element 5 active"));
    addAndMakeVisible (e5_active.get());
    e5_active->setButtonText (juce::String());
    e5_active->addListener (this);

    e5_active->setBounds (404, 273, 49, 21);

    e6_active.reset (new CustomToggleButton ("Element 6 active"));
    addAndMakeVisible (e6_active.get());
    e6_active->setButtonText (juce::String());
    e6_active->addListener (this);

    e6_active->setBounds (404, 318, 49, 21);

    e7_active.reset (new CustomToggleButton ("Element 7 active"));
    addAndMakeVisible (e7_active.get());
    e7_active->setButtonText (juce::String());
    e7_active->addListener (this);

    e7_active->setBounds (404, 363, 49, 21);

    e8_active.reset (new CustomToggleButton ("Element 8 active"));
    addAndMakeVisible (e8_active.get());
    e8_active->setButtonText (juce::String());
    e8_active->addListener (this);

    e8_active->setBounds (404, 408, 49, 21);

    gaussian_plane_label.reset (new juce::Label ("Gaussian plane",
                                                 TRANS ("GAUSSIAN PLANE")));
    addAndMakeVisible (gaussian_plane_label.get());
    gaussian_plane_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    gaussian_plane_label->setJustificationType (juce::Justification::centred);
    gaussian_plane_label->setEditable (false, false, false);
    gaussian_plane_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    gaussian_plane_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gaussian_plane_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    gaussian_plane_label->setBounds (95, 745, 140, 24);

    bypass.reset (new CustomToggleButton ("Bypass"));
    addAndMakeVisible (bypass.get());
    bypass->setButtonText (juce::String());

    bypass->setBounds (1067, 712, 60, 25);

    masterGain_slider.reset (new CustomSlider ("Master Gain"));
    addAndMakeVisible (masterGain_slider.get());
    masterGain_slider->setRange (0, 10, 0);
    masterGain_slider->setSkewFactor(9000, true);
    masterGain_slider->setSliderStyle (juce::Slider::LinearVertical);
    masterGain_slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    masterGain_slider->setColour (juce::Slider::thumbColourId, juce::Colours::orange);
    masterGain_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    masterGain_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    masterGain_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x008e989b));

    masterGain_slider->setBounds (1074, 540, 45, 160);

    linLog_switch.reset (new CustomToggleButton ("Linear / Logarithmic"));
    addAndMakeVisible (linLog_switch.get());
    linLog_switch->setButtonText (juce::String());
    linLog_switch->addListener (this);
    linLog_switch->setBounds (931, 362, 52, 21);
    
    m2_slider.reset (new CustomSlider ("Element 2 magnitude"));
    addAndMakeVisible (m2_slider.get());
    m2_slider->setRange (0, 10, 0);
    m2_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m2_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m2_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    m2_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    m2_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    m2_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    m2_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m2_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m2_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m2_slider->addListener (this);

    m2_slider->setBounds (20, 137, 120, 25);

    m3_slider.reset (new CustomSlider ("Element 3 magnitude"));
    addAndMakeVisible (m3_slider.get());
    m3_slider->setRange (0, 10, 0);
    m3_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m3_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m3_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    m3_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    m3_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    m3_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    m3_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m3_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m3_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m3_slider->addListener (this);

    m3_slider->setBounds (20, 182, 120, 25);

    m4_slider.reset (new CustomSlider ("Element 4 magnitude"));
    addAndMakeVisible (m4_slider.get());
    m4_slider->setRange (0, 10, 0);
    m4_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    m4_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    m4_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    m4_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    m4_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    m4_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    m4_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    m4_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    m4_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    m4_slider->addListener (this);

    m4_slider->setBounds (20, 227, 120, 25);

    m5_slider.reset (new CustomSlider ("Element 5 magnitude"));
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

    m5_slider->setBounds (20, 272, 120, 25);

    m6_slider.reset (new CustomSlider ("Element 6 magnitude"));
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

    m6_slider->setBounds (20, 317, 120, 25);

    m7_slider.reset (new CustomSlider ("Element 7 magnitude"));
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

    m7_slider->setBounds (20, 362, 120, 25);

    m8_slider.reset (new CustomSlider ("Element 8 magnitude"));
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

    m8_slider->setBounds (20, 407, 120, 25);

    p2_slider.reset (new CustomSlider ("Element 2 phase"));
    addAndMakeVisible (p2_slider.get());
    p2_slider->setRange (0, 10, 0);
    p2_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p2_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p2_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    p2_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    p2_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    p2_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    p2_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p2_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p2_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p2_slider->addListener (this);

    p2_slider->setBounds (144, 137, 120, 25);

    p3_slider.reset (new CustomSlider ("Element 3 phase"));
    addAndMakeVisible (p3_slider.get());
    p3_slider->setRange (0, 10, 0);
    p3_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p3_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p3_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    p3_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    p3_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    p3_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    p3_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p3_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p3_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p3_slider->addListener (this);

    p3_slider->setBounds (144, 182, 120, 25);

    p4_slider.reset (new CustomSlider ("Element 4 phase"));
    addAndMakeVisible (p4_slider.get());
    p4_slider->setRange (0, 10, 0);
    p4_slider->setSliderStyle (juce::Slider::LinearHorizontal);
    p4_slider->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    p4_slider->setColour (juce::Slider::backgroundColourId, juce::Colour (0xff383838));
    p4_slider->setColour (juce::Slider::thumbColourId, juce::Colour (0xff909497));
    p4_slider->setColour (juce::Slider::trackColourId, juce::Colour (0xff383838));
    p4_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff383838));
    p4_slider->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    p4_slider->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    p4_slider->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    p4_slider->addListener (this);

    p4_slider->setBounds (144, 227, 120, 25);

    p5_slider.reset (new CustomSlider ("Element 5 phase"));
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

    p5_slider->setBounds (144, 272, 120, 25);

    p6_slider.reset (new CustomSlider ("Element 6 phase"));
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

    p6_slider->setBounds (144, 317, 120, 25);

    p7_slider.reset (new CustomSlider ("Element 7 phase"));
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

    p7_slider->setBounds (144, 362, 120, 25);

    p8_slider.reset (new CustomSlider ("Element 8 phase"));
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

    p8_slider->setBounds (144, 407, 120, 25);

    p1_freq.reset (new CustomLabel ("Element 1 frequency",
                                    juce::String()));
    addAndMakeVisible (p1_freq.get());
    p1_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p1_freq->setJustificationType (juce::Justification::centredLeft);
    p1_freq->setEditable (true, true, false);
    p1_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p1_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p1_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p1_freq->addListener (this);

    p1_freq->setBounds (265, 92, 60, 25);

    p2_freq.reset (new CustomLabel ("Element 2 frequency",
                                    juce::String()));
    addAndMakeVisible (p2_freq.get());
    p2_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p2_freq->setJustificationType (juce::Justification::centredLeft);
    p2_freq->setEditable (true, true, false);
    p2_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p2_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p2_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p2_freq->addListener (this);

    p2_freq->setBounds (265, 137, 60, 25);

    p3_freq.reset (new CustomLabel ("Element 3 frequency",
                                    juce::String()));
    addAndMakeVisible (p3_freq.get());
    p3_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p3_freq->setJustificationType (juce::Justification::centredLeft);
    p3_freq->setEditable (true, true, false);
    p3_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p3_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p3_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p3_freq->addListener (this);

    p3_freq->setBounds (265, 182, 60, 25);

    p4_freq.reset (new CustomLabel ("Element 4 frequency",
                                    juce::String()));
    addAndMakeVisible (p4_freq.get());
    p4_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p4_freq->setJustificationType (juce::Justification::centredLeft);
    p4_freq->setEditable (true, true, false);
    p4_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p4_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p4_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p4_freq->addListener (this);

    p4_freq->setBounds (265, 227, 60, 25);

    p5_freq.reset (new CustomLabel ("Element 5 frequency",
                                    juce::String()));
    addAndMakeVisible (p5_freq.get());
    p5_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p5_freq->setJustificationType (juce::Justification::centredLeft);
    p5_freq->setEditable (true, true, false);
    p5_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p5_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p5_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p5_freq->addListener (this);

    p5_freq->setBounds (265, 272, 60, 25);

    p6_freq.reset (new CustomLabel ("Element 6 frequency",
                                    juce::String()));
    addAndMakeVisible (p6_freq.get());
    p6_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p6_freq->setJustificationType (juce::Justification::centredLeft);
    p6_freq->setEditable (true, true, false);
    p6_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p6_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p6_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p6_freq->addListener (this);

    p6_freq->setBounds (265, 317, 60, 25);

    p7_freq.reset (new CustomLabel ("Element 7 frequency",
                                    juce::String()));
    addAndMakeVisible (p7_freq.get());
    p7_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p7_freq->setJustificationType (juce::Justification::centredLeft);
    p7_freq->setEditable (true, true, false);
    p7_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p7_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p7_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p7_freq->addListener (this);

    p7_freq->setBounds (265, 362, 60, 25);

    p8_freq.reset (new CustomLabel ("Element 8 frequency",
                                    juce::String()));
    addAndMakeVisible (p8_freq.get());
    p8_freq->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    p8_freq->setJustificationType (juce::Justification::centredLeft);
    p8_freq->setEditable (true, true, false);
    p8_freq->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    p8_freq->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    p8_freq->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    p8_freq->addListener (this);

    p8_freq->setBounds (265, 407, 60, 25);

    type_box.reset (new CustomComboBox ("Design type"));
    addAndMakeVisible (type_box.get());
    type_box->setEditableText (false);
    type_box->setJustificationType (juce::Justification::centredLeft);
    type_box->setTextWhenNothingSelected (TRANS ("TYPE"));
    type_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    type_box->addListener (this);

    type_box->setBounds (1025, 145, 140, 25);

    shape_box.reset (new CustomComboBox ("Design shape"));
    addAndMakeVisible (shape_box.get());
    shape_box->setEditableText (false);
    shape_box->setJustificationType (juce::Justification::centredLeft);
    shape_box->setTextWhenNothingSelected (TRANS ("SHAPE"));
    shape_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    shape_box->addItem (TRANS ("LOWPASS"), 1);
    shape_box->addItem (TRANS ("HIGHPASS"), 2);
    shape_box->addListener (this);

    shape_box->setBounds (1025, 100, 140, 25);

    calculate_button.reset (new CustomButton ("Calculate"));
    addAndMakeVisible (calculate_button.get());
    calculate_button->setButtonText (juce::String());
    calculate_button->addListener (this);
    calculate_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    calculate_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    calculate_button->setBounds (1098, 482, 80, 30);

    multiply_phases_button.reset (new CustomButton ("Multiply phases"));
    addAndMakeVisible (multiply_phases_button.get());
    multiply_phases_button->setButtonText (juce::String());
    multiply_phases_button->addListener (this);
    multiply_phases_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    multiply_phases_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    multiply_phases_button->setBounds (328, 594, 80, 25);

    divide_phases_button.reset (new CustomButton ("Divide phases"));
    addAndMakeVisible (divide_phases_button.get());
    divide_phases_button->setButtonText (juce::String());
    divide_phases_button->addListener (this);
    divide_phases_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    divide_phases_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff727272));

    divide_phases_button->setBounds (421, 594, 80, 25);

    swap_button.reset (new CustomButton ("Swap poles/zeros"));
    addAndMakeVisible (swap_button.get());
    swap_button->setButtonText (juce::String());
    swap_button->addListener (this);
    swap_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff909497));
    swap_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    swap_button->setBounds (328, 664, 80, 25);

    turn_on_button.reset (new CustomButton ("Turn on all the elements"));
    addAndMakeVisible (turn_on_button.get());
    turn_on_button->setButtonText (juce::String());
    turn_on_button->addListener (this);
    turn_on_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff73cc81));
    turn_on_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    turn_on_button->setBounds (328, 531, 80, 25);

    turn_off_button.reset (new CustomButton ("Turn off all the elements"));
    addAndMakeVisible (turn_off_button.get());
    turn_off_button->setButtonText (juce::String());
    turn_off_button->addListener (this);
    turn_off_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0xffe86d5c));
    turn_off_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    turn_off_button->setBounds (421, 531, 80, 25);

    order_box.reset (new CustomComboBox ("Design order"));
    addAndMakeVisible (order_box.get());
    order_box->setEditableText (false);
    order_box->setJustificationType (juce::Justification::centredLeft);
    order_box->setTextWhenNothingSelected (TRANS ("ORDER"));
    order_box->setTextWhenNoChoicesAvailable (TRANS ("(no choices)"));
    order_box->addListener (this);

    order_box->setBounds (1025, 190, 140, 25);

    design_frequency_label.reset (new juce::Label ("Design frequency",
                                                   TRANS ("FREQUENCY\n")));
    addAndMakeVisible (design_frequency_label.get());
    design_frequency_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    design_frequency_label->setJustificationType (juce::Justification::centred);
    design_frequency_label->setEditable (false, false, false);
    design_frequency_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    design_frequency_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    design_frequency_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    design_frequency_label->setBounds (1063, 228, 72, 20);

    ampDb_switch.reset (new CustomToggleButton ("Amplitude / dB"));
    addAndMakeVisible (ampDb_switch.get());
    ampDb_switch->setButtonText (juce::String());
    ampDb_switch->addListener (this);

    ampDb_switch->setBounds (543, 362, 52, 21);

    frequency_design_slider.reset (new CustomSlider ("Frequency design slider"));
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

    frequency_design_slider->setBounds (1022, 250, 83, 25);

    frequency_label.reset (new CustomLabel ("Frequency Label",
                                            juce::String()));
    addAndMakeVisible (frequency_label.get());
    frequency_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    frequency_label->setJustificationType (juce::Justification::centredLeft);
    frequency_label->setEditable (true, true, false);
    frequency_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    frequency_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    frequency_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    frequency_label->addListener (this);

    frequency_label->setBounds (1110, 250, 60, 25);

    transition_width_label.reset (new juce::Label ("Transition width",
                                                   TRANS ("TRANSITION WIDTH")));
    addAndMakeVisible (transition_width_label.get());
    transition_width_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    transition_width_label->setJustificationType (juce::Justification::centred);
    transition_width_label->setEditable (false, false, false);
    transition_width_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    transition_width_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    transition_width_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    transition_width_label->setBounds (1031, 291, 129, 20);

    transition_width_slider.reset (new CustomSlider ("Transition width slider"));
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

    transition_width_slider->setBounds (1022, 315, 135, 25);

    passbandAmplitude_slider.reset (new CustomSlider ("Passband Amplitude"));
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

    passbandAmplitude_slider->setBounds (1022, 379, 135, 25);

    stopbandAmplitude_label.reset (new juce::Label ("Stopband Amplitude",
                                                    TRANS ("STOPBAND AMPLITUDE (DB)")));
    addAndMakeVisible (stopbandAmplitude_label.get());
    stopbandAmplitude_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    stopbandAmplitude_label->setJustificationType (juce::Justification::centred);
    stopbandAmplitude_label->setEditable (false, false, false);
    stopbandAmplitude_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    stopbandAmplitude_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    stopbandAmplitude_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    stopbandAmplitude_label->setBounds (1020, 337, 159, 20);

    autoUpdate_button.reset (new CustomToggleButton ("Auto update"));
    addAndMakeVisible (autoUpdate_button.get());
    autoUpdate_button->setButtonText (juce::String());
    autoUpdate_button->addListener (this);

    autoUpdate_button->setBounds (1013, 482, 80, 30);

    undo_button.reset (new CustomButton ("Undo"));
    addAndMakeVisible (undo_button.get());
    undo_button->setButtonText (juce::String());
    undo_button->addListener (this);
    undo_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00909497));
    undo_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    undo_button->setBounds (18, 9, 60, 25);

    redo_button.reset (new CustomButton ("Redo"));
    addAndMakeVisible (redo_button.get());
    redo_button->setButtonText (juce::String());
    redo_button->addListener (this);
    redo_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00909497));
    redo_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    redo_button->setBounds (91, 9, 60, 25);
    
    reset_button.reset (new CustomButton ("Reset"));
    addAndMakeVisible (reset_button.get());
    reset_button->setButtonText (juce::String());
    reset_button->addListener (this);
    reset_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00909497));
    reset_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    reset_button->setBounds (166, 9, 120, 25);

    saveCoefficients_button.reset (new CustomButton ("Save coefficients"));
    addAndMakeVisible (saveCoefficients_button.get());
    saveCoefficients_button->setButtonText (juce::String());
    saveCoefficients_button->addListener (this);
    saveCoefficients_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00909497));
    saveCoefficients_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    saveCoefficients_button->setBounds (830, 9, 130, 25);
    
    save_preset_button.reset (new CustomButton ("Save preset"));
    addAndMakeVisible (save_preset_button.get());
    save_preset_button->setButtonText (juce::String());
    save_preset_button->addListener (this);
    save_preset_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00909497));
    save_preset_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    save_preset_button->setBounds (971, 9, 100, 25);

    load_preset_button.reset (new CustomButton ("Load preset"));
    addAndMakeVisible (load_preset_button.get());
    load_preset_button->setButtonText (juce::String());
    load_preset_button->addListener (this);
    load_preset_button->setColour (juce::TextButton::buttonColourId, juce::Colour (0x00909497));
    load_preset_button->setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xff505050));

    load_preset_button->setBounds (1082, 9, 100, 25);

    gain_label.reset (new juce::Label ("Gain",
                                          TRANS ("GAIN")));
    addAndMakeVisible (gain_label.get());
    gain_label->setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
    gain_label->setJustificationType (juce::Justification::centred);
    gain_label->setEditable (false, false, false);
    gain_label->setColour (juce::Label::textColourId, juce::Colour (0xff383838));
    gain_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain_label->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    gain_label->setBounds (464, 57, 55, 24);

    autoGain.reset (new CustomToggleButton ("Auto Gain"));
    addAndMakeVisible (autoGain.get());
    autoGain->setButtonText (juce::String());
    autoGain->addListener (this);

    autoGain->setBounds (462, 445, 49, 21);

    e2_gain.reset (new CustomSlider ("Element 2 gain"));
    addAndMakeVisible (e2_gain.get());
    e2_gain->setRange (0, 10, 0);
    e2_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e2_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e2_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e2_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e2_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e2_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e2_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e2_gain->addListener (this);

    e2_gain->setBounds (310, 5, 97, 25);

    e3_gain.reset (new CustomSlider ("Element 3 gain"));
    addAndMakeVisible (e3_gain.get());
    e3_gain->setRange (0, 10, 0);
    e3_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e3_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e3_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e3_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e3_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e3_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e3_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e3_gain->addListener (this);

    e3_gain->setBounds (310, 8, 97, 25);

    e4_gain.reset (new CustomSlider ("Element 4 gain"));
    addAndMakeVisible (e4_gain.get());
    e4_gain->setRange (0, 10, 0);
    e4_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e4_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e4_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e4_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e4_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e4_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e4_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e4_gain->addListener (this);

    e4_gain->setBounds (310, 8, 97, 25);

    e5_gain.reset (new CustomSlider ("Element 5 gain"));
    addAndMakeVisible (e5_gain.get());
    e5_gain->setRange (0, 10, 0);
    e5_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e5_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e5_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e5_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e5_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e5_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e5_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e5_gain->addListener (this);

    e5_gain->setBounds (310, 8, 97, 25);

    e6_gain.reset (new CustomSlider ("Element 6 gain"));
    addAndMakeVisible (e6_gain.get());
    e6_gain->setRange (0, 10, 0);
    e6_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e6_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e6_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e6_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e6_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e6_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e6_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e6_gain->addListener (this);

    e6_gain->setBounds (310, 8, 97, 25);

    e7_gain.reset (new CustomSlider ("Element 7 gain"));
    addAndMakeVisible (e7_gain.get());
    e7_gain->setRange (0, 10, 0);
    e7_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e7_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e7_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e7_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e7_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e7_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e7_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e7_gain->addListener (this);

    e7_gain->setBounds (310, 8, 97, 25);

    e8_gain.reset (new CustomSlider ("Element 8 gain"));
    addAndMakeVisible (e8_gain.get());
    e8_gain->setRange (0, 10, 0);
    e8_gain->setSliderStyle (juce::Slider::LinearHorizontal);
    e8_gain->setTextBoxStyle (juce::Slider::TextBoxRight, false, 50, 20);
    e8_gain->setColour (juce::Slider::thumbColourId, juce::Colours::white);
    e8_gain->setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xff333333));
    e8_gain->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x00000000));
    e8_gain->setColour (juce::Slider::textBoxHighlightColourId, juce::Colour (0x66686868));
    e8_gain->setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0x00000000));
    e8_gain->addListener (this);

    e8_gain->setBounds (310, 8, 97, 25);

    gain1_label.reset (new DraggableGainLabel ("Element 1 Gain",
                                        juce::String()));
    addAndMakeVisible (gain1_label.get());
    gain1_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain1_label->setJustificationType (juce::Justification::centredLeft);
    gain1_label->setEditable (true, true, false);
    gain1_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain1_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain1_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain1_label->addListener (this);

    gain1_label->setBounds (467, 92, 50, 25);

    gain2_label.reset (new DraggableGainLabel ("Element 2 Gain",
                                        juce::String()));
    addAndMakeVisible (gain2_label.get());
    gain2_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain2_label->setJustificationType (juce::Justification::centredLeft);
    gain2_label->setEditable (true, true, false);
    gain2_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain2_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain2_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain2_label->addListener (this);

    gain2_label->setBounds (467, 137, 50, 25);

    gain3_label.reset (new DraggableGainLabel ("Element 3 Gain",
                                        juce::String()));
    addAndMakeVisible (gain3_label.get());
    gain3_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain3_label->setJustificationType (juce::Justification::centredLeft);
    gain3_label->setEditable (true, true, false);
    gain3_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain3_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain3_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain3_label->addListener (this);

    gain3_label->setBounds (467, 182, 50, 25);

    gain4_label.reset (new DraggableGainLabel ("Element 4 Gain",
                                        juce::String()));
    addAndMakeVisible (gain4_label.get());
    gain4_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain4_label->setJustificationType (juce::Justification::centredLeft);
    gain4_label->setEditable (true, true, false);
    gain4_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain4_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain4_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain4_label->addListener (this);

    gain4_label->setBounds (467, 227, 50, 25);

    gain5_label.reset (new DraggableGainLabel ("Element 5 Gain",
                                        juce::String()));
    addAndMakeVisible (gain5_label.get());
    gain5_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain5_label->setJustificationType (juce::Justification::centredLeft);
    gain5_label->setEditable (true, true, false);
    gain5_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain5_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain5_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain5_label->addListener (this);

    gain5_label->setBounds (467, 272, 50, 25);

    gain6_label.reset (new DraggableGainLabel ("Element 6 Gain",
                                        juce::String()));
    addAndMakeVisible (gain6_label.get());
    gain6_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain6_label->setJustificationType (juce::Justification::centredLeft);
    gain6_label->setEditable (true, true, false);
    gain6_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain6_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain6_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain6_label->addListener (this);

    gain6_label->setBounds (467, 317, 50, 25);

    gain7_label.reset (new DraggableGainLabel ("Element 7 Gain",
                                        juce::String()));
    addAndMakeVisible (gain7_label.get());
    gain7_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain7_label->setJustificationType (juce::Justification::centredLeft);
    gain7_label->setEditable (true, true, false);
    gain7_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain7_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain7_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain7_label->addListener (this);

    gain7_label->setBounds (467, 362, 50, 25);

    gain8_label.reset (new DraggableGainLabel ("Element 8 Gain",
                                        juce::String()));
    addAndMakeVisible (gain8_label.get());
    gain8_label->setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    gain8_label->setJustificationType (juce::Justification::centredLeft);
    gain8_label->setEditable (true, true, false);
    gain8_label->setColour (juce::Label::textColourId, juce::Colour (0xff333333));
    gain8_label->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    gain8_label->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black);
    gain8_label->addListener (this);

    gain8_label->setBounds (467, 407, 50, 25);

    cachedImage_anticlockwise_arrow_png_1 = juce::ImageCache::getFromMemory (anticlockwise_arrow_png, anticlockwise_arrow_pngSize);
    cachedImage_clockwise_arrow_png_2 = juce::ImageCache::getFromMemory (clockwise_arrow_png, clockwise_arrow_pngSize);
    cachedImage_load_icon_png_3 = juce::ImageCache::getFromMemory (load_icon_png, load_icon_pngSize);
    cachedImage_save_icon_png_4 = juce::ImageCache::getFromMemory (save_icon_png, save_icon_pngSize);  

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

    gainsAttachments[0].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(1), *e1_gain));
    gainsAttachments[1].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(2), *e2_gain));
    gainsAttachments[2].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(3), *e3_gain));
    gainsAttachments[3].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(4), *e4_gain));
    gainsAttachments[4].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(5), *e5_gain));
    gainsAttachments[5].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(6), *e6_gain));
    gainsAttachments[6].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(7), *e7_gain));
    gainsAttachments[7].reset(new SliderAttachment(valueTreeState, GAIN_NAME + std::to_string(8), *e8_gain));

    masterGainAttachment.reset(new SliderAttachment(valueTreeState, MASTER_GAIN_NAME, *masterGain_slider));
    bypassAttachment.reset(new ButtonAttachment(valueTreeState, FILTER_BYPASS_NAME, *bypass));

    m1_slider->setLookAndFeel(&activeSliderTheme);
    m2_slider->setLookAndFeel(&activeSliderTheme);
    m3_slider->setLookAndFeel(&activeSliderTheme);
    m4_slider->setLookAndFeel(&activeSliderTheme);
    m5_slider->setLookAndFeel(&activeSliderTheme);
    m6_slider->setLookAndFeel(&activeSliderTheme);
    m7_slider->setLookAndFeel(&activeSliderTheme);
    m8_slider->setLookAndFeel(&activeSliderTheme);

    p1_slider->setLookAndFeel(&activeSliderTheme);
    p2_slider->setLookAndFeel(&activeSliderTheme);
    p3_slider->setLookAndFeel(&activeSliderTheme);
    p4_slider->setLookAndFeel(&activeSliderTheme);
    p5_slider->setLookAndFeel(&activeSliderTheme);
    p6_slider->setLookAndFeel(&activeSliderTheme);
    p7_slider->setLookAndFeel(&activeSliderTheme);
    p8_slider->setLookAndFeel(&activeSliderTheme);

    masterGain_slider->setLookAndFeel(&masterGainSliderTheme);

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

    juce::Slider* sliders[]={e1_gain.get(), e2_gain.get(), e3_gain.get(), e4_gain.get(), e5_gain.get(), e6_gain.get(), e7_gain.get(), e8_gain.get()};
    for (auto& slider : sliders)
        slider->setVisible(false);

    bypass->setLookAndFeel(&bypassSwitchTheme);

    autoGain->setLookAndFeel(&activeSwitchesTheme);

    calculateButtonTheme.setTextToDisplay("CALCULATE");
    calculateButtonTheme.setFontSize(12.0f);
    calculate_button->setLookAndFeel(&calculateButtonTheme);

    swapButtonTheme.setTextToDisplay("SWAP Ps/Zs");
    swap_button->setLookAndFeel(&swapButtonTheme);

    multiplyPhasesTheme.setTextToDisplay("DOUBLE PHASES");
    multiply_phases_button->setLookAndFeel(&multiplyPhasesTheme);

    dividePhasesTheme.setTextToDisplay("HALF PHASES");
    divide_phases_button->setLookAndFeel(&dividePhasesTheme);

    turnAllOnTheme.setTextToDisplay("ALL ON");
    turn_on_button->setLookAndFeel(&turnAllOnTheme);

    turnAllOffTheme.setTextToDisplay("ALL OFF");
    turn_off_button->setLookAndFeel(&turnAllOffTheme);

    undoButtonTheme.setButtonFunction(MenuButtonTheme::buttonFunction::UNDO);
    undo_button->setLookAndFeel(&undoButtonTheme);
    undo_button->repaint();

    redoButtonTheme.setButtonFunction(MenuButtonTheme::buttonFunction::REDO);
    redo_button->setLookAndFeel(&redoButtonTheme);
    redo_button->repaint();
    
    resetButtonTheme.setButtonFunction(MenuButtonTheme::buttonFunction::RESET);
    reset_button->setLookAndFeel(&resetButtonTheme);
    reset_button->repaint();
    
    saveCoefficientsButtonTheme.setButtonFunction(MenuButtonTheme::buttonFunction::SAVE_COEFFS);
    saveCoefficients_button->setLookAndFeel(&saveCoefficientsButtonTheme);
    saveCoefficients_button->repaint();

    savePresetButtonTheme.setButtonFunction(MenuButtonTheme::buttonFunction::SAVE_PRST);
    save_preset_button->setLookAndFeel(&savePresetButtonTheme);
    save_preset_button->repaint();

    loadPresetButtonTheme.setButtonFunction(MenuButtonTheme::buttonFunction::LOAD_PRST);
    load_preset_button->setLookAndFeel(&loadPresetButtonTheme);
    load_preset_button->repaint();

    type_box->setLookAndFeel(&comboBoxTheme);
    order_box->setLookAndFeel(&comboBoxTheme);
    shape_box->setLookAndFeel(&comboBoxTheme);

    frequency_design_slider->setLookAndFeel(&activeSliderTheme);
    frequency_design_slider->setSkewFactor(0.2);
    transition_width_slider->setLookAndFeel(&activeSliderTheme);
    passbandAmplitude_slider->setLookAndFeel(&activeSliderTheme);
    stopbandAmplitude_slider->setLookAndFeel(&activeSliderTheme);

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

    design_frequency_label->setBounds (1058, 248, 72, 20);
    frequency_design_slider->setBounds (1027, 270, 83, 25);
    frequency_label->setBounds (1110, 270, 60, 25);

    transition_width_label->setVisible(false);
    transition_width_slider->setVisible(false);

    passbandAmplitude_label->setVisible(false);
    passbandAmplitude_slider->setVisible(false);

    stopbandAmplitude_label->setVisible(false);
    stopbandAmplitude_slider->setVisible(false);

    calculate_button->setEnabled(false);

    updateDesignSliderFromFrequency(DESIGN_FREQUENCY_FLOOR, frequency_design_slider.get(), sampleRate);

    autoUpdate_button->setLookAndFeel(&autoUpdateSwitchTheme);

    updateElements();
    slidersInit();
    autoGain->setToggleState(true, NotificationType::sendNotification);
    gainsInit();
}

EditorComponent::~EditorComponent()
{
    for (int i = 0; i < NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        magnitudesAttachments[i].reset();
        phasesAttachments[i].reset();
        typesAttachments[i].reset();
        activeAttachments[i].reset();
        gainsAttachments[i].reset();
    }
    masterGainAttachment.reset();
    bypassAttachment.reset();
    
    stopTimer();
    
    juce::Slider* sliders[] =
    {
        m1_slider.get(), m2_slider.get(), m3_slider.get(), m4_slider.get(), m5_slider.get(), m6_slider.get(), m7_slider.get(), m8_slider.get(),
        p1_slider.get(), p2_slider.get(), p3_slider.get(), p4_slider.get(), p5_slider.get(), p6_slider.get(), p7_slider.get(), p8_slider.get(),
        e1_gain.get(), e2_gain.get(), e3_gain.get(), e4_gain.get(), e5_gain.get(), e6_gain.get(), e7_gain.get(), e8_gain.get(),
        frequency_design_slider.get(),
        stopbandAmplitude_slider.get(),
        passbandAmplitude_slider.get(),
        masterGain_slider.get(),
        transition_width_slider.get()
    };

    for (auto* slider : sliders)
    {
        slider->removeListener(this);
    }
    
    juce::Button* buttons[]=
    {
        reset_button.get(), calculate_button.get(), multiply_phases_button.get(), divide_phases_button.get(), swap_button.get(), turn_on_button.get(), turn_off_button.get(),
        undo_button.get(), redo_button.get(), saveCoefficients_button.get(), save_preset_button.get(), load_preset_button.get()
    };
    
    for (auto* button : buttons)
    {
        button->removeListener(this);
    }
    
    juce::ToggleButton* toggButtons[]=
    {
        e1_type.get(), e2_type.get(), e3_type.get(), e4_type.get(), e5_type.get(), e6_type.get(), e7_type.get(), e8_type.get(),
        e1_active.get(), e2_active.get(), e3_active.get(), e4_active.get(), e5_active.get(), e6_active.get(), e7_active.get(), e8_active.get(),
        linLog_switch.get(), ampDb_switch.get(), autoUpdate_button.get(),
        autoGain.get()
    };
    
    for (auto* toggButton : toggButtons)
    {
        toggButton->removeListener(this);
    }
    
    juce::Label* labels[]=
    {
        p1_freq.get(), p2_freq.get(), p3_freq.get(), p4_freq.get(), p5_freq.get(), p6_freq.get(), p7_freq.get(), p8_freq.get(),
        frequency_label.get(),
        gain1_label.get(), gain2_label.get(), gain3_label.get(), gain4_label.get(), gain5_label.get(), gain6_label.get(), gain7_label.get(), gain8_label.get()
    };
    
    for (auto* label : labels)
    {
        label->removeListener(this);
    }
    
    juce::ComboBox* boxes[]=
    {
        type_box.get(), shape_box.get(), order_box.get()
    };
    
    for (auto* box : boxes)
    {
        box->removeListener(this);
    }

    e1_gain = nullptr;
    gaussian_plane = nullptr;
    e2 = nullptr;
    e3 = nullptr;
    e4 = nullptr;
    e5 = nullptr;
    e6 = nullptr;
    e7 = nullptr;
    e8 = nullptr;
    e1 = nullptr;
    passbandAmplitude_label = nullptr;
    stopbandAmplitude_slider = nullptr;
    reset_button = nullptr;
    magnitude_response = nullptr;
    mg_response_label = nullptr;
    phase_response = nullptr;
    ph_response_label = nullptr;
    m1_slider = nullptr;
    p1_slider = nullptr;
    radius_label = nullptr;
    angles_label = nullptr;
    type_label = nullptr;
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
    masterGain_slider = nullptr;
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
    undo_button = nullptr;
    redo_button = nullptr;
    saveCoefficients_button = nullptr;
    save_preset_button = nullptr;
    load_preset_button = nullptr;
    gain_label = nullptr;
    autoGain = nullptr;
    e2_gain = nullptr;
    e3_gain = nullptr;
    e4_gain = nullptr;
    e5_gain = nullptr;
    e6_gain = nullptr;
    e7_gain = nullptr;
    e8_gain = nullptr;
    gain1_label = nullptr;
    gain2_label = nullptr;
    gain3_label = nullptr;
    gain4_label = nullptr;
    gain5_label = nullptr;
    gain6_label = nullptr;
    gain7_label = nullptr;
    gain8_label = nullptr;
    
    warningRectangle = nullptr;
    warning_label = nullptr;
}

void EditorComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xffecf0f1));

    {
        float x = 15.0f, y = 55.0f, width = 510.0f, height = 720.0f;
        juce::Colour fillColour = juce::Colour (0x17b1b1b1);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 1.500f);
    }
    
    {
        float x = 525.0f, y = 55.0f, width = 480.0f, height = 720.0f;
        juce::Colour fillColour = juce::Colour (0x17b1b1b1);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 1.500f);
    }

    {
        float x = 1005.0f, y = 55.0f, width = 180.0f, height = 720.0f;
        juce::Colour fillColour = juce::Colour (0x19656565);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 1.500f);
    }

    {
        int x = 15, y = 130, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 175, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 220, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 265, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 310, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 400, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 15, y = 355, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour (0x25909497);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 1040, y = 60, width = 110, height = 24;
        juce::String text (TRANS ("FILTER DESIGN"));
        juce::Colour fillColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 1075, y = 520, width = 40, height = 20;
        juce::String text (TRANS ("GAIN"));
        juce::Colour fillColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 10.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        float x = 320.0f, y = 480.0f, width = 190.0f, height = 260.0f;
        juce::Colour fillColour = juce::Colour (0x11b1b1b1);
        juce::Colour strokeColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 14.500f);
        g.setColour (strokeColour);
        g.drawRoundedRectangle (x, y, width, height, 14.500f, 0.500f);
    }

    {
        int x = 365, y = 485, width = 100, height = 20;
        juce::String text (TRANS ("SHORTCUTS"));
        juce::Colour fillColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 12.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 1120, y = 681, width = 23, height = 20;
        juce::String text (TRANS ("dB"));
        juce::Colour fillColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 13.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }

    {
        int x = 0, y = 40, width = 1200, height = 1;
        juce::Colour fillColour = juce::Colour (0x91383838);
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    {
        int x = 59, y = 16, width = 10, height = 10;
        g.setColour (juce::Colours::black);
        g.drawImageWithin (cachedImage_anticlockwise_arrow_png_1,
                           x, y, width, height,
                           juce::RectanglePlacement::centred,
                           false);
    }

    {
        int x = 128, y = 16, width = 10, height = 10;
        g.setColour (juce::Colours::black);
        g.drawImageWithin (cachedImage_clockwise_arrow_png_2,
                           x, y, width, height,
                           juce::RectanglePlacement::centred,
                           false);
    }

    {
        int x = 1161, y = 11, width = 23, height = 19;

        g.setColour (juce::Colours::black);
        g.drawImageWithin (cachedImage_load_icon_png_3,
                           x, y, width, height,
                           juce::RectanglePlacement::centred,
                           false);
    }

    {
        int x = 1047, y = 15, width = 17, height = 12;
        g.setColour (juce::Colours::black);
        g.drawImageWithin (cachedImage_save_icon_png_4,
                           x, y, width, height,
                           juce::RectanglePlacement::centred,
                           false);
    }

    {
        int x = 402, y = 445, width = 56, height = 20;
        juce::String text (TRANS ("AUTO GAIN"));
        juce::Colour fillColour = juce::Colour (0xff383838);
        g.setColour (fillColour);
        g.setFont (juce::Font ("Gill Sans", 10.00f, juce::Font::plain).withTypefaceStyle ("SemiBold"));
        g.drawText (text, x, y, width, height,
                    juce::Justification::centred, true);
    }
}

void EditorComponent::sliderValueChanged (juce::Slider* sliderThatWasMoved)
{
    const double sampleRate = processor.getSampleRate();

    if (sliderThatWasMoved == e1_gain.get())
    {
        gain1_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == stopbandAmplitude_slider.get())
    {
        stopbandAmplitudedB = sliderThatWasMoved->getValue();
        setTransitionWidthRange();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == m1_slider.get())
    {
        auto element = processor.getElementState(1);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(1);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(1)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p1_slider.get())
    {
        updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(1);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(1)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m2_slider.get())
    {
        auto element = processor.getElementState(2);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(2);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(2)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m3_slider.get())
    {
        auto element = processor.getElementState(3);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(3);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(3)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m4_slider.get())
    {
        auto element = processor.getElementState(4);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(4);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(4)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m5_slider.get())
    {
        auto element = processor.getElementState(5);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(5);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(5)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m6_slider.get())
    {
        auto element = processor.getElementState(6);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(6);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(6)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m7_slider.get())
    {
        auto element = processor.getElementState(7);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(7);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(7)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m8_slider.get())
    {
        auto element = processor.getElementState(8);
        if (element.getType() == FilterElement::POLE && sliderThatWasMoved->getValue() == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(8);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(8)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p2_slider.get())
    {
        updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(2);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(2)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p3_slider.get())
    {
        updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(3);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(3)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p4_slider.get())
    {
        updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(4);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(4)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p5_slider.get())
    {
        updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(5);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(5)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p6_slider.get())
    {
        updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(6);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(6)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p7_slider.get())
    {
        updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(7);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(7)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p8_slider.get())
    {
        updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(8);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(8)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == frequency_design_slider.get())
    {
        design_frequency = updateFrequencyFromDesignSlider(frequency_design_slider.get(), frequency_label.get(), sampleRate);
        if (type_box->getSelectedId() > 1)
            setTransitionWidthRange();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == transition_width_slider.get())
    {
        normalisedTransitionWidth = sliderThatWasMoved->getValue();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == passbandAmplitude_slider.get())
    {
        passbandAmplitudedB = sliderThatWasMoved->getValue();
        setTransitionWidthRange();

        if (autoUpdate)
            autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == e2_gain.get())
    {
        gain2_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e3_gain.get())
    {
        gain3_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e4_gain.get())
    {
        gain4_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e5_gain.get())
    {
        gain5_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e6_gain.get())
    {
        gain6_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e7_gain.get())
    {
        gain7_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e8_gain.get())
    {
        gain8_label->setText(juce::String(sliderThatWasMoved->getValue(), 1) + " dB", NotificationType::dontSendNotification);
    }
}

void EditorComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == reset_button.get())
    {
        processor.resetFilter();
    }
    else if (buttonThatWasClicked == e1_type.get())
    {
        auto element = processor.getElementState(1);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(1)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m1_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m1_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(1, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(1)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e2_type.get())
    {
        auto element = processor.getElementState(2);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(2)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m2_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m2_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(2, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(2)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e3_type.get())
    {
        auto element = processor.getElementState(3);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(3)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m3_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m3_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(3, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(3)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e4_type.get())
    {
        auto element = processor.getElementState(4);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(4)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m4_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m4_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(4, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(4)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e5_type.get())
    {
        auto element = processor.getElementState(5);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(5)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m5_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m5_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(5, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(5)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e6_type.get())
    {
        auto element = processor.getElementState(6);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(6)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m6_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m6_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(6, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(6)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e7_type.get())
    {
        auto element = processor.getElementState(7);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(7)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m7_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m7_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(7, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(7)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e8_type.get())
    {
        auto element = processor.getElementState(8);
        if (element.getType() == FilterElement::ZERO && element.getMagnitude() == 1.0)
            processor.setParameterValue(processor.parameters.getParameter(MAGNITUDE_NAME + std::to_string(8)), POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m8_slider->setColour (juce::Slider::textBoxTextColourId, juce::Colour(0xff333333));
        m8_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(8, true);
            processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(8)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e1_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m1_slider->setLookAndFeel(&activeSliderTheme);
            p1_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m1_slider->setLookAndFeel(&unactiveSliderTheme);
            p1_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e2_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m2_slider->setLookAndFeel(&activeSliderTheme);
            p2_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m2_slider->setLookAndFeel(&unactiveSliderTheme);
            p2_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e3_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m3_slider->setLookAndFeel(&activeSliderTheme);
            p3_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m3_slider->setLookAndFeel(&unactiveSliderTheme);
            p3_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e4_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m4_slider->setLookAndFeel(&activeSliderTheme);
            p4_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m4_slider->setLookAndFeel(&unactiveSliderTheme);
            p4_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e5_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m5_slider->setLookAndFeel(&activeSliderTheme);
            p5_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m5_slider->setLookAndFeel(&unactiveSliderTheme);
            p5_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e6_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m6_slider->setLookAndFeel(&activeSliderTheme);
            p6_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m6_slider->setLookAndFeel(&unactiveSliderTheme);
            p6_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e7_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m7_slider->setLookAndFeel(&activeSliderTheme);
            p7_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m7_slider->setLookAndFeel(&unactiveSliderTheme);
            p7_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == e8_active.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            m8_slider->setLookAndFeel(&activeSliderTheme);
            p8_slider->setLookAndFeel(&activeSliderTheme);
        }
        else
        {
            m8_slider->setLookAndFeel(&unactiveSliderTheme);
            p8_slider->setLookAndFeel(&unactiveSliderTheme);
        }
    }
    else if (buttonThatWasClicked == linLog_switch.get())
    {
        linLog = linLog_switch->getToggleState();
        getFrequencyResponse();
        updateReferenceFrequencies();
        magnitude_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb);
        phase_response->updateValues(phases, referenceFrequencies, processor.getSampleRate(), true);
    }
    else if (buttonThatWasClicked == calculate_button.get())
    {
        filterDesignAndSetup();
    }
    else if (buttonThatWasClicked == multiply_phases_button.get())
    {
        processor.doublePhases();
    }
    else if (buttonThatWasClicked == divide_phases_button.get())
    {
        processor.halfPhases();
    }
    else if (buttonThatWasClicked == swap_button.get())
    {
        processor.swapPolesAndZeros();
    }
    else if (buttonThatWasClicked == turn_on_button.get())
    {
        processor.turnOnOffAllElements(1);
    }
    else if (buttonThatWasClicked == turn_off_button.get())
    {
        processor.turnOnOffAllElements(0);
    }
    else if (buttonThatWasClicked == ampDb_switch.get())
    {
        ampDb = ampDb_switch->getToggleState();
        magnitude_response->updateValues(magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb);
    }
    else if (buttonThatWasClicked == autoUpdate_button.get())
    {
        if (autoUpdate_button->getToggleState())
        {
            autoUpdate = true;
            autoUpdateCheckAndSetup();
        }
        else
            autoUpdate = false;
    }
    else if (buttonThatWasClicked == undo_button.get())
    {
        valueTreeState.undoManager->undo();
    }
    else if (buttonThatWasClicked == redo_button.get())
    {
        valueTreeState.undoManager->redo();
    }
    else if (buttonThatWasClicked == saveCoefficients_button.get())
    {
        auto coefficients = processor.getCoefficients();
        
        auto defaultPresetLocation = File::getSpecialLocation(File::SpecialLocationType::commonDocumentsDirectory);
        juce::FileChooser chooser("Select the save location...", defaultPresetLocation, "*.txt");
        if (chooser.browseForFileToSave(true))
        {
            auto file = chooser.getResult();
            if (file.exists())
                file.deleteFile();
            juce::FileOutputStream outputStream(file);
            if (outputStream.openedOk())
            {
                juce::String content;
                
                content += "Sample rate: " + juce::String(processor.getSampleRate()) + "\n\n";
                
                int i = 0;
                for (const auto& coeff : coefficients)
                {
                    content += juce::String(coeff) + "\n";
                    ++ i;
                    if (i % 3 == 0)
                        content += "\n";
                }
                
                file.replaceWithText(content);
            }
        }
    }
    else if (buttonThatWasClicked == save_preset_button.get())
    {
        auto defaultPresetLocation = File::getSpecialLocation(File::SpecialLocationType::commonDocumentsDirectory);
        juce::FileChooser chooser("Select the save location...", defaultPresetLocation, "*.xml");
        if (chooser.browseForFileToSave(true))
        {
            auto file = chooser.getResult();
            if (file.exists())
                file.deleteFile();
            juce::FileOutputStream outputStream(file);
            if (outputStream.openedOk())
            {
                MemoryBlock destData;
                processor.getStateInformation(destData);
                outputStream.write(destData.getData(), destData.getSize());
            }
        }
    }
    else if (buttonThatWasClicked == load_preset_button.get())
    {
        auto defaultPresetLocation = File::getSpecialLocation(File::SpecialLocationType::commonDocumentsDirectory);
        juce::FileChooser chooser("Select the preset to load...", defaultPresetLocation, "*.xml");
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            MemoryBlock sourceData;
            file.loadFileAsData(sourceData);
            processor.setStateInformation(sourceData.getData(), sourceData.getSize());
        }
    }
    else if (buttonThatWasClicked == autoGain.get())
    {
        if (buttonThatWasClicked->getToggleState())
        {
            gain1_label->setEditable(false);
            gain1_label->setDraggable(false);
            
            gain2_label->setEditable(false);
            gain2_label->setDraggable(false);
            
            gain3_label->setEditable(false);
            gain3_label->setDraggable(false);
            
            gain4_label->setEditable(false);
            gain4_label->setDraggable(false);
            
            gain5_label->setEditable(false);
            gain5_label->setDraggable(false);
            
            gain6_label->setEditable(false);
            gain6_label->setDraggable(false);
            
            gain7_label->setEditable(false);
            gain7_label->setDraggable(false);
            
            gain8_label->setEditable(false);
            gain8_label->setDraggable(false);
            
            gainsInit();
        }
        else
        {
            gain1_label->setEditable(true);
            gain1_label->setDraggable(true);
            
            gain2_label->setEditable(true);
            gain2_label->setDraggable(true);
            
            gain3_label->setEditable(true);
            gain3_label->setDraggable(true);
            
            gain4_label->setEditable(true);
            gain4_label->setDraggable(true);
            
            gain5_label->setEditable(true);
            gain5_label->setDraggable(true);
            
            gain6_label->setEditable(true);
            gain6_label->setDraggable(true);
            
            gain7_label->setEditable(true);
            gain7_label->setDraggable(true);
            
            gain8_label->setEditable(true);
            gain8_label->setDraggable(true);
        }
    }
}

void EditorComponent::labelTextChanged (juce::Label* labelThatHasChanged)
{
    const double sampleRate = processor.getSampleRate();
    int newFrequency = labelThatHasChanged->getText().getIntValue();

    float newGain = formatGainInput(labelThatHasChanged->getText().getFloatValue());

    if (labelThatHasChanged == frequency_label.get())
        formatDesignFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);

    if (labelThatHasChanged == p1_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p1_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p2_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p2_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p3_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p3_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p4_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p4_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p5_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p5_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p6_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p6_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p7_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p7_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == p8_freq.get())
    {
        formatFrequencyInput(newFrequency, labelThatHasChanged, sampleRate);
        updateSliderFromFrequency(newFrequency, p8_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == frequency_label.get())
    {
        design_frequency = updateDesignSliderFromFrequency(newFrequency, frequency_design_slider.get(), sampleRate);
        return;
    }
    else if (labelThatHasChanged == gain1_label.get())
    {
        e1_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain2_label.get())
    {
        e2_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain3_label.get())
    {
        e3_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain4_label.get())
    {
        e4_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain5_label.get())
    {
        e5_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain6_label.get())
    {
        e6_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain7_label.get())
    {
        e7_gain->setValue(newGain);
    }
    else if (labelThatHasChanged == gain8_label.get())
    {
        e8_gain->setValue(newGain);
    }
    
    labelThatHasChanged->setText(juce::String(newGain, 2) + " dB", NotificationType::dontSendNotification);
}

void EditorComponent::comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == type_box.get())
    {
        design_type = comboBoxThatHasChanged->getSelectedId();
        calculate_button->setEnabled(false);
        if (!design_type)
            return;
        if (design_type == 1)
            updateGUIButterworth();
        else
        {
            updateGUIEllipticChebyshevIandII();
            if (autoUpdate)
                autoUpdateCheckAndSetup();
        }
    }
    else if (comboBoxThatHasChanged == shape_box.get())
    {
        design_shape = comboBoxThatHasChanged->getSelectedId();
        updateGUIGivenShape();
        if (autoUpdate)
            autoUpdateCheckAndSetup();
    }
    else if (comboBoxThatHasChanged == order_box.get())
    {
        design_filters_to_activate = comboBoxThatHasChanged->getSelectedId();
        if (design_filters_to_activate)
        {
            calculate_button->setEnabled(true);
            if (autoUpdate)
                autoUpdateCheckAndSetup();
        }
    }
}

void EditorComponent::slidersInit()
{
    auto elements = processor.getFilterElementsChain();

    if (elements[0].isActive())
    {
        m1_slider->setLookAndFeel(&activeSliderTheme);
        p1_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m1_slider->setLookAndFeel(&unactiveSliderTheme);
        p1_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[1].isActive())
    {
        m2_slider->setLookAndFeel(&activeSliderTheme);
        p2_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m2_slider->setLookAndFeel(&unactiveSliderTheme);
        p2_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[2].isActive())
    {
        m3_slider->setLookAndFeel(&activeSliderTheme);
        p3_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m3_slider->setLookAndFeel(&unactiveSliderTheme);
        p3_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[3].isActive())
    {
        m4_slider->setLookAndFeel(&activeSliderTheme);
        p4_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m4_slider->setLookAndFeel(&unactiveSliderTheme);
        p4_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[4].isActive())
    {
        m5_slider->setLookAndFeel(&activeSliderTheme);
        p5_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m5_slider->setLookAndFeel(&unactiveSliderTheme);
        p5_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[5].isActive())
    {
        m6_slider->setLookAndFeel(&activeSliderTheme);
        p6_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m6_slider->setLookAndFeel(&unactiveSliderTheme);
        p6_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[6].isActive())
    {
        m7_slider->setLookAndFeel(&activeSliderTheme);
        p7_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m7_slider->setLookAndFeel(&unactiveSliderTheme);
        p7_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[7].isActive())
    {
        m8_slider->setLookAndFeel(&activeSliderTheme);
        p8_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m8_slider->setLookAndFeel(&unactiveSliderTheme);
        p8_slider->setLookAndFeel(&unactiveSliderTheme);
    }
}

void EditorComponent::gainsInit()
{
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        auto gain = calculateGain(i);
        processor.setParameterValue(processor.parameters.getParameter(GAIN_NAME + std::to_string(i)), jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
    }
}

void EditorComponent::getFrequencyResponse()
{
    double phi;
    const auto sampleRate = processor.getSampleRate();
    const double pi = MathConstants<double>::pi;

    auto n1 = log(FREQUENCY_FLOOR / sampleRate);
    auto n2 = log(0.5) - n1;

    std::complex<double> frequencyResponse;
    
    const double gain = processor.getCurrentGain();

    for (int i = 0; i < GRAPHS_QUALITY; ++ i)
    {
        if (linLog)
            phi = static_cast<double>(i) / static_cast<double>(2 * (GRAPHS_QUALITY - 1));
        else
            phi = exp(n1 + (n2 * (static_cast<double>(i) / (static_cast<double>(GRAPHS_QUALITY - 1)))));
        
        frequencyResponse = processor.getFrequencyResponseAtPhi(phi);
        magnitudes[i] = gain * std::abs(frequencyResponse);
        phases[i] = (pi + std::arg(frequencyResponse)) / (2.0 * pi);
    }
}

void EditorComponent::updateReferenceFrequencies()
{
    double phi;
    const auto sampleRate = processor.getSampleRate();

    auto n1 = log(FREQUENCY_FLOOR / sampleRate);
    auto n2 = log(0.5) - log(FREQUENCY_FLOOR / sampleRate);

    int k = 0;

    for (int i = 0; i < GRAPHS_QUALITY; ++ i)
    {
        if (linLog)
            phi = static_cast<double>(i) / static_cast<double>(2 * (GRAPHS_QUALITY - 1)); // Linear spectrum
        else
            phi = exp(n1 + (n2 * (static_cast<double>(i) / (static_cast<double>(GRAPHS_QUALITY - 1))))); // Log spectrum

        if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
        {
            referenceFrequencies[k] = phi;
            ++ k;
        }
    }
}

void EditorComponent::updateElements ()
{
    e1->updateElement(processor.getElementState(1), 1, gaussian_plane.get(), &processor);
    e2->updateElement(processor.getElementState(2), 2, gaussian_plane.get(), &processor);
    e3->updateElement(processor.getElementState(3), 3, gaussian_plane.get(), &processor);
    e4->updateElement(processor.getElementState(4), 4, gaussian_plane.get(), &processor);
    e5->updateElement(processor.getElementState(5), 5, gaussian_plane.get(), &processor);
    e6->updateElement(processor.getElementState(6), 6, gaussian_plane.get(), &processor);
    e7->updateElement(processor.getElementState(7), 7, gaussian_plane.get(), &processor);
    e8->updateElement(processor.getElementState(8), 8, gaussian_plane.get(), &processor);
}

void EditorComponent::updateFrequencyFromSlider(juce::Slider* slider, juce::Label* label, double sampleRate)
{
    double sliderValue = slider->getValue();
    int frequency = std::ceil((sliderValue * sampleRate) / 2.0);

    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);
}

void EditorComponent::updateSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate)
{
    double sliderValue = (frequency * 2.0) / sampleRate;
    slider->setValue(sliderValue, juce::sendNotificationSync);
}

double EditorComponent::updateFrequencyFromDesignSlider(juce::Slider* slider, juce::Label* label, double sampleRate)
{
    double sliderValue = slider->getValue();
    int frequency = std::ceil((sliderValue * sampleRate) / 2000.0);
    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);

    return frequency;
}

double EditorComponent::updateDesignSliderFromFrequency(int frequency, juce::Slider* slider, double sampleRate)
{
    double sliderValue = (frequency * 2000.0) / sampleRate;
    slider->setValue(sliderValue, juce::sendNotificationSync);

    return std::ceil((slider->getValue() * sampleRate) / 2000.0);
}

void EditorComponent::formatFrequencyInput(int& frequency, juce::Label *label, double sampleRate)
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

void EditorComponent::formatDesignFrequencyInput(int& frequency, juce::Label *label, double sampleRate)
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

void EditorComponent::updateDesignFrequencySlider(short int option)
{
    const double sampleRate = processor.getSampleRate();
    switch (option)
    {
        case 1:
        {
            design_frequency_label->setBounds (1058, 243, 72, 20);
            frequency_design_slider->setBounds (1027, 265, 83, 25);
            frequency_design_slider->setRange (0.0001, 999, 0.0001);
            updateDesignSliderFromFrequency(design_frequency, frequency_design_slider.get(), sampleRate);
            frequency_label->setBounds (1110, 265, 60, 25);
        } break;

        case 2:
        {
            design_frequency_label->setBounds (1058, 183, 72, 20);
            frequency_design_slider->setBounds (1027, 205, 83, 25);
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

void EditorComponent::setTransitionWidthRange ()
{
    double sampleRate = processor.getSampleRate();
    double normalisedFrequency = design_frequency / sampleRate;
    double minValue = 0.0001;
    double maxValue = jmin(2 * normalisedFrequency - minValue, 2 * (0.5 - normalisedFrequency) - minValue);
    const double interval = 0.00001;
    const int maxOrder = 8;

    auto Gp = Decibels::decibelsToGain (passbandAmplitudedB, -300.0);
    auto Gs = Decibels::decibelsToGain (stopbandAmplitudedB, -300.0);
    
    auto epsp = std::sqrt (1.0 / (Gp * Gp) - 1.0);
    auto epss = std::sqrt (1.0 / (Gs * Gs) - 1.0);
    
    if (design_type == 2 || design_type == 3)
    {
        
        double X = std::acosh( epss  /  epsp);
        
        double Y = std::acosh( std::tan(MathConstants<double>::pi * (normalisedFrequency + minValue / 2)) / std::tan(MathConstants<double>::pi * (normalisedFrequency - minValue / 2)));
        
        while (roundToInt(std::ceil(X / Y)) > maxOrder)
        {
            minValue += interval;
            Y = acosh( std::tan(MathConstants<double>::pi * (normalisedFrequency + minValue / 2)) / std::tan(MathConstants<double>::pi * (normalisedFrequency - minValue / 2)));
        }
    }
    else
    {
        double K, Kp, K1, K1p;
        
        auto k1 = epsp / epss;
        juce::dsp::SpecialFunctions::ellipticIntegralK (k1, K1, K1p); // Rimane fisso
        
        auto k = std::tan (MathConstants<double>::pi * std::tan(MathConstants<double>::pi * (normalisedFrequency - minValue / 2))) / std::tan (MathConstants<double>::pi * std::tan(MathConstants<double>::pi * (normalisedFrequency + minValue / 2)));
        juce::dsp::SpecialFunctions::ellipticIntegralK (k, K, Kp);
        
        while (roundToInt (std::ceil ((K1p * K) / (K1 * Kp))) > maxOrder)
        {
            minValue += interval;
            auto k = std::tan (MathConstants<double>::pi * std::tan(MathConstants<double>::pi * (normalisedFrequency - minValue / 2))) / std::tan (MathConstants<double>::pi * std::tan(MathConstants<double>::pi * (normalisedFrequency + minValue / 2)));
            juce::dsp::SpecialFunctions::ellipticIntegralK (k, K, Kp);
        }
    }

    transition_width_slider->setRange(minValue, maxValue);
    if (transition_width_slider->getValue() < minValue)
        transition_width_slider->setValue(minValue);
    normalisedTransitionWidth = transition_width_slider->getValue();
}

void EditorComponent::updateGUIGivenShape()
{
    if (!type_box->getSelectedId())
    {
        type_box->clear();
        int i = 1;
        for (juce::String& type : selectable_filter_types)
        {
            type_box->addItem(type, i);
            ++ i;
        }
    }
}

void EditorComponent::updateGUIButterworth()
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
        juce::String stringToVisualize = order + "  (-" + juce::String(attenuation) + " db / octave)";
        order_box->addItem(stringToVisualize, int_order);
    }
}

void EditorComponent::updateGUIEllipticChebyshevIandII()
{
    order_box->setVisible(false);

    updateDesignFrequencySlider(2);

    passbandAmplitude_slider->setValue(passbandAmplitudedB);
    stopbandAmplitude_slider->setValue(stopbandAmplitudedB);
    setTransitionWidthRange();

    frequency_label->setBounds (1110, 205, 60, 25);

    passbandAmplitude_label->setVisible(true);
    passbandAmplitude_label->setBounds (1020, 236, 159, 20);

    passbandAmplitude_slider->setVisible(true);
    passbandAmplitude_slider->setBounds (1027, 260, 135, 25);

    stopbandAmplitude_label->setVisible(true);
    stopbandAmplitude_label->setBounds(1022, 291, 159, 20);

    stopbandAmplitude_slider->setVisible(true);
    stopbandAmplitude_slider->setBounds(1027, 315, 135, 25);

    transition_width_label->setVisible(true);
    transition_width_label->setBounds(1030, 346, 129, 20);

    transition_width_slider->setVisible(true);
    transition_width_slider->setBounds (1027, 370, 135, 25);

    calculate_button->setEnabled(true);
}

float EditorComponent::formatGainInput(float gain)
{
    if (gain < GAIN_FLOOR)
        gain = GAIN_FLOOR;
    else if (gain > GAIN_CEILING)
        gain = GAIN_CEILING;

    return gain;
}

float EditorComponent::calculateGain(const int elementNr, bool isChangingType)
{
    auto element = processor.getElementState(elementNr);
    auto magnitude = element.getMagnitude();
    auto type = element.getType();

    if (isChangingType)
        type = (type == FilterElement::ZERO) ? FilterElement::POLE : FilterElement::ZERO;

    auto Re = element.getMagnitude() * std::cos(element.getPhase() * MathConstants<double>::pi);

    switch (type)
    {
        case FilterElement::ZERO:
        {
            auto gainValue = std::sqrt(1.0 / (1.0 + 4 * Re * Re + magnitude * magnitude * magnitude * magnitude));
            return static_cast<float>(Decibels::gainToDecibels(gainValue, GAIN_FLOOR - 1.0));
        }

        case FilterElement::POLE:
        {
            auto Im = element.getMagnitude() * std::sin(element.getPhase() * MathConstants<double>::pi);
            auto MSG = 1.0 / (std::abs(1.0 - magnitude * magnitude) * (Re * Re - 2 * std::abs(Re) + Im * Im + 1.0));
            auto gainValue = 1.0 / std::sqrt(MSG);
            return static_cast<float>(Decibels::gainToDecibels(gainValue, GAIN_FLOOR - 1.0));
        }
    }
}

void EditorComponent::coefficientsNormalization (double* c0, double* c1, double* c2)
{
    *c1 /= (*c0);
    if (c2 != nullptr)
        *c2 /= (*c0);
}

void EditorComponent::fromCoefficientsToMagnitudeAndPhase (double& mg, double& ph, double c1, double c2)
{
    mg = sqrt(c2);
    ph = (1 / MathConstants<double>::pi) * acos(-c1 / (2 * mg));
}

void EditorComponent::filterDesignAndSetup()
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
    isSettingFilters = true;
    autoGain->setToggleState(false, NotificationType::sendNotificationSync);

    for (int i = 0; i < iirCoefficients.size(); ++ i)
    {
        const auto& coeffs = iirCoefficients[i];

        if (coeffs->coefficients.size() == 3)
        {
            std::complex<double> zero;
            std::complex<double> pole;
            
            b0=coeffs->coefficients[0];
            b1=coeffs->coefficients[1];
            a1=coeffs->coefficients[2];
            
            if (design_shape == 2)
            {
                zero = - ((- b1) / b0);
                pole = a1;
            }
            else
            {
                zero = - (b1 / b0);
                pole = - a1;
            }

            processor.setFilter(std::abs(zero), std::arg(zero), FilterElement::ZERO, elementNr, b0);
            ++ elementNr;


            processor.setFilter(std::abs(pole), std::arg(pole), FilterElement::POLE, elementNr, b0);
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

            coefficientsNormalization(&b0, &b1, &b2);

            fromCoefficientsToMagnitudeAndPhase(magnitude, phase, b1, b2);
            std::complex<double> zero = std::polar(magnitude, phase);
            processor.setFilter(std::abs(zero), std::arg(zero), FilterElement::ZERO, elementNr, b0);

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
    isSettingFilters = false;
}

void EditorComponent::autoUpdateCheckAndSetup ()
{
    if (design_type == 1)
        if (order_box->getSelectedId() != 0)
            filterDesignAndSetup();
        else
            return;
    else
        filterDesignAndSetup();
}

void EditorComponent::timerCallback()
{
    if (processor.getSafetyFlag())
    {
        warningRectangle->setVisible(true);
        warningRectangle->toFront(true);
        
        warning_label->setVisible(true);
        warning_label->toFront(true);
    }
    else
    {
        warning_label->setVisible(false);
        warningRectangle->setVisible(false);
        warningRectangle->toBack();
        warning_label->toBack();
    }
}


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="EditorComponent" componentName=""
                 parentClasses="public juce::AudioProcessorEditor" constructorParams="PolesAndZerosEQAudioProcessor&amp; p, AudioProcessorValueTreeState&amp; vts"
                 variableInitialisers="AudioProcessorEditor(&amp;p), processor (p), valueTreeState (vts)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="1200" initialHeight="790">
  <BACKGROUND backgroundColour="ffecf0f1">
    <ROUNDRECT pos="15 55 510 720" cornerSize="14.5" fill="solid: 17b1b1b1"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <ROUNDRECT pos="1005 55 180 720" cornerSize="14.5" fill="solid: 19656565"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <ROUNDRECT pos="525 55 480 720" cornerSize="14.5" fill="solid: 17b1b1b1"
               hasStroke="1" stroke="1.5, mitered, butt" strokeColour="solid: ff383838"/>
    <RECT pos="15 130 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 175 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 220 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 265 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 310 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 400 510 1" fill="solid: 25909497" hasStroke="0"/>
    <RECT pos="15 355 510 1" fill="solid: 25909497" hasStroke="0"/>
    <TEXT pos="1040 60 110 24" fill="solid: ff383838" hasStroke="0" text="FILTER DESIGN"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1075 520 40 20" fill="solid: ff383838" hasStroke="0" text="GAIN"
          fontname="Gill Sans" fontsize="10.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <ROUNDRECT pos="320 480 190 260" cornerSize="14.5" fill="solid: 11b1b1b1"
               hasStroke="1" stroke="0.5, mitered, butt" strokeColour="solid: ff383838"/>
    <TEXT pos="365 485 100 20" fill="solid: ff383838" hasStroke="0" text="SETUP SHORTCUTS"
          fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="1120 681 23 20" fill="solid: ff383838" hasStroke="0" text="dB"
          fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <TEXT pos="495 8 210 25" fill="solid: ff383838" hasStroke="0" text="POLES AND ZEROS - EQ"
          fontname="Gill Sans" fontsize="20.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
    <RECT pos="0 40 1200 1" fill="solid: 91383838" hasStroke="0"/>
    <IMAGE pos="59 16 10 10" resource="anticlockwise_arrow_png" opacity="1.0"
           mode="1"/>
    <IMAGE pos="128 16 10 10" resource="clockwise_arrow_png" opacity="1.0"
           mode="1"/>
    <IMAGE pos="1161 11 23 19" resource="load_icon_png" opacity="1.0" mode="1"/>
    <IMAGE pos="1047 15 17 12" resource="save_icon_png" opacity="1.0" mode="1"/>
    <TEXT pos="402 445 56 20" fill="solid: ff383838" hasStroke="0" text="AUTO GAIN"
          fontname="Gill Sans" fontsize="10.0" kerning="0.0" bold="0" italic="0"
          justification="36" typefaceStyle="SemiBold"/>
  </BACKGROUND>
  <SLIDER name="Element 1 gain" id="d741331d65d867d1" memberName="e1_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 5 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="30"
          textBoxHeight="10" skewFactor="1.0" needsCallback="1"/>
  <GENERICCOMPONENT name="gaussianPlane" id="f84485816497c4e3" memberName="gaussian_plane"
                    virtualName="" explicitFocusOrder="0" pos="30 480 260 260" class="GaussianPlane"
                    params="processor.getFilterElementsChain()"/>
  <GENERICCOMPONENT name="Element 2" id="9321829c993da051" memberName="e2" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(2), 2, gaussian_plane.get(), &amp;processor&#10;"/>
  <GENERICCOMPONENT name="Element 3" id="4ddeafd0e05642ed" memberName="e3" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(3), 3, gaussian_plane.get(), &amp;processor"/>
  <GENERICCOMPONENT name="Element 4" id="922bf3d6c293e1ad" memberName="e4" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(4), 4, gaussian_plane.get(), &amp;processor"/>
  <GENERICCOMPONENT name="Element 5" id="b7d51f9dcf114b20" memberName="e5" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(5), 5, gaussian_plane.get(), &amp;processor"/>
  <GENERICCOMPONENT name="Element 6" id="c34c7547dec4347" memberName="e6" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(6), 6, gaussian_plane.get(), &amp;processor"/>
  <GENERICCOMPONENT name="Element 7" id="db33ef1c083bc433" memberName="e7" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(7), 7, gaussian_plane.get(), &amp;processor"/>
  <GENERICCOMPONENT name="Element 8" id="444b4efe799724ec" memberName="e8" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(8), 8, gaussian_plane.get(), &amp;processor"/>
  <GENERICCOMPONENT name="Element 1" id="2619ff360594d8ed" memberName="e1" virtualName=""
                    explicitFocusOrder="0" pos="152 602 16 16" class="DraggableElement"
                    params="processor.getElementState(1), 1, gaussian_plane.get(), &amp;processor"/>
  <LABEL name="Passband Amplitude" id="a57ca268c3802211" memberName="passbandAmplitude_label"
         virtualName="" explicitFocusOrder="0" pos="1020 360 159 20" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="PASSBAND AMPLITUDE (DB)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <SLIDER name="Stopband Amplitude" id="308fcfe400b70b27" memberName="stopbandAmplitude_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="1022 366 135 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="-35.0" max="-21.0" int="0.1" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <TEXTBUTTON name="Reset" id="2581837dc85daae9" memberName="reset_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="421 664 80 25"
              bgColOff="ff909497" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="frequencyResponse" id="161cb81e63dc8e46" memberName="magnitude_response"
                    virtualName="" explicitFocusOrder="0" pos="540 70 450 285" class="FrequencyResponse"
                    params="magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb"/>
  <LABEL name="Frequency response" id="4c8fffb65e845bfc" memberName="mg_response_label"
         virtualName="" explicitFocusOrder="0" pos="695 360 140 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="SPECTRUM MAGNITUDE&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <GENERICCOMPONENT name="phaseResponse" id="c9a48273dec25832" memberName="phase_response"
                    virtualName="" explicitFocusOrder="0" pos="540 455 450 285" class="PhaseResponse"
                    params="phases, referenceFrequencies, processor.getSampleRate(), ampDb"/>
  <LABEL name="Phase response" id="6d08c4e421703ed5" memberName="ph_response_label"
         virtualName="" explicitFocusOrder="0" pos="710 745 110 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="SPECTRUM PHASE"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <SLIDER name="Element 1 magnitude" id="9107cda7959bc0ad" memberName="m1_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 92 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 1 phase" id="b9f95ed5c32caef9" memberName="p1_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 92 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="Magnitudes" id="f2206057c7f672c6" memberName="magnitudes_label"
         virtualName="" explicitFocusOrder="0" pos="33 57 90 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="MAGNITUDE&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <LABEL name="Phases" id="e0e34f187bbbdcdc" memberName="phases_label"
         virtualName="" explicitFocusOrder="0" pos="198 57 80 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="FREQUENCY&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <LABEL name="Filter Type" id="676532893526fbc7" memberName="zero_pole_label"
         virtualName="" explicitFocusOrder="0" pos="310 57 91 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="FILTER TYPE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <LABEL name="Active" id="e82d0bcbcd7af0" memberName="active_label" virtualName=""
         explicitFocusOrder="0" pos="401 57 55 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="ACTIVE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Element 1 type" id="cb026ef1068db639" memberName="e1_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 93 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 2 type" id="56addf679899dfb" memberName="e2_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 138 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 3 type" id="5abda647a887dc8e" memberName="e3_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 183 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 4 type" id="3136ab6a98246edb" memberName="e4_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 228 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 5 type" id="4d6d7aa8e46a31c6" memberName="e5_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 273 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 6 type" id="6a190298ad04abf" memberName="e6_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 318 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 7 type" id="57fd1da9881e7a6a" memberName="e7_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 363 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 8 type" id="a0b1086b7f717581" memberName="e8_type"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="331 408 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 1 active" id="84bb86bdbd9ccede" memberName="e1_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 93 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 2 active" id="176e3fb309f12ef5" memberName="e2_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 138 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 3 active" id="d6d9c0542e41b9c3" memberName="e3_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 183 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 4 active" id="72d6ebba0023e28e" memberName="e4_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 228 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 5 active" id="d8c38a2ce324a73d" memberName="e5_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 273 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 6 active" id="1b42bda33e1c947c" memberName="e6_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 318 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 7 active" id="3405e0694e2b063d" memberName="e7_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 363 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TOGGLEBUTTON name="Element 8 active" id="b60e0b9c2104a2bd" memberName="e8_active"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="404 408 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <LABEL name="Gaussian plane" id="32980deccea32218" memberName="gaussian_plane_label"
         virtualName="" explicitFocusOrder="0" pos="95 745 140 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="GAUSSIAN PLANE"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="13.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Bypass" id="4daf2a36bc47407c" memberName="bypass" virtualName="CustomToggleButton"
                explicitFocusOrder="0" pos="1067 712 60 25" buttonText="" connectedEdges="0"
                needsCallback="0" radioGroupId="0" state="0"/>
  <SLIDER name="Master Gain" id="7e880f1fc774c2af" memberName="masterGain_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="1074 540 45 160"
          thumbcol="ffffffff" textboxtext="ff383838" textboxhighlight="66686868"
          textboxoutline="8e989b" min="0.0" max="10.0" int="0.0" style="LinearVertical"
          textBoxPos="TextBoxBelow" textBoxEditable="1" textBoxWidth="100"
          textBoxHeight="20" skewFactor="1.0" needsCallback="0"/>
  <TOGGLEBUTTON name="Linear / Logarithmic" id="269b07022e58d862" memberName="linLog_switch"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="931 362 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <SLIDER name="Element 2 magnitude" id="e1a6d1b02cc8ab80" memberName="m2_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 137 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 3 magnitude" id="b0094bd01977f340" memberName="m3_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 182 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 4 magnitude" id="8f433acd768ec1d9" memberName="m4_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 227 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 5 magnitude" id="fe0864ae3596b10d" memberName="m5_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 272 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 6 magnitude" id="e6371565d3e4bb47" memberName="m6_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 317 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 7 magnitude" id="3661ca4111317400" memberName="m7_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 362 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 8 magnitude" id="8e83a0a6b9d1e264" memberName="m8_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="20 407 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 2 phase" id="60007c00fdd0e234" memberName="p2_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 137 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 3 phase" id="dad16f547c32eed" memberName="p3_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 182 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 4 phase" id="2baa73d9d72e7d2a" memberName="p4_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 227 120 25"
          bkgcol="ff383838" thumbcol="ff909497" trackcol="ff383838" textboxtext="ff383838"
          textboxbkgd="0" textboxhighlight="66686868" textboxoutline="0"
          min="0.0" max="10.0" int="0.0" style="LinearHorizontal" textBoxPos="TextBoxRight"
          textBoxEditable="1" textBoxWidth="50" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Element 5 phase" id="6ba360671cfe4646" memberName="p5_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 272 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 6 phase" id="6fb200ac73072a45" memberName="p6_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 317 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 7 phase" id="481e9546dc440bf4" memberName="p7_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 362 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 8 phase" id="d587f0ce08864b4e" memberName="p8_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="144 407 120 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Element 1 frequency" id="79f5387b07d9c483" memberName="p1_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 92 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 2 frequency" id="5b5fee371d8a9052" memberName="p2_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 137 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 3 frequency" id="a15e9da2c1f0e36b" memberName="p3_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 182 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 4 frequency" id="d6a7e59b3d26f91e" memberName="p4_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 227 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 5 frequency" id="f0a2b500cc7bc03" memberName="p5_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 272 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 6 frequency" id="5dfdb03e7a654b13" memberName="p6_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 317 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 7 frequency" id="600a01c1726873b3" memberName="p7_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 362 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 8 frequency" id="f85a4dea8bdf47f1" memberName="p8_freq"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="265 407 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <COMBOBOX name="Design type" id="5a16af79e3a09d2b" memberName="type_box"
            virtualName="CustomComboBox" explicitFocusOrder="0" pos="1025 150 140 25"
            editable="0" layout="33" items="" textWhenNonSelected="TYPE"
            textWhenNoItems="(no choices)"/>
  <COMBOBOX name="Design shape" id="fab26cb579c24549" memberName="shape_box"
            virtualName="CustomComboBox" explicitFocusOrder="0" pos="1025 105 140 25"
            editable="0" layout="33" items="LOWPASS&#10;HIGHPASS" textWhenNonSelected="SHAPE"
            textWhenNoItems="(no choices)"/>
  <TEXTBUTTON name="Calculate" id="6b0929d790004858" memberName="calculate_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="1098 430 80 30"
              bgColOff="ff909497" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Multiply phases" id="fd9508a1dc4c09ae" memberName="multiply_phases_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="328 594 80 25"
              bgColOff="ff909497" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Divide phases" id="a62537b9345044f3" memberName="divide_phases_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="421 594 80 25"
              bgColOff="ff909497" bgColOn="ff727272" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Swap poles/zeros" id="938b23da9ff326b0" memberName="swap_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="328 664 80 25"
              bgColOff="ff909497" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Turn on all the elements" id="e4b8377926241c51" memberName="turn_on_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="328 531 80 25"
              bgColOff="ff73cc81" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Turn off all the elements" id="f6c36c1c53a4a06d" memberName="turn_off_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="421 531 80 25"
              bgColOff="ffe86d5c" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <COMBOBOX name="Design order" id="a7c23e76d01914d5" memberName="order_box"
            virtualName="CustomComboBox" explicitFocusOrder="0" pos="1025 195 140 25"
            editable="0" layout="33" items="" textWhenNonSelected="ORDER"
            textWhenNoItems="(no choices)"/>
  <LABEL name="Design frequency" id="bc37557b2c8cc2ce" memberName="design_frequency_label"
         virtualName="" explicitFocusOrder="0" pos="1063 233 72 20" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="FREQUENCY&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Amplitude / dB" id="b9198764b9daa498" memberName="ampDb_switch"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="543 362 52 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <SLIDER name="Frequency design slider" id="805d44cec628c3" memberName="frequency_design_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="1022 255 83 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0001" max="1000.0" int="0.0001" style="LinearHorizontal"
          textBoxPos="NoTextBox" textBoxEditable="0" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Frequency Label" id="bb3cf5d37607187f" memberName="frequency_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="1110 255 60 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Transition width" id="b5068a6d459942e" memberName="transition_width_label"
         virtualName="" explicitFocusOrder="0" pos="1031 296 129 20" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="TRANSITION WIDTH"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <SLIDER name="Transition width slider" id="a9239ce032310b32" memberName="transition_width_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="1022 320 135 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.001" max="0.5" int="1.0e-5" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Passband Amplitude" id="ef6299d4c0c90b49" memberName="passbandAmplitude_slider"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="1022 384 135 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="-10.0" max="-0.1" int="0.1" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Stopband Amplitude" id="586783be637d3c53" memberName="stopbandAmplitude_label"
         virtualName="" explicitFocusOrder="0" pos="1020 342 159 20" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="STOPBAND AMPLITUDE (DB)"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="36" typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Auto update" id="8d6ebbb17b01b41e" memberName="autoUpdate_button"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="1013 430 80 30"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <TEXTBUTTON name="Undo" id="aa5d105b68b6f3c" memberName="undo_button" virtualName="CustomButton"
              explicitFocusOrder="0" pos="18 9 60 25" bgColOff="909497" bgColOn="ff505050"
              buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Redo" id="83782176a800780" memberName="redo_button" virtualName="CustomButton"
              explicitFocusOrder="0" pos="91 9 60 25" bgColOff="909497" bgColOn="ff505050"
              buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Save preset" id="df6cbad3d0574472" memberName="save_preset_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="971 9 100 25"
              bgColOff="909497" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Load preset" id="7b1d6ea147b5ed36" memberName="load_preset_button"
              virtualName="CustomButton" explicitFocusOrder="0" pos="1082 9 100 25"
              bgColOff="909497" bgColOn="ff505050" buttonText="" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="Active" id="d1b48585f0499e3f" memberName="active_label2"
         virtualName="" explicitFocusOrder="0" pos="464 57 55 24" textCol="ff383838"
         edTextCol="ff000000" edBkgCol="0" labelText="GAIN" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Gill Sans"
         fontsize="13.0" kerning="0.0" bold="0" italic="0" justification="36"
         typefaceStyle="SemiBold"/>
  <TOGGLEBUTTON name="Auto Gain" id="90911b86576c8406" memberName="autoGain"
                virtualName="CustomToggleButton" explicitFocusOrder="0" pos="462 445 49 21"
                buttonText="" connectedEdges="0" needsCallback="1" radioGroupId="0"
                state="0"/>
  <SLIDER name="Element 2 gain" id="a4ada05f7ffd7d3" memberName="e2_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 5 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 3 gain" id="958a3cfe5686df02" memberName="e3_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 8 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 4 gain" id="9a6c08a59a4a3368" memberName="e4_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 8 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 5 gain" id="4b84959a38db6ff1" memberName="e5_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 8 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 6 gain" id="814629cc690e6dca" memberName="e6_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 8 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 7 gain" id="d34f362f2ea94202" memberName="e7_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 8 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <SLIDER name="Element 8 gain" id="96723bac4463f16f" memberName="e8_gain"
          virtualName="CustomSlider" explicitFocusOrder="0" pos="310 8 97 25"
          thumbcol="ffffffff" textboxtext="ff333333" textboxbkgd="0" textboxhighlight="66686868"
          textboxoutline="0" min="0.0" max="10.0" int="0.0" style="LinearHorizontal"
          textBoxPos="TextBoxRight" textBoxEditable="1" textBoxWidth="50"
          textBoxHeight="20" skewFactor="1.0" needsCallback="1"/>
  <LABEL name="Element 1 Gain" id="d3b3c84dbd820cb0" memberName="gain1_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 92 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 2 Gain" id="8fb3eee154973f41" memberName="gain2_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 137 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 3 Gain" id="42dedfd9cadd4d58" memberName="gain3_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 182 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 4 Gain" id="2b3213a7dc1fa0a6" memberName="gain4_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 227 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 5 Gain" id="9ec39126e0cb6ee0" memberName="gain5_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 272 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 6 Gain" id="adac5f376d3d0dc3" memberName="gain6_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 317 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 7 Gain" id="342ffb8ce4c00357" memberName="gain7_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 362 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
  <LABEL name="Element 8 Gain" id="ace3eac3aa495676" memberName="gain8_label"
         virtualName="CustomLabel" explicitFocusOrder="0" pos="467 407 50 25"
         textCol="ff333333" edTextCol="ff000000" edBkgCol="ff000000" labelText=""
         editableSingleClick="1" editableDoubleClick="1" focusDiscardsChanges="0"
         fontname="Gill Sans" fontsize="12.0" kerning="0.0" bold="0" italic="0"
         justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif

// JUCER_RESOURCE: anticlockwise_arrow_png, 22421, "../GUI/anticlockwise_arrow.png"
static const unsigned char resource_PluginEditor_anticlockwise_arrow_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,3,72,0,0,3,118,8,6,0,0,0,195,151,169,6,0,0,0,9,112,72,89,115,0,0,11,19,
0,0,11,19,1,0,154,156,24,0,0,87,71,73,68,65,84,120,156,237,253,123,204,109,231,125,31,118,126,207,161,14,47,34,45,17,242,97,29,178,182,69,37,14,233,212,54,17,84,35,170,158,56,100,28,212,54,101,23,141,
41,27,237,184,82,144,6,147,136,30,175,181,128,162,82,130,4,133,99,42,198,20,233,196,154,6,88,107,165,161,147,255,70,178,39,128,65,58,51,77,76,38,25,199,100,18,164,166,226,65,74,229,98,82,174,69,230,66,
70,195,99,85,20,121,76,30,30,113,191,243,199,187,14,117,72,158,203,123,217,107,63,235,242,249,0,2,37,210,90,251,107,157,247,178,191,251,121,158,223,115,98,111,111,47,0,0,0,36,39,75,7,0,0,0,152,10,5,9,
0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,
65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,
80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,188,171,116,0,0,166,165,170,155,235,147,
124,36,201,95,74,242,190,36,55,92,244,143,223,149,111,124,184,118,205,101,30,113,226,109,127,61,138,189,43,252,231,55,46,250,247,155,33,207,235,23,253,189,87,147,252,203,36,63,221,119,237,19,199,200,0,
192,10,157,216,219,123,251,239,32,0,150,166,170,155,123,146,116,73,126,111,190,81,114,46,46,56,39,114,188,66,51,7,123,121,103,209,218,36,121,57,201,63,76,242,231,250,174,125,166,68,48,0,166,67,65,2,152,
177,170,110,78,37,249,207,146,252,249,36,119,37,57,53,252,163,109,172,226,172,217,222,219,254,250,114,146,255,37,201,95,73,242,255,233,187,246,124,137,80,0,140,79,65,2,152,160,170,110,78,38,249,99,73,
126,44,251,197,231,214,236,111,117,187,33,206,143,78,201,133,85,169,175,12,255,250,82,246,139,212,223,233,187,246,201,146,193,0,56,26,5,9,160,160,170,110,238,72,242,107,73,222,147,228,218,236,111,123,
83,128,150,227,66,129,122,35,251,231,164,190,150,228,255,216,119,237,179,37,67,1,112,121,10,18,192,142,84,117,243,80,146,143,69,17,98,223,38,223,40,78,191,220,119,237,199,11,231,1,32,10,18,192,40,170,
186,121,56,201,15,103,191,12,37,206,2,113,48,23,86,156,94,77,242,115,125,215,62,88,54,14,192,250,40,72,0,199,80,213,205,221,73,254,114,146,15,38,121,119,20,33,198,177,201,55,198,151,255,133,190,107,31,
45,156,7,96,177,20,36,128,3,24,138,208,223,78,242,77,217,159,20,103,123,28,83,176,73,114,62,251,83,246,126,196,96,8,128,227,83,144,0,46,161,170,155,91,147,60,149,228,189,217,191,55,200,202,16,115,112,
97,139,222,203,73,254,100,223,181,143,20,206,3,48,59,10,18,64,146,170,110,110,76,242,108,146,247,101,29,151,166,178,30,23,46,195,253,15,251,174,61,91,58,12,192,212,41,72,192,42,85,117,115,58,201,63,78,
242,29,177,93,142,117,217,100,255,206,166,63,212,119,237,51,165,195,0,76,141,130,4,44,94,85,55,215,39,121,44,201,127,148,253,21,34,133,8,190,225,194,0,136,223,78,242,189,86,153,128,181,83,144,128,69,170,
234,230,241,36,31,206,254,152,109,219,229,224,224,246,178,127,55,211,87,250,174,189,173,116,24,128,93,83,144,128,197,168,234,230,197,236,15,85,56,85,58,11,44,200,38,251,101,233,150,210,65,0,118,65,65,
2,102,235,162,73,115,182,205,193,110,92,24,248,240,131,70,138,3,75,165,32,1,179,49,76,154,251,59,73,190,55,86,137,96,10,206,39,249,213,36,63,230,236,18,176,20,10,18,48,105,85,221,60,153,228,174,56,75,
4,115,112,62,201,25,103,151,128,57,83,144,128,201,169,234,230,209,36,127,52,86,137,96,174,246,146,124,61,201,175,246,93,123,95,233,48,0,135,161,32,1,147,80,213,205,253,73,126,49,201,117,165,179,0,91,119,
62,201,159,239,187,246,51,165,131,0,92,141,130,4,20,83,213,205,3,73,62,147,228,198,210,89,128,157,57,151,228,191,83,150,128,169,82,144,128,157,170,234,230,217,36,223,22,83,231,128,253,169,120,95,78,242,
193,190,107,95,40,29,6,32,81,144,128,29,168,234,230,169,36,223,21,165,8,184,188,243,73,126,179,239,218,187,74,7,1,214,77,65,2,70,81,213,205,233,36,207,198,246,57,224,240,206,39,249,238,190,107,159,41,
29,4,88,31,5,9,216,170,170,110,158,78,242,29,177,90,4,28,223,94,146,47,246,93,123,103,233,32,192,122,40,72,192,177,85,117,243,120,92,222,10,140,107,147,228,239,25,27,14,140,77,65,2,142,100,40,69,223,23,
43,69,192,238,109,146,124,165,239,218,91,74,7,1,150,71,65,2,14,172,170,155,147,73,190,150,228,221,73,78,20,142,3,144,36,103,20,37,96,155,20,36,224,170,170,186,185,59,201,19,113,137,43,48,93,231,147,220,
209,119,237,179,165,131,0,243,166,32,1,151,53,140,231,254,238,88,45,2,230,99,147,228,95,24,23,14,28,149,130,4,188,133,139,92,129,5,217,36,249,55,125,215,222,94,58,8,48,31,10,18,144,36,169,234,230,149,
184,179,8,88,46,103,149,128,3,81,144,96,229,170,186,121,49,201,233,210,57,0,118,228,108,223,181,55,149,14,1,76,151,130,4,43,84,213,205,173,73,158,139,123,139,128,245,58,155,228,187,13,117,0,222,78,65,
130,21,169,234,230,201,36,31,140,243,69,0,23,236,37,249,231,134,58,0,23,40,72,176,2,85,221,188,22,35,186,1,174,230,92,223,181,215,151,14,1,148,165,32,193,130,25,188,0,112,36,206,41,193,138,41,72,176,48,
85,221,156,76,242,187,177,98,4,112,92,103,147,188,167,239,218,77,233,32,192,238,40,72,176,16,85,221,220,157,228,137,40,70,0,219,182,73,242,253,125,215,62,81,58,8,48,62,5,9,102,174,170,155,231,147,252,
158,36,39,74,103,1,88,184,189,36,255,218,197,179,176,108,10,18,204,84,85,55,143,38,249,193,40,70,0,187,182,151,228,239,246,93,123,95,233,32,192,246,41,72,48,51,138,17,192,100,108,146,252,84,223,181,15,
149,14,2,108,143,130,4,51,81,213,205,125,73,254,118,220,97,4,48,53,155,36,63,219,119,237,131,165,131,0,199,167,32,193,196,85,117,243,84,146,239,138,98,4,48,117,155,36,191,209,119,237,221,165,131,0,71,
167,32,193,68,85,117,243,100,146,15,149,206,1,192,145,188,208,119,237,109,165,67,0,135,167,32,193,196,84,117,115,127,146,95,138,21,35,128,185,219,36,249,3,125,215,62,83,58,8,112,112,10,18,76,68,85,55,
215,103,255,82,66,197,8,96,89,206,37,185,185,239,218,215,74,7,1,174,78,65,130,194,170,186,57,157,228,223,198,5,175,0,75,119,182,239,218,155,74,135,0,174,76,65,130,66,170,186,185,39,201,63,136,21,35,128,
181,57,159,228,61,86,148,96,154,20,36,40,160,170,155,23,147,156,46,157,3,128,162,172,40,193,4,41,72,176,67,38,211,1,240,54,123,73,254,122,223,181,15,148,14,2,236,83,144,96,7,134,237,116,191,150,228,68,
225,40,0,76,211,38,201,143,247,93,251,72,233,32,176,118,10,18,140,168,170,155,7,147,252,116,156,51,2,224,96,54,73,190,191,239,218,39,74,7,129,181,82,144,96,36,85,221,188,22,147,233,0,56,26,231,147,160,
16,5,9,182,172,170,155,199,147,220,83,58,7,0,139,240,92,223,181,183,151,14,1,107,162,32,193,150,24,219,13,192,72,140,5,135,29,82,144,96,11,170,186,121,37,201,141,165,115,0,176,104,103,147,156,86,148,96,
92,10,18,28,131,98,4,64,1,103,250,174,189,165,116,8,88,42,5,9,142,160,170,155,207,38,249,88,233,28,0,172,214,38,201,183,244,93,123,166,116,16,88,26,5,9,14,201,170,17,0,19,242,82,223,181,55,151,14,1,75,
162,32,193,1,85,117,243,104,146,31,140,203,94,1,152,150,189,36,127,166,239,218,207,148,14,2,75,160,32,193,85,40,70,0,204,196,38,201,207,246,93,251,96,233,32,48,103,10,18,92,65,85,55,47,38,57,93,58,7,0,
28,130,75,102,225,24,20,36,184,132,170,110,238,72,242,175,226,78,35,0,230,105,147,228,84,223,181,155,210,65,96,110,188,249,131,183,25,134,48,60,29,223,31,0,204,215,201,36,111,84,117,243,84,233,32,48,55,
86,144,96,80,213,205,67,73,254,116,156,53,2,96,89,156,77,130,67,80,144,32,73,85,55,111,196,138,17,0,203,118,174,239,218,235,75,135,128,169,83,144,88,181,170,110,30,79,114,79,233,28,0,176,35,123,73,254,
72,223,181,79,148,14,2,83,165,32,177,90,38,212,1,176,98,46,152,133,203,80,144,88,157,170,110,30,76,242,23,226,172,17,0,235,182,73,242,158,190,107,207,150,14,2,83,162,32,177,42,206,26,1,192,59,184,55,9,
46,162,32,177,10,85,221,220,151,228,87,74,231,0,128,137,218,75,242,123,251,174,125,182,116,16,40,77,65,98,241,170,186,121,62,201,173,165,115,0,192,12,60,209,119,237,189,165,67,64,73,10,18,139,85,213,205,
245,73,206,198,150,58,0,56,12,91,238,88,53,5,137,69,170,234,230,217,36,239,47,157,3,0,102,236,11,125,215,222,85,58,4,236,154,130,196,226,24,196,0,0,91,99,53,137,213,81,144,88,140,170,110,30,77,242,67,
165,115,0,192,194,108,146,156,234,187,118,83,58,8,236,130,79,217,89,132,97,75,157,114,4,0,219,119,50,201,27,85,221,60,94,58,8,236,130,21,36,102,173,170,155,83,217,31,196,112,170,116,22,0,88,1,91,238,88,
60,5,137,217,170,234,230,197,36,167,75,231,0,128,21,250,84,223,181,159,41,29,2,198,160,32,49,75,85,221,188,146,228,198,210,57,0,96,197,206,244,93,123,75,233,16,176,109,10,18,179,83,213,205,235,177,165,
14,0,166,224,124,223,181,215,150,14,1,219,164,32,49,27,85,221,124,54,201,199,74,231,0,0,222,194,148,59,22,69,65,98,22,108,169,3,128,201,179,229,142,69,80,144,152,60,23,191,2,192,108,108,250,174,189,166,
116,8,56,14,5,137,201,170,234,230,238,36,191,94,58,7,0,112,40,74,18,179,166,32,49,73,85,221,60,157,228,142,210,57,0,128,35,187,179,239,218,103,74,135,128,195,82,144,152,28,231,141,0,96,49,158,235,187,
246,246,210,33,224,48,20,36,38,163,170,155,147,73,190,158,228,68,233,44,0,192,214,216,114,199,172,56,248,206,36,84,117,243,96,146,55,162,28,1,192,210,156,28,6,46,193,44,88,65,162,184,170,110,158,74,242,
61,165,115,0,0,163,115,46,137,201,83,144,40,170,170,155,23,147,156,46,157,3,0,216,153,207,247,93,123,119,233,16,112,57,10,18,197,184,223,8,0,86,235,108,223,181,55,149,14,1,151,162,32,177,115,85,221,220,
159,228,225,210,57,0,128,162,12,111,96,146,182,254,233,125,85,55,111,84,117,115,207,182,159,203,50,84,117,243,64,148,35,0,192,240,6,38,106,235,43,72,85,221,92,120,224,249,190,107,175,221,234,195,153,181,
170,110,30,77,242,67,165,115,0,0,147,99,120,3,147,49,102,65,186,224,165,190,107,111,222,234,139,48,59,85,221,124,53,201,123,75,231,0,182,234,237,63,239,223,252,128,44,251,119,154,93,236,235,195,223,191,
154,83,73,222,117,209,127,126,215,240,247,222,126,5,128,43,1,96,121,30,233,187,246,163,165,67,192,46,10,210,5,231,250,174,189,126,171,47,198,44,24,198,0,147,183,151,183,150,157,87,147,156,73,242,88,146,
207,37,249,151,125,215,158,41,17,236,48,134,203,166,191,61,251,43,213,247,39,249,96,246,63,152,185,38,111,45,84,202,21,76,151,15,214,41,110,151,5,233,130,39,250,174,189,119,171,47,202,100,41,71,80,220,
197,229,231,141,36,175,39,249,151,73,126,120,14,165,103,76,85,221,220,151,228,255,145,111,172,110,95,40,82,10,20,148,229,67,117,138,42,81,144,146,100,147,228,91,251,174,125,97,171,47,206,164,40,71,176,
83,155,236,23,160,77,146,151,147,252,165,190,107,63,83,54,210,124,85,117,115,58,201,63,78,242,31,38,185,54,202,19,236,154,9,119,20,83,170,32,93,96,6,254,2,13,147,234,254,90,233,28,176,64,23,74,208,235,
217,223,6,247,165,36,117,223,181,79,22,77,181,50,85,221,124,54,201,247,37,185,49,201,13,195,191,124,24,4,219,103,224,23,69,148,46,72,23,248,6,88,8,195,24,96,43,246,178,95,130,158,75,242,199,21,160,249,
24,86,158,254,74,146,31,141,226,4,219,242,97,63,7,217,165,169,20,164,11,206,244,93,123,203,214,194,176,83,85,221,60,149,228,123,74,231,128,153,185,176,42,244,82,146,15,245,93,251,108,217,56,140,161,170,
155,199,147,124,56,251,19,249,148,38,56,188,207,245,93,251,241,210,33,88,135,169,21,164,100,255,205,194,79,245,93,251,208,54,242,176,27,85,221,60,155,228,253,165,115,192,12,108,178,63,238,250,153,190,
107,239,42,29,134,50,170,186,185,63,251,3,34,172,50,193,193,61,214,119,237,125,165,67,176,124,99,20,164,77,182,115,136,245,92,246,7,57,172,122,202,210,28,216,86,7,87,180,201,254,121,161,223,86,136,184,
156,170,110,30,74,242,95,38,121,119,246,239,126,50,12,2,46,205,93,73,140,110,202,5,233,2,69,105,194,76,170,131,183,216,36,249,74,146,62,201,255,181,239,218,131,92,140,10,151,52,172,50,253,143,73,190,45,
38,232,193,197,28,201,96,84,115,40,72,23,152,120,55,49,202,17,188,185,58,244,199,251,174,125,164,116,24,150,173,170,155,27,147,60,155,228,125,81,152,192,93,73,140,102,78,5,233,130,47,216,166,82,158,114,
196,138,109,146,252,155,190,107,111,47,29,132,117,27,6,227,124,103,246,7,63,192,26,41,73,140,98,140,130,180,139,55,206,46,154,45,104,7,37,24,166,230,92,146,95,239,187,246,222,210,65,224,82,134,187,153,
126,60,251,151,218,250,249,204,154,188,212,119,237,205,165,67,176,44,115,45,72,23,156,77,242,221,198,226,238,70,85,55,247,37,249,149,210,57,96,7,54,73,190,156,228,51,125,215,126,166,116,24,56,140,225,
46,166,127,156,253,201,162,215,21,142,3,187,96,37,137,173,154,123,65,186,224,124,146,27,29,136,30,79,85,55,247,36,121,188,116,14,24,209,217,36,63,220,119,237,19,165,131,192,54,13,133,233,95,37,249,230,
88,93,98,185,246,250,174,181,245,159,173,88,74,65,186,224,133,190,107,111,43,244,218,139,85,213,205,237,73,190,84,58,7,108,217,133,251,136,62,96,187,46,107,49,148,165,223,74,242,158,40,75,44,80,223,181,
190,174,57,182,165,21,164,36,217,75,242,103,108,139,217,142,170,110,238,78,242,235,165,115,192,22,157,79,242,231,253,140,96,237,170,186,121,48,201,159,139,109,120,44,203,166,239,218,107,74,135,96,222,
150,88,144,46,56,159,228,61,125,215,190,86,58,200,92,13,191,60,127,166,116,14,216,130,77,146,175,184,55,3,46,109,152,136,247,93,153,198,239,111,56,46,37,137,99,89,114,65,186,224,124,146,255,212,185,130,
195,169,234,230,241,36,247,148,206,1,199,112,46,201,127,103,165,8,14,167,170,155,7,146,252,15,177,13,143,249,187,165,239,218,51,165,67,48,63,107,40,72,23,152,112,114,64,85,221,60,148,228,19,165,115,192,
17,156,79,242,63,247,93,251,209,210,65,96,9,134,157,4,63,157,105,254,94,135,131,184,215,135,228,28,214,154,10,210,5,143,245,93,123,95,233,16,83,53,124,114,248,215,74,231,128,67,58,151,228,155,251,174,
61,91,58,8,44,85,85,55,47,38,57,93,58,7,28,150,193,13,28,214,26,11,82,178,127,30,225,71,250,174,125,180,116,144,41,113,230,136,153,217,36,249,173,190,107,239,44,29,4,214,100,40,74,239,203,244,127,215,
195,5,70,128,115,40,107,45,72,23,156,79,242,126,35,126,147,170,110,158,76,242,161,210,57,224,42,54,73,190,146,253,15,56,158,44,29,6,214,174,170,155,103,147,124,123,156,85,98,250,246,146,188,219,240,46,
14,98,237,5,233,130,77,146,111,89,235,65,190,170,110,30,78,114,127,233,28,112,5,155,36,191,175,239,218,103,75,7,1,222,105,184,18,226,87,147,220,88,58,11,92,133,51,73,92,149,130,244,86,103,251,174,189,
169,116,136,93,170,234,230,147,73,126,174,116,14,184,140,151,250,174,189,185,116,8,224,224,170,186,249,106,76,192,99,194,156,73,226,106,198,40,72,175,39,57,181,213,135,238,214,38,201,79,245,93,251,80,
233,32,99,171,234,230,254,36,15,151,206,1,151,112,198,157,69,48,111,85,221,60,159,228,214,210,57,224,18,220,147,196,21,41,72,151,183,232,251,147,140,242,102,130,246,146,252,142,98,4,203,82,213,205,211,
73,190,35,243,221,93,194,50,41,73,92,150,130,116,117,139,155,120,87,213,205,103,147,124,172,116,14,24,156,77,242,158,190,107,55,165,131,0,227,169,234,230,100,146,127,27,171,74,76,199,94,146,235,250,174,
61,95,58,8,211,162,32,29,220,34,46,154,173,234,230,246,36,95,42,157,131,213,219,75,242,239,251,174,189,173,116,16,96,247,108,191,99,74,156,73,226,237,20,164,195,123,174,239,218,219,75,135,56,138,170,110,
174,79,242,106,233,28,172,158,243,69,64,18,151,207,50,25,182,219,241,22,10,210,209,108,146,124,255,156,206,39,85,117,115,42,201,235,165,115,176,106,38,210,1,151,164,40,49,1,171,155,100,204,229,141,81,
144,94,75,114,221,86,31,58,93,231,147,252,247,125,215,62,88,58,200,149,84,117,115,58,201,139,165,115,176,90,143,245,93,123,95,233,16,192,244,85,117,243,104,146,31,136,129,14,148,161,36,145,68,65,218,150,
77,146,235,167,122,200,175,170,155,237,254,33,195,213,109,146,252,120,223,181,143,148,14,2,204,79,85,55,15,36,249,171,81,148,216,189,243,125,215,94,91,58,4,101,41,72,219,53,185,179,21,43,217,242,200,116,
236,37,249,133,190,107,63,94,58,8,48,127,85,221,60,156,228,254,210,57,88,157,69,12,230,226,232,20,164,237,219,75,242,203,125,215,126,180,116,16,127,22,236,216,228,62,32,0,150,161,170,155,39,147,124,168,
116,14,86,101,182,67,185,56,62,5,105,60,231,147,252,151,165,182,24,85,117,243,74,146,27,75,188,54,171,163,24,1,59,97,152,3,59,246,68,223,181,247,150,14,193,238,41,72,227,219,75,242,15,119,249,13,102,91,
29,59,242,133,190,107,239,42,29,2,88,159,170,110,30,79,242,125,113,70,137,241,249,93,183,66,99,20,36,43,23,151,182,147,201,40,46,223,99,7,172,24,1,147,96,69,137,29,121,100,10,71,39,216,29,5,105,247,158,
233,187,246,206,49,30,92,213,205,67,73,62,49,198,179,33,46,210,3,38,202,206,9,118,224,206,190,107,159,41,29,130,221,176,52,189,123,119,84,117,179,169,234,230,147,219,124,104,85,55,247,71,57,98,28,123,
73,62,175,28,1,83,53,140,101,254,84,246,175,24,128,49,252,102,233,0,236,142,21,164,178,54,73,254,70,223,181,15,28,231,33,85,221,124,54,201,199,182,19,9,222,180,149,175,79,128,93,26,126,39,254,68,124,8,
204,246,217,73,177,18,10,210,52,108,146,124,111,223,181,79,30,246,191,88,213,205,29,73,158,222,126,36,86,238,133,190,107,111,43,29,2,224,168,170,186,121,52,201,15,149,206,193,226,40,73,43,160,32,77,203,
161,15,191,87,117,179,73,114,98,164,60,172,207,78,134,137,0,236,138,233,186,140,192,69,178,11,103,249,121,90,78,15,231,147,62,123,144,255,227,161,140,42,71,108,203,167,149,35,96,105,134,55,178,143,101,
255,60,37,108,195,117,195,123,48,22,202,10,210,116,109,146,252,84,223,181,15,93,234,31,86,117,243,213,36,239,221,105,34,150,202,216,110,96,21,170,186,121,58,201,29,165,115,176,24,143,245,93,123,95,233,
16,108,159,130,52,125,155,36,127,182,239,218,207,92,248,27,254,55,102,75,94,234,187,246,230,210,33,0,118,205,239,81,182,232,39,47,247,97,54,243,165,32,205,199,38,201,247,39,249,187,177,151,154,227,217,
75,242,99,125,215,62,82,58,8,64,41,195,144,163,223,140,173,234,28,83,223,181,190,134,22,70,65,130,117,49,132,1,224,34,85,221,188,152,228,116,233,28,204,218,94,223,181,206,245,47,200,24,127,152,95,31,225,
153,192,241,236,101,127,27,128,114,4,112,145,225,12,230,231,99,136,3,71,119,162,170,155,55,74,135,96,123,198,88,65,250,106,12,15,128,169,216,75,242,175,251,174,189,189,116,16,128,169,171,234,230,241,36,
247,148,206,193,108,25,255,189,16,99,20,36,75,213,48,13,126,80,3,28,129,187,147,56,134,47,244,93,123,87,233,16,28,143,253,146,176,76,79,40,71,0,71,51,252,252,124,174,116,14,102,233,123,170,186,49,250,
123,230,198,88,65,114,199,0,148,179,151,228,135,251,174,125,180,116,16,128,37,24,206,150,248,64,153,195,186,182,239,218,243,165,67,112,52,99,124,195,191,52,194,51,129,171,123,161,239,218,147,202,17,192,
246,244,93,123,77,246,135,56,192,97,188,94,58,0,71,55,70,65,250,202,8,207,4,46,239,124,223,181,39,250,174,189,173,116,16,128,37,234,187,246,238,36,215,100,255,78,66,56,16,147,237,230,107,140,130,100,207,
46,236,206,75,125,215,94,91,58,4,192,210,245,93,187,25,86,147,190,80,58,11,179,113,114,56,122,194,204,140,81,144,158,25,225,153,192,59,125,186,239,218,155,75,135,0,88,147,97,66,217,157,165,115,48,27,119,
24,218,48,63,99,12,105,184,47,201,175,108,245,161,192,197,46,124,138,9,64,65,85,221,188,158,228,84,233,28,204,194,53,125,215,218,162,57,19,206,32,193,188,60,161,28,1,76,195,176,197,249,115,217,159,32,
10,87,226,60,210,140,140,177,130,116,107,146,231,183,250,80,192,170,17,192,132,185,92,150,3,240,187,124,38,198,88,65,250,218,8,207,132,53,123,201,15,84,128,105,27,46,151,61,91,58,7,147,118,178,170,27,
139,8,51,48,70,65,122,117,132,103,194,90,61,103,16,3,192,60,244,93,123,83,146,71,74,231,96,210,110,29,118,91,49,97,91,223,98,151,36,85,221,216,139,11,199,179,73,242,189,125,215,62,89,58,8,0,135,103,128,
3,87,176,215,119,237,24,139,20,108,137,130,4,211,115,166,239,218,91,74,135,0,224,120,170,186,121,49,201,233,210,57,152,36,231,145,38,76,123,133,105,121,68,57,2,88,134,225,231,249,207,151,206,193,36,157,
172,234,230,149,210,33,184,52,43,72,48,29,143,244,93,251,209,210,33,0,216,190,170,110,54,73,78,148,206,193,228,248,221,63,65,86,144,160,188,77,146,27,252,128,4,88,174,225,204,201,185,210,57,152,156,251,
75,7,224,157,20,36,40,235,185,190,107,175,233,187,246,181,210,65,0,24,215,48,10,252,153,210,57,152,150,170,110,92,34,59,49,10,18,148,243,185,190,107,111,47,29,2,128,221,233,187,246,206,36,31,46,157,131,
73,57,89,213,205,211,165,67,240,13,206,32,65,25,63,217,119,237,67,165,67,0,80,142,115,73,188,205,7,250,174,125,182,116,8,172,32,193,174,237,37,185,86,57,2,96,56,151,180,41,157,131,201,248,82,233,0,236,
83,144,96,119,206,246,93,123,178,239,218,243,165,131,0,48,13,195,93,56,103,74,231,96,26,134,11,134,41,76,65,130,221,56,211,119,237,77,165,67,0,48,61,238,75,226,34,167,170,186,121,170,116,136,181,27,171,
32,57,131,4,223,240,5,151,191,2,112,37,125,215,62,144,228,3,165,115,48,9,223,83,58,192,218,89,65,130,113,125,170,239,218,187,74,135,0,96,250,134,3,250,62,80,195,86,187,194,198,154,98,103,42,11,107,183,
151,228,63,232,187,214,190,114,0,14,109,184,27,199,7,217,235,118,198,14,148,50,124,227,193,246,109,134,97,12,202,17,0,71,98,120,3,73,78,87,117,243,112,233,16,107,164,32,193,118,109,134,95,106,0,112,44,
195,234,193,11,165,115,80,212,253,165,3,172,145,130,4,219,115,86,57,2,96,155,250,174,189,45,201,217,210,57,40,167,170,155,215,74,103,88,27,5,9,182,227,37,99,188,1,24,195,240,251,197,118,187,245,186,174,
170,155,123,74,135,88,19,99,190,225,248,94,232,187,246,230,210,33,0,88,174,97,187,221,71,74,231,160,152,95,43,29,96,77,172,32,193,241,60,54,108,127,0,128,81,245,93,251,104,146,155,226,131,232,53,58,81,
213,205,43,165,67,172,133,130,4,71,247,233,190,107,239,43,29,2,128,245,232,187,246,108,146,255,160,116,14,138,184,209,84,187,221,80,144,224,104,62,213,119,237,131,165,67,0,176,62,195,53,18,118,47,172,
147,169,118,59,48,214,69,177,46,55,99,201,62,173,28,1,48,5,222,115,173,146,43,69,70,230,27,10,14,231,94,229,8,128,169,24,222,40,159,43,157,131,157,58,105,171,221,184,20,36,56,184,107,251,174,125,162,116,
8,0,184,88,223,181,215,39,217,148,206,193,78,253,104,233,0,75,166,32,193,193,220,208,119,237,249,210,33,0,224,82,134,149,36,211,237,214,227,68,85,55,47,150,14,177,84,10,18,92,221,189,125,215,186,197,26,
128,73,235,187,246,100,172,36,173,201,233,170,110,76,211,29,129,130,4,87,246,1,219,234,0,152,139,97,37,233,108,233,28,236,204,175,148,14,176,68,10,18,92,222,53,125,215,62,91,58,4,0,28,70,223,181,55,197,
224,134,213,168,234,230,245,210,25,150,70,65,130,75,187,182,239,90,219,20,0,152,37,131,27,86,229,84,85,55,15,149,14,177,36,10,18,188,211,157,6,50,0,48,119,195,118,59,37,105,29,254,84,233,0,75,162,32,193,
91,125,170,239,218,103,74,135,0,128,109,80,146,86,227,100,85,55,79,150,14,177,20,99,21,164,55,70,122,46,140,233,153,190,107,63,83,58,4,0,108,147,203,100,87,227,67,165,3,44,133,21,36,248,134,207,151,14,
0,0,99,24,206,36,185,39,105,225,170,186,113,45,201,22,40,72,240,13,31,115,200,17,128,165,26,238,73,98,217,174,243,94,230,248,124,163,192,91,125,162,170,155,215,170,186,185,189,116,16,0,24,193,77,165,3,
48,186,79,148,14,48,119,39,246,246,182,191,218,58,204,99,63,181,245,7,195,110,109,146,252,108,223,181,15,150,14,2,0,219,84,213,205,38,201,137,210,57,24,205,249,190,107,175,45,29,98,174,172,32,193,229,
157,76,242,51,85,221,124,181,116,16,0,216,178,119,151,14,192,168,78,217,13,115,116,10,18,92,221,123,171,186,217,171,234,230,209,210,65,0,96,27,250,174,125,45,201,35,165,115,48,170,223,44,29,96,174,20,
36,56,184,31,170,234,230,141,170,110,238,47,29,4,0,142,171,239,218,143,38,121,161,116,14,70,115,93,85,55,15,148,14,49,71,10,18,28,206,201,36,15,87,117,243,186,162,4,192,220,245,93,123,91,92,115,177,100,
255,83,233,0,115,52,214,144,134,215,146,92,183,245,7,195,244,108,146,124,107,223,181,62,129,3,96,182,170,186,121,58,201,29,165,115,48,138,151,250,174,189,185,116,136,57,177,130,4,199,115,50,201,243,85,
221,188,88,58,8,0,28,85,223,181,119,38,121,169,116,14,70,241,222,170,110,110,44,29,98,78,20,36,216,142,211,85,221,108,170,186,121,176,116,16,0,56,138,97,149,97,83,58,7,163,248,90,233,0,115,50,86,65,82,
188,88,163,19,217,31,11,254,70,85,55,247,149,14,3,0,135,213,119,237,53,73,182,127,254,130,210,78,86,117,243,80,233,16,115,49,86,145,241,233,3,107,118,50,201,175,12,69,201,244,24,0,102,165,239,90,31,116,
47,211,159,42,29,96,46,124,3,192,120,78,38,249,107,85,221,188,94,58,8,0,28,210,77,165,3,176,117,39,171,186,121,184,116,136,57,80,144,96,124,167,134,139,102,159,42,29,4,0,14,162,239,218,179,73,62,87,58,
7,91,247,163,165,3,204,193,88,5,233,235,35,61,23,230,236,123,134,109,119,159,44,29,4,0,174,166,239,218,143,39,57,91,58,7,91,117,194,228,221,171,179,130,4,187,117,50,201,207,13,69,233,209,210,97,0,224,
74,250,174,189,41,74,210,210,156,46,29,96,234,198,186,40,246,197,248,31,31,14,98,147,228,61,195,86,6,0,152,164,170,110,222,136,15,214,151,228,124,223,181,215,150,14,49,85,99,125,161,159,31,233,185,176,
52,39,147,188,82,213,205,43,165,131,0,192,229,24,255,189,56,167,220,221,120,121,99,21,164,151,71,122,46,44,213,141,195,69,179,182,221,1,48,85,223,89,58,0,91,245,211,165,3,76,213,88,5,233,220,72,207,133,
37,59,145,228,135,134,243,73,198,112,2,48,41,125,215,62,147,228,243,165,115,176,53,39,171,186,185,167,116,136,41,26,171,32,253,239,35,61,23,214,224,100,146,251,135,21,37,69,9,128,201,232,187,246,238,24,
218,176,36,255,160,116,128,41,26,171,32,253,206,72,207,133,53,57,145,253,162,244,90,233,32,0,112,193,48,217,110,83,58,7,91,113,178,170,155,7,74,135,152,154,177,10,146,249,234,176,61,215,13,23,205,250,
190,2,96,18,134,161,13,44,195,95,45,29,96,106,198,42,72,47,140,244,92,88,179,211,195,182,187,207,150,14,2,0,73,126,178,116,0,182,226,100,85,55,143,151,14,49,37,10,18,204,203,137,36,31,27,138,146,21,37,
0,138,233,187,246,161,36,95,40,157,131,173,48,172,225,34,99,21,164,231,70,122,46,176,239,68,246,87,148,222,168,234,230,142,210,97,0,88,167,190,107,239,138,243,72,139,224,78,198,111,24,171,32,125,101,164,
231,2,111,117,50,201,211,85,221,124,181,116,16,0,214,201,121,164,197,184,177,116,128,169,24,171,32,125,117,164,231,2,151,246,222,97,144,195,211,165,131,0,176,74,159,46,29,128,227,171,234,230,217,210,25,
166,96,172,130,244,242,72,207,5,174,236,142,161,40,61,89,58,8,0,235,209,119,237,131,73,158,41,157,131,99,123,127,233,0,83,48,86,65,58,63,210,115,129,131,249,208,48,200,65,81,2,96,39,250,174,189,51,46,
145,157,61,187,81,198,43,72,14,121,65,121,39,178,95,148,222,40,29,4,128,117,112,137,236,34,252,254,210,1,74,27,171,32,189,62,210,115,129,195,59,57,108,187,123,190,116,16,0,86,225,111,149,14,192,177,156,
88,251,42,210,137,189,189,189,81,30,92,213,205,56,15,6,142,235,243,125,215,222,93,58,4,0,203,53,140,140,54,21,109,198,250,174,61,81,58,67,41,10,18,172,215,115,125,215,222,94,58,4,0,203,52,108,241,30,107,
183,18,227,59,59,108,153,92,29,95,180,176,94,239,31,6,57,124,178,116,16,0,150,199,253,72,179,183,218,21,64,5,9,214,237,68,146,159,171,234,198,185,65,0,198,240,88,233,0,28,221,176,85,114,117,20,36,32,73,
78,185,104,22,128,109,235,187,246,190,152,106,55,103,171,92,69,82,144,128,139,93,184,104,246,197,210,65,0,88,6,91,237,230,173,170,155,135,75,103,216,53,5,9,184,148,211,67,81,122,180,116,16,0,22,225,153,
210,1,56,178,31,45,29,96,215,20,36,224,74,126,168,170,155,55,170,186,185,181,116,16,0,230,171,239,218,59,99,171,221,92,157,168,234,230,254,210,33,118,73,65,2,174,230,100,146,231,171,186,121,173,116,16,
0,230,203,86,187,89,251,165,210,1,118,73,65,2,14,234,186,97,219,221,179,165,131,0,48,91,143,148,14,192,145,156,92,211,181,32,10,18,112,88,239,31,138,210,42,71,127,2,112,116,125,215,126,52,201,217,210,
57,56,146,255,91,233,0,187,162,32,1,71,117,163,139,102,1,56,172,190,107,111,74,178,87,58,7,135,118,178,170,155,7,75,135,216,5,5,9,56,14,23,205,2,112,20,95,44,29,128,35,249,233,210,1,118,225,196,222,222,
56,5,190,170,27,159,12,192,250,188,212,119,237,205,165,67,0,48,125,85,221,188,17,31,214,207,209,53,125,215,46,122,34,225,152,95,148,10,18,172,207,123,135,109,119,206,39,1,112,53,215,151,14,192,145,124,
185,116,128,177,141,89,144,78,140,248,108,96,186,78,100,255,124,210,94,85,55,47,150,14,3,192,52,245,93,123,62,201,231,75,231,224,208,78,151,14,48,54,43,72,192,152,78,15,23,205,222,81,58,8,0,211,211,119,
237,221,113,129,236,236,84,117,243,104,233,12,99,178,239,19,24,219,201,36,79,219,118,7,192,101,188,167,116,0,14,237,7,74,7,24,147,130,4,236,202,133,109,119,95,45,29,4,128,233,232,187,246,108,146,51,165,
115,112,40,39,171,186,185,191,116,136,177,40,72,192,174,189,215,69,179,0,92,172,239,218,91,74,103,224,208,126,169,116,128,177,40,72,64,41,23,86,148,158,46,29,4,128,73,120,161,116,0,14,229,100,85,55,183,
150,14,49,6,5,9,40,237,142,170,110,94,171,234,230,84,233,32,0,148,211,119,237,109,49,228,107,110,254,109,233,0,99,80,144,128,41,184,46,201,235,85,221,188,162,40,1,172,218,31,41,29,128,67,89,100,151,56,
177,183,55,78,81,175,234,102,19,119,33,1,71,115,62,201,245,75,191,169,27,128,119,26,238,208,91,252,93,59,11,242,82,223,181,55,151,14,177,77,139,108,125,192,236,157,74,242,134,139,102,1,214,103,24,216,
96,171,221,124,44,110,76,187,139,98,129,41,59,93,213,205,166,170,155,207,150,14,2,192,78,253,245,210,1,56,176,19,75,27,214,96,5,9,152,186,19,73,62,54,156,79,242,51,11,96,5,250,174,125,32,137,109,214,243,
241,197,210,1,182,201,155,13,96,46,222,157,228,31,148,14,1,192,206,252,98,233,0,28,216,141,165,3,108,211,152,67,26,222,136,2,6,108,199,217,190,107,111,42,29,2,128,221,242,126,114,86,190,208,119,237,93,
165,67,108,131,47,56,96,202,54,73,110,80,142,0,86,235,91,74,7,224,192,190,171,116,128,109,81,144,128,169,122,166,239,218,107,250,174,125,173,116,16,0,202,232,187,246,76,146,179,165,115,112,32,39,171,186,
185,191,116,136,109,80,144,128,169,57,151,228,154,190,107,239,44,29,4,128,242,236,34,152,149,191,89,58,192,54,40,72,192,84,156,75,114,75,223,181,46,136,5,224,237,30,43,29,128,3,57,85,213,205,237,165,67,
28,151,130,4,76,193,207,15,197,232,76,233,32,0,76,79,223,181,247,197,216,239,185,248,231,165,3,28,215,187,74,7,0,86,237,92,223,181,215,151,14,1,192,44,252,189,36,63,84,58,4,87,245,238,210,1,142,203,10,
18,80,194,38,201,71,149,35,0,14,106,88,69,98,250,78,204,125,155,157,21,36,96,151,246,146,252,197,190,107,31,44,29,4,128,89,122,46,201,251,75,135,224,170,62,159,228,150,210,33,142,74,65,2,118,229,25,147,
233,0,56,142,190,107,111,175,234,102,147,228,68,233,44,92,209,55,151,14,112,28,182,216,1,99,59,215,119,237,9,229,8,128,45,249,225,210,1,184,170,19,85,221,220,81,58,196,81,41,72,192,152,206,56,103,4,192,
54,245,93,251,104,76,180,155,131,255,111,233,0,71,101,139,29,48,134,151,250,174,189,185,116,8,0,22,235,247,37,249,82,233,16,92,209,141,165,3,28,149,21,36,96,155,46,108,167,187,185,116,16,0,150,171,239,
218,103,179,127,193,56,19,86,213,205,179,165,51,28,133,130,4,108,195,94,146,159,180,157,14,128,29,186,171,116,0,174,234,219,74,7,56,138,19,123,123,123,163,60,184,170,155,215,147,156,26,229,225,192,148,
188,208,119,237,109,165,67,0,176,62,85,221,188,17,31,248,79,221,109,125,215,190,80,58,196,97,140,249,5,229,240,28,44,219,38,201,135,149,35,0,10,250,55,165,3,112,85,95,44,29,224,176,198,44,72,95,31,241,
217,64,57,155,236,111,167,187,166,239,218,39,75,135,1,96,189,250,174,189,61,251,219,188,153,174,217,13,107,176,36,9,28,198,19,67,49,122,168,116,16,0,24,252,66,233,0,92,89,85,55,159,45,157,225,48,172,32,
1,7,177,25,166,211,221,91,58,8,0,92,172,239,218,143,199,209,142,169,251,207,74,7,56,140,49,11,210,249,17,159,13,236,198,94,146,207,245,93,123,77,233,32,0,112,5,127,171,116,0,174,232,155,74,7,56,140,49,
167,216,61,159,228,214,81,30,14,236,194,115,195,222,110,0,152,60,19,237,38,239,154,190,107,103,177,210,247,174,17,159,253,114,20,36,152,163,151,92,244,10,192,12,253,141,36,159,40,29,130,203,250,183,73,
102,49,249,86,203,6,46,216,36,185,73,57,2,96,142,250,174,125,32,38,218,77,217,183,148,14,112,80,99,174,32,157,27,241,217,192,118,157,237,187,246,166,210,33,0,224,152,254,117,146,247,151,14,193,37,205,
102,97,102,204,160,175,141,248,108,96,59,206,103,127,213,72,57,2,96,9,126,127,233,0,92,94,85,55,47,150,206,112,16,99,22,164,87,71,124,54,112,60,231,147,220,208,119,237,181,125,215,158,45,29,6,0,182,161,
239,218,243,49,73,121,202,222,87,58,192,65,204,102,169,11,216,154,199,134,98,100,149,23,128,37,106,74,7,224,178,78,86,117,115,125,233,16,87,51,102,65,250,157,17,159,13,28,222,75,195,101,175,247,149,14,
2,0,99,233,187,246,161,184,56,118,202,254,125,233,0,87,51,102,65,154,197,30,67,88,129,189,36,159,50,157,14,128,21,249,197,210,1,184,172,201,95,26,107,139,29,44,215,94,146,207,247,93,123,178,239,218,207,
148,14,3,0,187,210,119,237,199,99,228,247,84,157,172,234,230,147,165,67,92,201,152,99,190,159,25,241,217,192,149,61,211,119,237,157,165,67,0,64,65,191,156,228,254,210,33,184,164,79,39,153,236,135,183,
99,174,32,189,50,226,179,129,75,219,100,127,58,157,114,4,192,170,245,93,251,209,210,25,184,172,27,74,7,184,146,49,87,144,254,127,35,62,27,120,167,207,247,93,123,119,233,16,0,48,33,103,146,156,46,29,130,
119,152,244,49,31,83,236,96,254,206,14,211,233,148,35,0,120,171,219,75,7,224,210,170,186,121,160,116,134,203,25,179,32,185,99,5,198,117,46,201,181,125,215,222,84,58,8,0,76,209,112,25,186,97,13,211,244,
63,148,14,112,57,99,110,177,251,234,136,207,134,53,219,75,242,99,125,215,62,82,58,8,0,204,192,239,38,185,177,116,8,222,225,61,165,3,92,206,152,5,233,245,17,159,13,107,117,214,138,17,0,28,92,223,181,55,
85,117,99,21,105,122,78,148,14,112,57,99,110,177,59,55,226,179,97,109,54,73,62,170,28,1,192,145,156,47,29,128,119,170,234,230,169,210,25,46,101,204,21,36,95,136,112,124,155,36,63,213,119,237,67,165,131,
0,192,140,253,167,73,30,47,29,130,119,248,174,210,1,46,101,204,130,244,234,136,207,134,53,120,161,239,218,219,74,135,0,128,185,235,187,246,137,170,110,54,153,248,120,233,21,154,228,159,199,152,161,222,
24,241,217,176,100,231,134,177,221,202,17,0,108,207,63,42,29,128,119,170,234,230,209,210,25,222,110,146,173,13,86,236,115,125,215,94,95,58,4,0,44,77,223,181,247,198,200,239,41,250,163,165,3,188,157,41,
118,48,13,103,250,174,189,165,116,8,0,88,56,35,191,167,103,204,62,114,36,163,173,32,245,93,187,25,235,217,176,32,47,13,219,233,148,35,0,24,223,237,165,3,240,14,39,170,186,121,176,116,136,139,217,98,7,
101,108,146,220,217,119,237,205,165,131,0,192,90,244,93,123,38,251,191,131,153,150,79,149,14,112,177,201,45,105,193,10,152,78,7,0,229,124,37,201,233,210,33,120,139,27,74,7,184,152,130,4,187,115,190,239,
218,107,75,135,0,128,149,251,75,73,126,174,116,8,222,98,82,187,218,38,21,6,22,106,147,228,35,202,17,0,148,215,119,237,103,74,103,224,157,170,186,121,184,116,134,11,172,32,193,184,62,215,119,237,199,75,
135,0,0,222,226,108,76,179,155,154,63,92,58,192,5,10,18,140,227,108,223,181,55,149,14,1,0,92,210,15,39,121,188,116,8,222,226,189,165,3,92,96,139,29,108,215,94,146,71,148,35,0,152,174,190,107,159,136,105,
118,83,115,170,116,128,11,172,32,193,118,236,37,249,167,125,215,222,93,58,8,0,112,32,166,217,77,76,85,55,167,250,174,61,95,58,135,21,36,56,190,151,250,174,61,169,28,1,192,124,184,164,125,146,254,126,233,
0,137,21,36,56,14,99,187,1,96,222,54,177,96,48,37,223,91,58,64,226,11,2,142,234,9,229,8,0,102,239,95,148,14,192,91,76,98,241,70,65,130,195,57,159,228,154,190,107,239,45,29,4,0,56,158,190,107,239,42,157,
129,183,56,81,58,64,162,32,193,65,157,77,114,67,223,181,215,246,93,107,234,13,0,44,135,223,235,19,82,213,205,179,165,51,76,98,25,11,38,108,47,201,95,236,187,246,193,210,65,0,128,81,252,70,146,15,149,14,
193,155,110,43,29,64,65,130,203,115,217,43,0,44,223,61,73,94,45,29,130,55,21,239,39,182,216,193,59,109,146,220,166,28,1,192,242,245,93,251,90,233,12,188,197,137,170,110,138,94,26,91,188,161,193,132,236,
37,249,133,190,107,63,94,58,8,0,176,83,231,147,20,125,83,206,91,60,159,164,216,61,85,10,18,236,251,188,139,94,1,96,181,206,36,185,181,116,8,222,244,222,146,47,174,32,177,118,231,250,174,189,190,116,8,
0,160,156,190,107,111,171,234,102,175,116,14,222,84,180,163,56,131,196,154,61,163,28,1,0,3,227,190,167,163,232,57,36,43,72,172,145,233,116,0,192,219,125,37,201,233,210,33,120,211,115,41,52,242,219,10,
18,107,114,182,239,218,19,202,17,0,240,118,125,215,22,27,10,192,37,189,167,212,11,43,72,172,193,38,201,7,20,35,0,224,42,108,179,155,142,27,74,189,176,45,118,44,221,51,125,215,222,89,58,4,0,48,11,47,167,
240,4,53,222,116,162,212,11,143,189,130,100,26,8,165,92,184,236,85,57,2,0,14,234,59,74,7,224,77,139,45,72,176,107,155,36,63,217,119,237,53,125,215,190,80,58,12,0,48,31,125,215,158,137,109,118,147,81,213,
77,145,59,42,109,177,99,73,30,235,187,246,190,210,33,0,128,89,59,159,228,186,210,33,72,146,252,229,36,247,238,250,69,21,36,150,224,124,223,181,215,150,14,1,0,44,194,47,37,249,88,233,16,36,73,62,88,226,
69,109,177,99,206,246,146,124,74,57,2,0,182,165,239,218,143,151,206,192,155,222,93,226,69,173,32,49,87,95,232,187,246,174,210,33,0,0,24,77,145,65,13,10,18,115,115,198,69,110,0,192,200,206,39,57,85,58,
4,101,24,243,205,92,156,79,114,141,114,4,0,236,192,243,165,3,176,175,170,155,135,118,253,154,86,144,152,3,171,70,0,192,46,61,150,228,19,165,67,144,100,127,96,198,3,187,124,65,67,26,152,178,243,125,215,
158,80,142,0,128,93,234,187,118,167,111,200,185,162,27,118,253,130,10,18,83,180,73,114,131,233,116,0,64,65,142,138,76,195,206,7,53,140,189,197,238,141,40,97,28,220,94,146,95,238,187,246,163,165,131,0,
0,171,247,187,73,110,44,29,130,221,23,36,229,133,169,56,211,119,237,73,229,8,0,152,136,127,84,58,0,251,170,186,121,120,151,175,55,118,65,218,140,252,124,230,111,47,201,189,206,25,1,0,83,210,119,237,125,
165,51,240,166,31,220,229,139,153,98,71,41,123,73,254,105,223,181,119,151,14,2,0,112,25,155,216,113,53,5,59,61,151,174,32,81,194,51,125,215,222,89,58,4,0,192,85,156,79,114,93,233,16,228,154,93,190,152,
130,196,46,109,250,174,221,233,23,56,0,192,49,252,90,146,31,42,29,130,221,174,226,141,253,98,95,31,249,249,204,199,99,202,17,0,48,39,206,33,77,71,85,55,59,59,150,97,5,137,177,157,237,187,246,166,210,33,
0,0,142,200,57,164,105,248,219,73,118,50,212,203,31,54,99,57,155,228,26,229,8,0,152,185,55,74,7,32,73,114,195,174,94,200,10,18,219,182,73,242,253,125,215,62,81,58,8,0,192,22,124,41,201,29,165,67,176,187,
73,118,10,18,219,244,82,223,181,55,151,14,1,0,176,69,127,36,201,243,165,67,176,187,73,118,99,111,177,123,117,228,231,51,13,155,36,183,41,71,0,192,210,244,93,251,66,233,12,36,217,225,209,32,103,144,56,
142,77,146,143,246,93,123,141,31,30,0,192,130,237,149,14,64,82,213,205,245,187,120,29,91,236,56,170,207,247,93,187,179,113,139,0,0,5,125,61,201,169,210,33,200,71,146,60,50,246,139,40,72,28,150,177,221,
0,192,218,24,212,48,13,63,150,29,20,36,91,236,56,140,79,43,71,0,192,10,253,124,233,0,36,73,238,218,197,139,140,189,130,116,126,228,231,179,27,103,250,174,221,201,197,92,0,0,19,244,11,73,126,174,116,8,
114,235,46,94,196,22,59,174,68,49,2,0,86,175,239,218,23,170,186,41,29,131,228,155,118,241,34,10,18,151,178,73,242,158,190,107,207,150,14,2,0,48,17,123,73,78,148,14,177,114,59,185,44,118,236,130,244,114,
118,180,20,198,214,60,215,119,237,237,165,67,0,0,76,140,130,84,222,78,254,247,183,130,196,5,231,250,174,221,201,108,121,0,128,25,58,159,228,186,210,33,24,159,41,118,108,146,124,68,57,2,0,184,162,175,151,
14,192,110,140,93,144,206,141,252,124,142,110,47,201,167,250,174,189,166,239,218,71,75,135,1,0,152,184,223,46,29,128,164,170,155,219,199,126,13,43,72,235,116,182,239,218,147,125,215,126,166,116,16,0,128,
57,232,187,118,39,119,240,112,85,255,245,216,47,48,118,65,122,109,228,231,115,56,155,36,31,117,217,43,0,0,51,245,95,143,253,2,134,52,172,195,94,146,127,218,119,237,221,165,131,0,0,204,216,38,118,96,149,
118,219,216,47,160,32,45,159,203,94,1,0,182,195,36,187,242,174,25,251,5,198,46,72,175,142,252,124,46,239,124,223,181,59,185,76,11,0,96,37,190,30,5,105,241,44,17,46,211,99,202,17,0,192,214,253,203,210,
1,24,255,178,88,43,72,203,226,178,87,0,128,241,252,161,36,175,151,14,177,114,163,23,36,43,72,203,112,54,201,53,202,17,0,192,120,250,174,61,95,58,3,227,27,187,32,253,238,200,207,95,187,189,36,31,233,187,
246,166,190,107,55,165,195,0,0,172,128,247,92,133,85,117,115,235,152,207,31,123,139,157,130,52,30,211,233,0,0,118,239,141,216,133,85,90,159,228,163,99,61,220,152,239,249,217,36,185,222,18,47,0,64,17,86,
144,202,251,195,99,62,124,236,130,116,118,228,231,175,201,38,201,223,234,187,118,180,182,12,0,192,85,25,245,93,222,55,141,249,112,43,72,243,240,68,223,181,247,150,14,1,0,64,126,35,201,61,165,67,172,220,
168,91,28,21,164,105,59,219,119,237,77,165,67,0,0,240,166,63,145,228,75,165,67,172,220,53,99,62,220,1,179,233,122,68,57,2,0,152,150,190,107,159,45,157,129,113,141,189,130,244,194,200,207,95,34,171,70,
0,0,80,136,45,118,211,161,24,1,0,192,213,157,24,243,225,99,111,177,123,121,228,231,47,193,38,201,45,202,17,0,192,108,236,149,14,176,114,163,22,36,43,72,101,125,161,239,218,187,74,135,0,0,224,80,246,50,
242,155,116,202,49,164,161,140,77,146,107,149,35,0,0,56,188,170,110,110,31,235,217,99,175,32,253,246,200,207,159,155,77,146,159,234,187,246,161,210,65,0,0,56,178,87,147,220,88,58,196,202,253,223,147,124,
116,140,7,219,98,183,27,123,73,254,122,223,181,15,148,14,2,0,192,177,253,118,146,239,41,29,98,229,254,240,88,15,86,144,198,103,58,29,0,192,178,252,147,40,72,165,221,48,214,131,199,62,131,244,234,200,207,
159,178,189,36,247,42,71,0,0,139,243,72,233,0,140,199,10,210,56,158,235,187,246,246,210,33,0,0,24,197,63,44,29,128,241,140,93,144,126,119,228,231,79,205,11,125,215,222,86,58,4,0,0,227,233,187,246,108,
85,55,165,99,172,221,104,61,198,10,210,118,156,235,187,246,250,210,33,0,0,96,37,70,59,42,228,30,164,227,123,78,57,2,0,128,157,186,102,172,7,143,189,130,244,218,200,207,47,201,170,17,0,0,44,204,216,5,233,
171,35,63,191,132,243,73,174,239,187,118,83,58,8,0,0,176,93,206,32,29,220,94,146,63,211,119,237,103,74,7,1,0,0,198,49,118,65,122,125,228,231,239,138,233,116,0,0,176,2,134,52,92,217,38,201,7,148,35,0,0,
222,198,113,139,178,78,140,245,224,177,87,144,94,25,249,249,99,217,75,242,197,190,107,239,44,29,4,0,0,216,157,177,11,210,87,70,126,254,24,190,208,119,237,93,165,67,0,0,48,105,111,196,110,172,69,26,181,
32,245,93,187,153,209,45,195,231,251,174,189,182,116,8,0,0,224,170,70,219,98,167,245,238,111,167,251,180,114,4,0,192,33,56,131,180,80,107,31,243,253,82,223,181,55,151,14,1,0,192,236,124,61,201,117,165,
67,172,89,85,55,39,199,184,155,116,173,43,72,103,147,92,171,28,1,0,192,108,189,111,140,135,174,109,5,105,147,228,91,251,174,125,161,116,16,0,0,102,237,235,165,3,144,155,146,156,217,246,67,215,84,144,108,
167,3,0,128,229,248,246,36,207,110,251,161,107,216,98,183,73,114,141,114,4,0,0,139,242,7,198,120,232,146,87,144,54,73,126,170,239,218,135,74,7,1,0,96,113,206,151,14,64,254,227,49,30,186,212,130,244,249,
190,107,239,46,29,2,0,128,197,58,85,58,0,121,255,24,15,221,69,65,218,203,136,23,57,189,205,153,190,107,111,217,209,107,1,0,0,229,220,48,198,67,151,114,6,105,47,201,71,148,35,0,0,118,196,22,187,242,126,
207,24,15,93,194,22,187,179,125,215,222,84,58,4,0,0,176,83,223,52,198,67,231,92,144,94,232,187,246,182,210,33,0,0,128,229,152,227,25,164,77,146,111,233,187,118,235,151,66,1,0,0,179,241,242,24,15,157,219,
10,210,19,125,215,222,91,58,4,0,0,80,220,191,31,227,161,115,41,72,231,250,174,189,190,116,8,0,0,96,50,126,103,140,135,78,125,138,221,185,236,79,167,83,142,0,0,128,139,61,53,198,67,167,186,130,180,73,242,
227,125,215,62,82,58,8,0,0,92,130,139,98,203,251,95,199,120,232,20,11,210,75,125,215,222,92,58,4,0,0,92,193,20,223,71,175,205,226,183,216,109,146,220,171,28,1,0,48,3,95,47,29,128,124,113,140,135,78,165,
249,126,190,239,218,187,75,135,0,0,128,3,154,202,251,232,53,251,218,24,15,45,253,7,235,178,87,0,0,224,40,94,29,227,161,165,10,146,177,221,0,0,204,89,233,133,134,213,235,187,118,51,198,115,75,156,65,250,
121,229,8,0,128,153,155,210,89,126,182,104,151,205,247,108,223,181,55,237,240,245,0,0,0,14,101,23,5,233,183,250,174,189,115,7,175,3,0,0,187,114,77,233,0,43,183,55,214,131,71,95,26,84,142,0,0,128,185,176,
119,18,0,0,152,155,249,174,32,1,0,192,2,121,31,189,80,254,96,1,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,192,220,188,49,214,131,21,36,0,0,96,110,54,99,61,88,65,2,0,128,67,168,234,230,214,210,25,200,
215,199,122,176,130,4,0,0,135,243,251,75,7,96,60,10,18,0,0,28,206,199,74,7,96,60,10,18,0,0,28,206,247,150,14,64,94,29,235,193,10,18,0,0,28,206,239,45,29,128,60,54,214,131,21,36,0,0,56,156,27,74,7,32,255,
205,88,15,86,144,0,0,128,89,233,187,246,204,88,207,86,144,0,0,224,112,78,148,14,192,120,20,36,0,0,128,129,130,4,0,0,135,99,5,169,172,189,49,31,174,32,1,0,0,12,20,36,0,0,56,160,170,110,238,46,157,1,43,
72,0,0,48,21,127,163,116,0,242,198,152,15,87,144,0,0,224,224,92,18,91,222,102,204,135,43,72,0,0,112,112,215,150,14,64,94,30,243,225,10,18,0,0,28,220,53,165,3,144,127,56,230,195,21,36,0,0,96,78,254,219,
49,31,174,32,1,0,192,193,121,255,92,88,223,181,207,142,249,124,127,192,0,0,112,0,85,221,156,42,157,129,241,41,72,0,0,112,48,191,81,58,0,227,222,129,148,40,72,0,0,112,80,119,148,14,128,130,4,0,0,83,225,
189,243,10,248,67,6,0,128,131,49,226,187,188,55,198,126,1,5,9,0,0,14,230,68,233,0,228,249,177,95,64,65,2,0,128,171,168,234,230,100,20,164,41,104,199,126,1,5,9,0,0,174,238,159,149,14,64,146,228,23,198,
126,1,5,9,0,0,174,238,247,150,14,64,210,119,237,11,99,191,134,130,4,0,0,87,247,174,210,1,216,13,5,9,0,0,174,238,84,233,0,236,134,130,4,0,0,87,103,64,67,121,163,95,18,155,40,72,0,0,112,16,10,82,121,191,
187,139,23,81,144,0,0,224,10,170,186,57,93,58,3,73,146,87,119,241,34,10,18,0,0,92,217,159,40,29,128,36,201,232,19,236,18,5,9,0,0,174,230,19,165,3,144,36,121,106,23,47,162,32,1,0,192,149,189,191,116,0,
146,36,159,221,197,139,40,72,0,0,112,101,215,150,14,64,146,228,215,118,241,34,10,18,0,0,92,153,9,118,19,208,119,237,107,187,120,29,5,9,0,0,46,163,170,155,91,75,103,32,73,178,217,213,11,41,72,0,0,112,121,
191,81,58,0,73,146,55,118,245,66,10,18,0,0,92,158,59,144,166,225,245,93,189,144,130,4,0,0,151,247,174,210,1,72,178,163,75,98,19,5,9,0,0,174,196,128,134,105,248,145,93,189,144,130,4,0,0,151,80,213,205,
147,165,51,176,175,239,218,157,253,89,40,72,0,0,112,105,119,149,14,64,146,29,78,176,75,20,36,0,0,184,156,83,165,3,176,123,10,18,0,0,92,154,247,202,211,176,179,1,13,137,63,116,0,0,120,135,170,110,30,47,
157,129,55,253,242,46,95,76,65,2,0,128,119,250,96,233,0,236,235,187,246,227,187,124,61,5,9,0,0,222,233,134,210,1,72,146,236,237,250,5,21,36,0,0,120,39,239,147,167,65,65,2,0,128,146,170,186,121,168,116,
6,222,180,211,1,13,137,130,4,0,0,111,247,67,165,3,240,166,207,237,250,5,21,36,0,0,120,171,223,83,58,0,251,250,174,125,96,215,175,169,32,1,0,192,91,93,91,58,0,229,40,72,0,0,240,86,39,74,7,32,73,178,41,
241,162,10,18,0,0,12,12,104,152,148,157,15,104,72,20,36,0,0,184,216,159,40,29,128,55,253,70,137,23,85,144,0,0,224,27,156,63,154,142,63,83,226,69,79,236,237,237,252,238,37,0,0,152,156,170,110,110,77,242,
124,233,28,236,235,187,182,200,89,48,43,72,0,0,176,239,139,165,3,240,166,98,171,56,10,18,0,0,236,187,161,116,0,222,164,32,1,0,64,97,222,27,79,71,145,9,118,137,47,2,0,0,72,85,55,47,150,206,192,91,124,173,
212,11,43,72,0,0,144,188,175,116,0,222,226,91,75,189,176,130,4,0,0,222,23,79,74,223,181,155,82,175,237,11,1,0,128,85,171,234,198,104,239,105,57,95,242,197,21,36,0,0,214,238,116,233,0,188,197,75,37,95,
92,65,2,0,96,237,78,149,14,192,91,124,91,201,23,87,144,0,0,88,173,170,110,110,44,157,129,183,216,235,187,246,181,146,1,20,36,0,0,214,236,159,148,14,192,91,124,189,116,0,5,9,0,128,53,251,174,210,1,120,
139,47,149,14,160,32,1,0,176,102,222,15,79,72,223,181,119,150,206,224,11,2,0,128,85,170,234,230,201,210,25,120,139,189,210,1,18,5,9,0,128,245,250,96,233,0,188,69,241,243,71,137,130,4,0,192,122,121,47,
60,45,255,172,116,128,196,23,5,0,0,43,84,213,205,139,165,51,240,14,255,73,233,0,137,130,4,0,192,58,189,175,116,0,222,170,239,218,77,233,12,137,130,4,0,192,202,84,117,115,95,188,15,158,154,243,165,3,92,
224,11,3,0,128,181,249,165,210,1,120,135,51,165,3,92,160,32,1,0,176,54,55,150,14,192,59,252,191,75,7,184,64,65,2,0,96,53,170,186,121,176,116,6,222,169,239,218,7,74,103,184,64,65,2,0,96,77,170,210,1,120,
135,73,12,103,184,64,65,2,0,96,77,76,175,155,158,87,75,7,184,152,130,4,0,192,42,84,117,115,58,222,255,78,209,207,148,14,112,49,95,32,0,0,172,197,179,165,3,240,14,123,125,215,126,166,116,136,139,41,72,
0,0,172,133,233,117,211,243,245,210,1,222,78,65,2,0,96,241,170,186,121,178,116,6,46,233,87,75,7,120,59,5,9,0,128,53,248,96,233,0,188,83,223,181,247,149,206,240,118,10,18,0,0,139,86,213,205,125,241,190,
119,138,206,151,14,112,41,190,80,0,0,88,186,255,87,233,0,92,210,63,43,29,224,82,222,85,58,0,229,85,117,243,116,146,239,72,242,70,223,181,215,150,206,3,0,176,101,167,74,7,224,157,250,174,189,187,116,134,
75,81,144,86,106,184,7,224,217,188,117,154,203,201,170,110,78,246,93,59,169,219,140,1,0,142,170,170,155,87,74,103,96,94,20,164,149,169,234,230,179,73,126,34,151,223,94,249,181,36,55,237,46,17,0,192,168,
222,93,58,0,151,116,166,116,128,203,81,144,86,160,170,155,167,146,124,103,14,182,188,236,126,0,0,96,17,134,29,51,39,74,231,224,146,62,81,58,192,229,156,216,219,219,43,157,129,17,84,117,115,42,201,255,
158,163,21,158,179,125,215,90,69,2,0,102,173,170,155,23,147,156,46,157,131,119,234,187,118,178,197,213,10,210,194,84,117,115,127,146,95,202,241,38,20,90,138,6,0,150,64,57,154,166,73,159,119,55,230,123,
33,170,186,121,182,170,155,55,146,60,156,227,255,185,158,168,234,230,129,45,196,2,0,40,162,170,155,135,75,103,224,178,94,45,29,224,74,172,32,205,216,48,112,225,191,200,56,163,43,63,147,228,161,17,158,
11,0,176,11,127,172,116,0,46,235,147,165,3,92,137,130,52,51,85,221,60,158,228,123,51,254,60,127,195,26,0,128,89,26,86,143,236,148,154,166,77,223,181,147,254,16,222,23,206,76,84,117,243,212,176,133,238,
158,236,232,178,179,161,140,1,0,204,141,213,163,233,122,185,116,128,171,177,130,52,113,195,229,102,165,86,115,190,175,208,235,2,0,28,201,112,142,218,34,192,116,253,158,210,1,174,198,152,239,9,170,234,
230,209,36,63,144,105,124,115,127,174,239,218,143,151,14,1,0,112,16,85,221,188,158,29,237,182,225,208,54,125,215,94,83,58,196,213,88,65,154,136,170,110,158,78,242,29,153,70,41,186,216,79,36,81,144,0,128,
201,171,234,230,198,40,71,83,246,229,210,1,14,98,106,111,198,87,167,170,155,87,170,186,217,75,114,71,166,249,231,113,114,88,209,2,0,152,186,47,150,14,192,229,245,93,123,91,233,12,7,97,5,169,128,225,50,
215,191,153,249,124,194,241,3,165,3,0,0,28,192,228,207,183,172,216,164,47,135,189,216,20,87,44,22,171,170,155,167,171,186,217,100,255,50,215,185,148,163,100,127,21,201,101,107,0,192,100,13,247,67,158,
40,157,131,203,154,197,246,186,196,10,210,232,170,186,121,50,201,31,204,188,10,209,165,24,151,9,0,76,217,79,148,14,192,229,205,101,123,93,162,32,141,98,248,4,227,191,200,252,75,209,197,172,54,2,0,147,
52,156,151,246,94,101,186,102,179,189,46,81,144,182,106,194,147,232,182,162,170,155,231,231,212,254,1,128,213,112,94,122,218,38,127,57,236,197,20,164,99,170,234,230,100,146,175,165,220,101,174,187,228,
224,35,0,48,41,195,57,233,69,126,56,189,32,191,90,58,192,97,184,40,246,136,134,179,69,31,204,250,190,33,159,233,187,246,206,210,33,0,0,146,100,24,128,101,56,195,116,237,245,93,59,171,247,203,86,144,14,
97,233,91,232,14,232,142,210,1,0,0,146,164,170,155,103,163,28,77,221,239,150,14,112,88,10,210,85,84,117,115,125,146,51,89,199,22,186,3,169,234,230,171,125,215,222,92,58,7,0,176,122,223,86,58,0,87,117,
123,233,0,135,165,32,93,70,85,55,15,36,105,179,172,73,116,219,242,222,210,1,0,128,117,171,234,230,142,172,123,87,207,28,108,250,174,61,83,58,196,97,41,72,111,51,44,213,126,123,44,215,94,81,85,55,175,244,
93,123,83,233,28,0,192,106,61,85,58,0,87,245,91,165,3,28,133,130,148,253,241,213,73,190,37,62,133,56,12,91,14,1,128,34,134,213,163,235,74,231,224,202,230,58,216,107,181,5,201,22,186,227,115,47,18,0,80,
200,191,42,29,128,171,58,91,58,192,81,173,174,32,89,45,218,170,91,75,7,0,0,214,165,170,155,79,198,251,184,57,248,238,210,1,142,106,21,247,32,85,117,115,99,146,47,199,182,176,49,124,161,239,218,187,74,
135,0,0,214,193,189,71,179,48,187,187,143,46,182,232,21,164,21,95,230,186,75,179,253,116,0,0,152,23,247,30,205,198,239,148,14,112,28,139,43,72,182,208,237,220,9,247,34,1,0,59,242,254,210,1,56,144,89,239,
46,90,76,65,170,234,230,149,36,239,142,79,21,74,112,47,18,0,48,170,97,103,16,51,208,119,237,11,165,51,28,199,172,11,82,85,55,15,38,249,233,88,45,42,174,170,155,215,250,174,189,190,116,14,0,96,177,62,88,
58,0,7,50,219,233,117,23,204,178,32,185,204,117,146,174,171,234,230,190,190,107,31,45,29,4,0,88,150,170,110,30,143,15,196,231,98,246,231,211,103,83,144,170,186,121,49,201,55,71,41,154,178,191,157,228,
154,210,33,0,128,197,185,167,116,0,14,228,124,223,181,207,150,14,113,92,179,104,226,85,221,60,149,228,116,148,163,169,59,89,213,205,221,165,67,0,0,203,49,156,51,103,30,254,243,210,1,182,97,22,5,201,61,
59,179,242,79,74,7,0,0,150,161,170,155,211,113,143,229,92,236,45,229,168,197,44,10,210,224,92,233,0,28,200,201,170,110,30,40,29,2,0,88,132,47,151,14,192,129,253,243,210,1,182,101,78,5,201,42,210,124,252,
213,210,1,0,128,121,171,234,230,209,204,235,189,234,170,45,105,199,215,108,190,232,250,174,125,38,201,166,116,14,14,228,228,48,84,3,0,224,168,126,160,116,0,14,236,165,210,1,182,105,54,5,105,240,103,75,
7,224,192,78,151,14,0,0,204,83,85,55,159,205,252,222,167,174,217,31,40,29,96,155,102,245,133,215,119,237,103,146,236,149,206,193,193,84,117,243,122,233,12,0,192,44,253,87,165,3,112,96,123,125,215,190,
80,58,196,54,205,170,32,13,190,88,58,0,7,118,170,170,27,147,103,0,128,3,27,182,233,187,218,101,62,126,183,116,128,109,155,93,65,234,187,246,206,210,25,56,148,175,149,14,0,0,204,195,48,9,215,54,253,25,
233,187,246,166,210,25,182,109,118,5,105,176,168,131,96,11,119,178,170,155,135,74,135,0,0,102,225,127,42,29,128,67,57,91,58,192,24,230,90,144,254,79,165,3,112,40,127,186,116,0,0,96,218,170,186,121,62,
182,214,205,205,237,165,3,140,97,150,5,105,184,165,215,176,134,249,56,97,236,55,0,112,21,183,150,14,192,161,108,250,174,61,83,58,196,24,102,89,144,6,191,83,58,0,135,114,122,216,87,12,0,240,22,62,72,157,
165,31,41,29,96,44,179,45,72,125,215,222,82,58,3,135,102,95,49,0,240,22,85,221,156,142,193,12,115,179,25,118,116,45,210,108,11,210,192,176,134,121,57,81,213,205,179,165,67,0,0,147,242,229,210,1,56,180,
95,44,29,96,76,179,46,72,125,215,222,92,58,3,135,246,237,165,3,0,0,211,80,213,205,147,153,249,251,209,53,234,187,246,227,165,51,140,105,9,95,144,231,74,7,224,80,12,108,0,0,82,213,205,245,73,62,84,58,7,
135,182,200,193,12,23,155,125,65,234,187,246,250,210,25,56,180,211,85,221,220,83,58,4,0,80,212,34,239,208,89,186,53,204,1,152,125,65,26,248,6,155,159,127,80,58,0,0,80,198,112,38,121,41,239,67,215,100,
83,58,192,46,44,229,11,243,63,46,29,128,67,59,89,213,205,87,75,135,0,0,138,120,127,233,0,28,201,159,45,29,96,23,78,236,237,45,227,190,213,170,110,54,113,251,242,28,125,186,239,218,7,75,135,0,0,118,99,
248,128,244,189,165,115,112,104,155,190,107,175,41,29,98,23,150,178,130,148,36,255,186,116,0,142,228,103,74,7,0,0,118,163,170,155,135,163,28,205,213,143,151,14,176,43,139,41,72,125,215,222,94,58,3,71,
83,213,205,243,165,51,0,0,59,113,127,233,0,28,201,166,239,218,71,74,135,216,149,197,20,164,193,226,199,14,46,212,173,85,221,156,42,29,2,0,24,79,85,55,111,148,206,192,145,173,102,245,40,89,88,65,90,195,
216,193,5,51,137,16,0,22,170,170,155,167,179,176,247,157,43,178,183,166,213,163,100,153,95,168,86,145,230,233,212,48,242,19,0,88,158,59,74,7,224,200,126,185,116,128,93,91,204,20,187,139,153,104,55,107,
63,223,119,237,3,165,67,0,0,219,49,108,173,91,226,135,242,171,208,119,237,234,222,83,47,245,139,245,159,150,14,192,145,125,162,116,0,0,96,59,170,186,121,52,203,125,191,185,6,171,60,2,177,200,47,216,190,
107,239,46,157,129,163,171,234,230,149,210,25,0,128,173,248,161,210,1,56,186,190,107,111,42,157,161,132,69,22,164,129,179,72,243,117,99,85,55,247,149,14,1,0,28,157,169,117,179,183,202,213,163,100,193,
5,201,68,187,217,251,149,210,1,0,128,163,25,118,131,44,246,125,230,26,172,117,245,40,89,254,23,238,185,210,1,56,58,159,60,1,192,252,12,231,142,110,44,157,131,99,249,66,233,0,37,45,189,32,125,83,233,0,
28,203,201,225,222,4,0,96,62,126,176,116,0,142,101,211,119,237,93,165,67,148,180,232,130,212,119,237,249,88,69,154,187,59,170,186,185,167,116,8,0,224,234,134,173,117,171,27,11,189,48,191,85,58,64,105,
139,46,72,73,210,119,237,245,165,51,112,108,191,86,58,0,0,112,101,195,133,239,182,214,205,92,223,181,119,150,206,80,218,226,11,210,224,185,210,1,56,150,19,85,221,124,181,116,8,0,224,210,170,186,121,48,
201,251,75,231,224,216,158,41,29,96,10,86,81,144,250,174,189,189,116,6,142,237,189,195,15,95,0,96,122,126,166,116,0,142,207,234,209,190,85,20,164,129,85,164,249,243,195,23,0,38,198,212,217,197,88,237,
189,71,111,119,98,111,111,175,116,134,157,169,234,102,61,255,207,46,215,166,239,218,107,74,135,0,0,146,97,218,236,29,165,115,112,124,125,215,26,174,49,88,211,10,82,162,25,47,193,73,159,84,1,64,121,195,
214,119,229,104,25,158,40,29,96,74,86,85,144,214,124,35,240,194,156,28,38,229,0,0,229,216,250,190,12,155,190,107,239,45,29,98,74,86,85,144,6,103,74,7,96,43,222,95,213,205,233,210,33,0,96,141,236,230,88,
148,31,47,29,96,106,86,87,144,250,174,189,37,137,179,72,203,240,229,210,1,0,96,109,134,203,96,87,247,30,114,161,54,125,215,62,82,58,196,212,172,245,139,251,23,74,7,96,43,78,86,117,243,90,233,16,0,176,
22,85,221,60,25,151,193,46,201,247,150,14,48,69,171,154,98,119,177,170,110,94,79,114,170,116,14,182,226,172,243,101,0,48,174,170,110,238,75,242,43,165,115,176,53,38,3,95,198,90,87,144,146,228,187,75,7,
96,107,110,172,234,230,249,210,33,0,96,225,148,163,101,249,169,210,1,166,106,181,43,72,137,85,164,5,186,161,239,90,91,238,0,96,203,134,161,12,107,254,96,125,105,206,245,93,123,125,233,16,83,181,246,47,
244,166,116,0,182,202,61,87,0,176,101,195,121,223,181,191,103,92,20,229,232,202,86,253,197,222,119,237,67,73,54,165,115,176,53,39,171,186,241,231,9,0,91,50,76,172,187,174,116,14,182,234,153,210,1,166,
110,213,5,105,96,139,221,178,156,24,182,78,2,0,199,80,213,205,103,99,98,221,210,108,250,174,189,179,116,136,169,91,125,65,234,187,118,19,151,199,46,205,169,170,110,94,44,29,2,0,230,106,184,140,253,99,
165,115,176,117,191,88,58,192,28,172,190,32,37,46,143,93,168,211,85,221,124,178,116,8,0,152,41,31,52,46,207,166,239,218,143,151,14,49,7,10,210,55,252,245,210,1,216,186,159,171,234,198,214,0,0,56,4,231,
121,23,235,103,75,7,152,11,5,105,208,119,237,3,73,206,149,206,193,214,189,82,58,0,0,204,197,48,206,251,68,233,28,108,221,166,239,218,7,75,135,152,11,5,233,173,126,176,116,0,182,207,39,97,0,112,117,195,
196,58,239,13,151,201,142,154,67,88,245,69,177,151,226,34,180,197,218,244,93,123,77,233,16,0,48,69,85,221,60,157,228,142,210,57,24,197,75,125,215,222,92,58,196,156,40,2,239,244,35,165,3,48,138,147,195,
69,119,0,192,69,170,186,121,42,202,209,82,237,41,71,135,167,32,189,77,223,181,143,38,121,161,116,14,70,113,93,85,55,95,45,29,2,0,166,162,170,155,231,147,124,79,233,28,140,230,199,74,7,152,35,91,236,46,
195,86,187,69,251,66,223,181,119,149,14,1,0,37,85,117,243,112,146,251,75,231,96,52,103,251,174,189,169,116,136,57,82,0,46,207,86,187,229,250,158,225,118,112,0,88,165,225,26,12,229,104,217,190,187,116,
128,185,82,144,46,99,216,106,103,236,247,114,125,172,170,155,135,74,135,0,128,66,94,46,29,128,81,157,239,187,246,217,210,33,230,202,22,187,171,168,234,198,255,64,203,246,147,125,215,42,74,0,172,194,176,
114,228,142,192,133,235,187,214,93,86,199,96,5,233,234,206,148,14,192,168,254,218,240,203,2,0,22,77,57,90,141,79,151,14,48,119,10,210,85,244,93,123,75,233,12,140,206,47,11,0,214,192,182,186,229,59,215,
119,237,131,165,67,204,157,130,116,48,159,47,29,128,113,217,74,9,192,146,85,117,243,74,18,219,174,22,174,239,218,235,75,103,88,2,5,233,0,250,174,189,59,201,217,210,57,24,87,85,55,123,182,219,1,176,52,
195,213,37,126,191,45,223,19,165,3,44,133,130,116,64,195,28,121,171,12,203,103,187,29,0,139,225,94,199,213,216,244,93,123,111,233,16,75,225,27,230,112,254,76,233,0,140,111,248,101,2,0,179,166,28,173,202,
207,150,14,176,36,198,124,31,82,85,55,175,37,185,174,116,14,70,183,233,187,246,154,210,33,0,224,40,188,95,89,149,243,125,215,94,91,58,196,146,248,84,225,240,62,80,58,0,59,113,210,74,18,0,115,52,12,100,
80,142,86,66,57,218,62,5,233,144,250,174,125,33,238,70,90,139,147,166,219,1,48,39,6,50,172,206,71,74,7,88,34,91,236,142,168,170,155,77,140,203,92,139,189,190,107,125,152,0,192,164,121,111,178,58,207,245,
93,123,123,233,16,75,228,77,223,209,125,103,233,0,236,204,137,225,151,14,0,76,210,176,114,164,28,173,199,158,114,52,30,5,233,136,250,174,125,38,182,218,173,201,9,103,146,0,152,162,97,32,131,247,116,235,
242,203,165,3,44,153,45,118,199,84,213,205,235,73,78,149,206,193,78,125,184,239,218,39,75,135,0,0,163,188,87,201,164,221,145,249,134,58,62,7,33,215,231,215,171,186,241,231,14,64,49,85,221,220,61,12,18,
242,94,110,101,148,163,241,249,166,58,166,190,107,207,39,121,169,116,14,118,238,21,37,9,128,18,170,186,185,53,201,175,151,206,65,17,159,43,29,96,13,108,177,219,18,227,160,87,235,206,225,60,26,0,140,110,
248,112,238,149,210,57,40,226,108,223,181,55,149,14,177,6,86,144,182,71,163,95,167,167,171,186,121,184,116,8,0,150,175,170,155,251,163,28,173,213,158,114,180,59,10,210,150,244,93,251,241,36,86,18,214,
233,254,170,110,158,45,29,2,128,229,170,234,230,171,73,124,32,183,94,255,73,233,0,107,98,139,221,150,153,106,183,106,47,244,93,123,91,233,16,0,44,203,48,198,251,186,210,57,40,230,92,223,181,215,151,14,
177,38,10,210,8,156,71,90,53,251,131,1,216,154,170,110,94,76,114,186,116,14,202,233,187,214,5,192,59,102,139,221,56,190,80,58,0,197,220,56,172,34,2,192,177,40,71,36,121,164,116,128,53,178,130,52,18,23,
183,173,158,75,220,0,56,50,239,35,72,114,166,239,218,91,74,135,88,35,5,105,68,182,218,173,222,94,146,119,245,93,187,41,29,4,128,121,168,234,230,246,36,95,42,157,131,226,108,217,47,200,39,19,227,250,249,
210,1,40,234,68,146,55,170,186,185,167,116,16,0,166,111,248,125,161,28,17,229,168,44,5,105,68,125,215,62,144,228,108,233,28,20,247,120,85,55,79,149,14,1,192,116,85,117,243,80,146,199,75,231,96,18,156,
101,47,204,22,187,29,176,143,152,129,49,224,0,188,131,97,12,92,228,124,223,181,215,150,14,177,118,222,180,239,134,217,245,36,201,173,67,89,6,128,36,73,85,55,155,40,71,236,219,83,142,166,65,65,218,129,
190,107,207,39,57,83,58,7,147,112,178,170,155,77,85,55,126,25,2,172,220,80,142,220,113,195,5,191,183,116,0,246,217,98,183,67,182,218,241,54,247,246,93,251,68,233,16,0,236,214,48,140,193,121,35,46,246,
76,223,181,119,150,14,193,62,5,105,199,140,254,230,109,30,233,187,246,163,165,67,0,176,27,85,221,60,157,228,142,210,57,152,20,119,39,78,140,213,140,221,251,112,233,0,76,202,253,85,221,188,82,58,4,0,227,
171,234,230,181,40,71,188,141,114,52,61,86,144,10,168,234,230,241,36,238,198,225,237,62,220,119,237,147,165,67,0,176,93,85,221,92,159,228,119,227,188,17,239,100,39,201,4,89,65,42,160,239,218,123,147,156,
43,157,131,201,249,245,170,110,30,40,29,2,128,237,169,234,230,225,36,175,70,57,226,157,94,82,142,166,201,10,82,65,206,35,113,25,95,232,187,246,174,210,33,0,56,30,247,27,113,5,206,29,77,152,21,164,178,
30,43,29,128,73,250,158,170,110,94,175,234,230,84,233,32,0,28,77,85,55,175,71,57,226,210,148,163,137,83,144,10,234,187,246,190,36,95,40,157,131,73,58,149,228,245,170,110,62,91,58,8,0,7,87,213,205,147,
195,14,17,31,114,113,57,223,82,58,0,87,102,139,221,4,12,83,204,110,44,157,131,201,114,55,2,192,12,248,125,206,1,60,215,119,237,237,165,67,112,101,10,210,68,184,77,155,171,56,215,119,237,245,165,67,0,112,
105,195,150,58,171,70,92,137,173,117,51,161,32,77,68,85,55,183,39,249,82,233,28,76,222,167,250,174,253,76,233,16,0,236,171,234,230,161,36,159,40,157,131,233,235,187,214,7,225,51,161,32,77,136,219,181,
57,160,243,125,215,94,91,58,4,192,218,85,117,243,70,156,231,230,0,148,163,121,81,144,38,198,254,101,14,193,197,178,0,5,84,117,115,119,146,95,47,157,131,217,248,72,223,181,143,150,14,193,193,41,72,19,228,
19,41,14,225,133,190,107,111,43,29,2,96,45,236,246,224,144,12,90,154,33,5,105,162,12,109,224,16,206,246,93,123,83,233,16,0,75,54,220,77,247,90,124,128,201,193,25,202,48,83,190,201,167,235,93,165,3,48,
27,55,86,117,179,169,234,198,118,59,128,17,12,63,95,95,143,247,77,28,130,114,52,95,86,144,38,172,170,155,135,147,220,95,58,7,179,178,73,114,99,223,181,175,149,14,2,48,119,86,141,56,134,107,250,174,221,
148,14,193,209,248,134,159,176,190,107,63,154,228,108,233,28,204,202,201,36,175,14,99,103,1,56,162,170,110,158,143,85,35,142,230,35,202,209,188,89,65,154,1,67,27,56,34,103,147,0,142,160,170,155,215,146,
92,87,58,7,179,244,249,190,107,239,46,29,130,227,81,144,102,66,73,226,24,252,176,6,56,128,225,172,209,135,74,231,96,182,124,48,185,16,10,210,140,152,108,199,49,108,146,124,107,223,181,47,148,14,2,48,53,
67,49,250,63,196,239,88,142,206,196,186,5,177,34,49,47,223,89,58,0,179,117,50,201,243,195,158,122,0,6,85,221,188,152,253,85,35,229,136,163,218,83,142,150,197,10,210,204,12,63,200,79,151,206,193,172,109,
146,252,190,190,107,159,45,29,4,160,148,170,110,238,75,242,119,162,24,113,60,123,125,215,90,112,88,24,127,160,51,211,119,237,45,73,206,149,206,193,172,157,76,242,165,161,108,3,172,78,85,55,175,36,249,
149,40,71,28,223,255,165,116,0,182,207,10,210,76,85,117,243,213,36,239,45,157,131,69,120,161,239,218,219,74,135,0,24,91,85,55,79,39,185,163,116,14,22,227,177,190,107,239,43,29,130,237,83,144,102,108,248,
4,236,198,210,57,88,132,189,36,63,214,119,237,35,165,131,0,108,91,85,55,143,39,185,167,116,14,22,197,196,186,5,83,144,102,206,100,59,182,236,165,190,107,111,46,29,2,96,91,170,186,121,61,201,169,210,57,
88,148,115,125,215,94,95,58,4,227,81,144,102,174,170,155,187,147,252,122,233,28,44,142,109,3,192,172,13,171,70,127,56,62,68,100,187,140,243,94,1,5,105,1,170,186,249,108,146,143,149,206,193,226,216,118,
7,204,78,85,55,143,38,249,129,24,68,197,246,41,71,43,161,32,45,68,85,55,159,76,242,115,165,115,176,72,155,36,127,207,138,18,48,101,85,221,60,152,228,47,196,138,17,227,48,206,123,69,20,164,5,177,146,196,
200,206,247,93,123,109,233,16,0,23,171,234,230,84,146,215,98,197,136,113,221,217,119,237,51,165,67,176,27,10,210,194,84,117,243,108,146,247,151,206,193,162,61,215,119,237,237,165,67,0,184,242,130,29,249,
92,223,181,31,47,29,130,221,81,144,22,200,196,30,118,96,147,228,31,245,93,123,111,233,32,192,250,56,103,196,14,125,161,239,218,187,74,135,96,183,20,164,133,114,71,18,59,178,151,228,95,91,81,2,118,193,
138,17,59,102,162,235,74,41,72,11,86,213,205,107,73,174,43,157,131,85,216,75,242,59,125,215,222,82,58,8,176,60,85,221,60,153,228,67,165,115,176,42,182,147,175,152,130,180,112,85,221,188,17,91,16,216,173,
159,239,187,246,129,210,33,128,249,171,234,230,225,36,63,26,147,233,216,45,23,193,174,156,130,180,2,85,221,248,67,102,215,54,73,126,164,239,218,71,75,7,1,230,167,170,155,7,146,252,213,248,128,143,221,
51,177,21,5,105,13,170,186,57,157,228,197,210,57,88,165,77,146,159,237,187,246,193,210,65,128,233,51,124,129,194,206,246,93,123,83,233,16,148,167,32,173,196,112,79,196,235,165,115,176,90,206,40,1,151,
85,213,205,139,73,78,151,206,193,170,41,71,188,73,65,90,145,170,110,78,38,121,163,116,14,86,239,140,162,4,36,138,17,147,177,233,187,246,154,210,33,152,14,5,105,101,170,186,185,61,201,151,74,231,96,245,
54,73,254,141,9,65,176,78,138,17,83,210,119,173,33,32,188,133,130,180,66,85,221,60,148,228,19,165,115,192,192,182,6,88,129,97,23,195,151,163,24,49,29,123,73,174,235,187,246,124,233,32,76,139,130,180,82,
85,221,60,152,228,103,74,231,128,139,108,146,252,86,223,181,119,150,14,2,108,79,85,55,79,39,249,142,24,188,192,180,216,86,199,101,41,72,43,86,213,205,253,73,30,46,157,3,222,198,64,7,88,0,219,232,152,48,
229,136,43,82,144,86,110,184,107,226,175,149,206,1,151,97,160,3,204,76,85,55,95,77,242,222,210,57,224,114,156,57,226,106,20,36,82,213,205,103,147,124,172,116,14,184,130,243,73,110,235,187,246,76,233,32,
192,59,85,117,115,119,146,95,77,114,99,233,44,112,21,247,246,93,251,68,233,16,76,155,130,68,146,164,170,155,39,147,124,168,116,14,184,138,77,146,175,36,185,171,239,218,23,74,135,129,53,27,134,46,252,118,
146,111,79,226,19,121,166,110,47,201,187,251,174,125,173,116,16,166,79,65,226,77,6,55,48,51,155,36,255,162,239,218,187,74,7,129,53,25,206,22,189,47,134,46,48,31,206,28,113,40,10,18,111,81,213,205,125,
73,126,165,116,14,56,164,115,73,190,213,22,60,24,143,179,69,204,149,51,71,28,150,130,196,59,88,73,98,198,54,73,254,94,223,181,247,149,14,2,75,48,220,155,247,167,98,181,136,249,114,230,136,67,83,144,184,
164,170,110,30,78,114,127,233,28,112,12,103,147,252,241,190,107,31,41,29,4,230,164,170,155,79,38,249,116,146,119,199,217,34,230,237,54,231,85,57,10,5,137,203,178,146,196,130,108,146,124,197,200,112,184,
180,225,50,215,15,36,57,85,58,11,108,129,51,71,28,139,130,196,21,85,117,115,123,146,47,149,206,1,91,116,62,201,159,239,187,246,51,165,131,64,73,195,246,185,63,25,165,136,101,81,142,56,54,5,137,171,170,
234,230,116,146,23,75,231,128,45,219,75,242,245,36,223,215,119,237,147,165,195,192,46,84,117,115,107,146,127,149,228,61,177,125,142,5,50,144,129,109,80,144,56,144,170,110,238,73,242,120,233,28,48,146,
189,36,95,75,242,159,59,204,203,210,12,31,114,253,171,36,223,28,165,136,229,218,235,187,214,48,17,182,66,65,226,192,148,36,86,228,124,146,51,73,62,99,43,30,115,51,108,141,254,181,36,183,37,121,87,148,
34,150,239,92,223,181,215,151,14,193,114,40,72,28,90,85,55,111,196,200,87,214,229,124,146,223,116,41,45,83,85,213,205,227,73,62,156,228,218,40,68,172,203,217,190,107,111,42,29,130,101,81,144,56,18,37,
137,21,219,36,249,114,223,181,183,149,14,194,186,85,117,243,100,146,63,24,171,68,172,151,149,35,70,161,32,113,100,74,18,100,147,228,213,36,63,99,43,30,99,187,232,44,209,251,226,103,47,40,71,140,70,65,
226,88,148,36,120,139,11,195,30,254,102,146,186,239,218,243,133,243,48,99,195,133,173,159,76,242,45,241,115,22,46,118,198,189,118,140,73,65,226,216,170,186,121,49,201,233,210,57,96,162,46,172,50,253,187,
190,107,239,44,29,134,105,170,234,230,225,36,63,152,253,51,68,182,204,193,229,61,210,119,237,71,75,135,96,217,20,36,182,162,170,155,167,146,124,79,233,28,48,3,155,236,15,125,120,217,39,160,235,53,172,
14,125,58,201,13,177,58,4,7,245,88,223,181,247,149,14,193,242,41,72,108,205,112,96,248,67,165,115,192,204,236,13,255,122,57,201,31,232,187,246,133,194,121,24,65,85,55,143,38,249,35,73,78,69,33,130,163,
248,249,190,107,31,40,29,130,117,80,144,216,42,219,237,96,43,46,148,166,47,39,249,116,223,181,15,21,206,195,1,85,117,115,107,146,62,251,219,229,172,14,193,118,220,217,119,237,51,165,67,176,30,10,18,91,
87,213,205,125,73,254,78,236,161,135,49,108,146,124,37,251,219,244,254,109,246,135,65,60,89,54,210,122,12,147,228,62,155,228,59,147,220,152,228,189,113,102,8,198,114,190,239,218,107,75,135,96,125,20,36,
70,99,194,29,236,212,102,248,235,249,36,95,79,242,219,46,182,61,186,97,37,232,55,146,188,39,251,131,19,174,137,159,103,176,75,155,190,107,175,41,29,130,117,82,144,24,149,146,4,147,112,161,60,189,145,228,
245,36,255,46,201,159,238,187,246,137,114,145,202,171,234,230,254,36,63,159,228,155,178,255,115,234,93,195,63,178,26,4,101,185,227,136,162,20,36,70,167,36,193,228,237,101,191,20,92,92,164,206,36,249,213,
236,111,39,251,95,147,124,185,239,218,205,165,255,235,211,80,213,205,169,36,31,72,242,253,73,238,79,242,193,236,111,129,187,240,41,244,197,197,71,9,130,105,58,219,119,237,77,165,67,176,110,10,18,59,97,
120,3,44,206,133,82,181,247,182,127,255,198,240,207,55,217,223,234,119,193,133,127,127,165,203,115,79,229,27,171,56,23,92,216,222,246,246,66,163,224,192,242,60,209,119,237,189,165,67,128,130,196,206,84,
117,243,96,146,159,41,157,3,0,152,156,123,215,190,237,151,233,80,144,216,169,170,110,238,78,242,235,165,115,0,0,147,96,24,3,147,163,32,177,115,85,221,220,158,228,75,165,115,0,0,69,25,227,205,36,41,72,
20,99,120,3,0,172,150,97,12,76,150,55,167,20,51,44,169,187,25,27,0,214,229,243,202,17,83,166,32,81,84,223,181,119,38,121,172,116,14,0,96,39,62,220,119,237,221,165,67,192,149,216,98,199,36,12,195,27,254,
151,24,221,11,0,75,100,24,3,179,161,32,49,41,206,37,1,192,226,40,71,204,138,55,162,76,202,240,3,244,76,233,28,0,192,86,60,167,28,49,55,10,18,147,211,119,237,45,113,46,9,0,230,238,206,190,107,111,47,29,
2,14,203,22,59,38,171,170,155,147,73,190,30,231,146,0,96,78,108,169,99,214,20,36,38,207,185,36,0,152,13,229,136,217,243,166,147,201,27,126,208,158,45,157,3,0,184,162,51,202,17,75,160,32,49,11,195,133,
114,31,45,157,3,0,120,135,77,146,107,135,51,196,48,123,182,216,49,59,85,221,188,150,228,186,210,57,0,128,156,239,187,246,218,210,33,96,155,172,32,49,59,125,215,94,31,163,192,1,160,180,51,202,17,75,164,
32,49,75,195,50,254,11,165,115,0,192,74,125,218,150,58,150,202,22,59,102,175,170,155,87,146,220,88,58,7,0,172,192,185,97,39,7,44,150,21,36,102,111,24,224,240,133,210,57,0,96,225,62,175,28,177,6,10,18,
139,208,119,237,93,73,62,93,58,7,0,44,208,94,146,107,250,174,189,187,116,16,216,5,91,236,88,28,23,203,2,192,214,216,82,199,234,120,19,201,226,12,151,212,125,190,116,14,0,152,185,47,40,71,172,145,21,36,
22,205,106,18,0,28,218,217,225,124,47,172,146,55,142,44,218,176,154,100,128,3,0,28,204,23,148,35,214,78,65,98,241,134,1,14,31,40,157,3,0,38,238,35,195,239,76,88,53,91,236,88,149,170,110,94,79,114,170,
116,14,0,152,16,131,24,224,34,86,144,88,149,190,107,175,77,242,209,36,155,210,89,0,160,176,77,146,15,40,71,240,86,86,144,88,173,170,110,94,76,114,186,116,14,0,40,224,165,190,107,111,46,29,2,166,200,10,
18,171,213,119,237,45,73,126,190,116,14,0,216,177,159,84,142,224,242,172,32,65,156,77,2,96,21,156,53,130,3,176,130,4,121,243,108,210,167,226,108,18,0,203,179,151,228,211,202,17,28,140,21,36,120,27,103,
147,0,88,144,103,250,174,189,179,116,8,152,19,5,9,46,161,170,155,219,147,252,111,177,202,10,192,60,109,146,92,223,119,237,249,210,65,96,110,20,36,184,130,170,110,158,79,114,107,233,28,0,112,8,103,251,
174,189,169,116,8,152,43,159,142,195,21,244,93,123,91,246,39,221,57,155,4,192,212,109,178,127,214,72,57,130,99,176,130,4,7,84,213,205,103,147,252,68,124,176,0,192,180,236,37,249,139,125,215,62,88,58,8,
44,129,130,4,135,100,136,3,0,19,98,59,29,108,153,79,194,225,144,134,11,102,63,146,253,79,236,0,160,132,189,36,247,42,71,176,125,86,144,224,24,172,38,1,80,128,85,35,24,145,130,4,91,80,213,205,43,73,110,
44,157,3,128,69,59,155,228,246,190,107,207,148,14,2,75,166,32,193,150,84,117,115,58,201,151,99,235,42,0,219,117,62,201,251,251,174,125,161,116,16,88,3,5,9,182,172,170,155,199,147,220,83,58,7,0,139,240,
92,223,181,183,151,14,1,107,162,32,193,72,170,186,121,45,201,117,165,115,0,48,75,231,250,174,189,190,116,8,88,35,91,129,96,36,195,47,182,143,196,37,179,0,28,220,38,251,211,233,148,35,40,196,10,18,236,
64,85,55,183,39,249,223,226,67,9,0,46,109,147,228,167,250,174,125,168,116,16,88,59,5,9,118,168,170,155,135,147,252,104,146,19,133,163,0,48,13,155,36,191,216,119,237,199,75,7,1,246,41,72,80,64,85,55,207,
38,121,127,233,28,0,20,229,62,35,152,32,219,125,160,128,97,34,209,45,113,62,9,96,141,206,39,185,69,57,130,105,178,130,4,133,85,117,115,99,146,127,151,228,189,165,179,0,48,170,115,73,190,185,239,218,179,
165,131,0,151,167,32,193,132,84,117,243,122,146,83,165,115,0,176,85,231,147,220,214,119,237,153,210,65,128,171,83,144,96,98,170,186,185,59,201,63,137,45,176,0,115,183,73,242,253,125,215,62,81,58,8,112,
112,10,18,76,148,137,119,0,179,246,66,223,181,183,149,14,1,28,158,130,4,19,87,213,205,83,73,190,43,86,148,0,166,110,147,228,55,250,174,189,187,116,16,224,232,20,36,152,137,170,110,238,73,242,247,227,140,
18,192,212,108,146,252,141,190,107,31,40,29,4,56,62,5,9,102,166,170,155,207,38,249,137,88,81,2,40,109,147,228,103,251,174,125,176,116,16,96,123,20,36,152,169,161,40,253,87,113,70,9,96,215,246,146,252,
243,190,107,239,42,29,4,216,62,5,9,102,174,170,155,231,147,252,158,40,74,0,99,219,75,242,59,125,215,222,82,58,8,48,30,5,9,22,98,24,15,254,171,73,110,44,157,5,96,97,54,73,126,164,239,218,71,75,7,1,198,
167,32,193,2,85,117,243,90,146,235,74,231,0,152,185,179,73,190,165,239,218,179,165,131,0,187,163,32,193,130,85,117,243,74,172,40,1,28,214,217,190,107,111,42,29,2,40,67,65,130,21,80,148,0,14,228,92,146,
155,251,174,125,173,116,16,160,28,5,9,86,164,170,155,39,147,124,48,70,132,3,92,176,151,228,139,125,215,222,89,58,8,48,13,10,18,172,80,85,55,183,38,121,46,46,157,5,214,235,92,146,123,250,174,125,178,116,
16,96,90,20,36,88,185,170,110,94,76,114,186,116,14,128,29,113,190,8,184,34,5,9,72,226,156,18,176,120,47,245,93,123,115,233,16,192,244,41,72,192,91,84,117,243,116,146,239,136,115,74,192,252,237,37,249,
247,125,215,222,86,58,8,48,31,10,18,112,89,85,221,60,149,228,187,147,156,40,157,5,224,128,54,73,254,69,223,181,119,149,14,2,204,147,130,4,92,85,85,55,183,39,249,205,184,124,22,152,174,243,73,154,190,107,
31,42,29,4,152,55,5,9,56,148,170,110,190,154,228,155,98,11,30,48,13,206,22,1,91,165,32,1,71,82,213,205,163,73,126,32,138,18,176,123,155,36,95,233,187,246,150,210,65,128,229,81,144,128,99,171,234,230,241,
36,223,27,247,42,1,227,217,36,249,141,190,107,239,46,29,4,88,54,5,9,216,42,83,240,128,45,50,112,1,216,57,5,9,24,69,85,55,183,38,249,98,220,173,4,28,222,249,36,223,215,119,237,147,165,131,0,235,163,32,
1,163,171,234,230,249,36,223,18,171,74,192,229,109,146,252,86,223,181,119,150,14,2,172,155,130,4,236,84,85,55,207,38,249,182,40,75,192,48,108,33,201,183,245,93,251,90,233,48,0,137,130,4,20,84,213,205,
125,73,126,41,182,225,193,154,156,79,242,223,247,93,251,96,233,32,0,151,162,32,1,147,80,213,205,131,73,254,92,92,70,11,75,179,151,228,245,36,127,73,41,2,230,64,65,2,38,199,216,112,88,132,243,73,126,181,
239,218,251,74,7,1,56,12,5,9,152,180,170,110,158,74,242,157,81,150,96,14,206,39,57,211,119,237,109,165,131,0,28,149,130,4,204,198,48,58,252,177,40,76,48,21,231,147,252,207,73,254,120,223,181,103,75,135,
1,216,6,5,9,152,173,97,200,195,255,51,201,55,197,84,60,216,133,77,146,151,147,252,129,190,107,95,40,29,6,96,12,10,18,176,8,85,221,156,204,254,184,96,101,9,182,107,147,228,43,125,215,222,82,58,8,192,46,
40,72,192,34,85,117,243,100,146,63,152,228,93,73,78,148,77,3,179,114,97,234,220,87,156,37,2,214,72,65,2,22,175,170,155,211,73,254,78,146,59,98,133,9,222,110,147,228,213,36,191,157,228,143,246,93,123,166,
112,30,128,162,20,36,96,149,170,186,185,61,201,175,37,249,182,40,76,172,199,222,240,175,175,36,249,67,125,215,62,83,56,15,192,228,40,72,0,73,170,186,185,59,201,223,141,21,38,150,103,147,253,66,244,109,
125,215,190,86,58,12,192,212,41,72,0,151,80,213,205,29,73,254,113,146,247,69,97,98,62,46,172,16,189,156,228,79,246,93,251,72,225,60,0,179,163,32,1,28,64,85,55,159,76,242,211,217,31,250,112,67,148,38,166,
97,147,253,187,136,94,78,114,151,209,219,0,199,167,32,1,28,67,85,55,247,36,249,217,36,31,78,114,109,76,204,99,28,23,6,41,252,203,36,127,161,239,218,71,11,231,1,88,44,5,9,96,203,170,186,57,149,228,239,
39,249,222,36,215,100,191,52,41,78,28,196,133,45,114,175,38,249,185,190,107,31,44,27,7,96,125,20,36,128,29,169,234,230,225,36,63,152,253,45,122,137,109,122,107,183,73,242,70,246,239,28,250,229,190,107,
63,94,56,15,0,81,144,0,138,170,234,230,193,36,255,77,246,207,54,93,155,253,21,39,197,105,57,46,172,8,93,40,66,95,203,254,89,33,119,13,1,76,148,130,4,48,65,85,221,156,76,242,199,146,252,88,146,187,146,
220,154,253,149,167,27,98,203,222,212,92,56,31,116,38,201,115,73,126,51,201,35,206,9,1,204,147,130,4,48,99,85,221,92,159,228,79,36,249,63,39,249,143,242,141,237,123,39,222,246,87,14,103,239,109,127,125,
57,201,175,38,249,43,125,215,62,81,38,18,0,187,160,32,1,172,192,112,17,238,95,78,242,193,236,111,231,59,153,253,237,124,23,172,97,85,234,194,118,183,100,127,203,91,178,191,250,243,114,146,127,152,228,
191,237,187,246,217,2,185,0,152,16,5,9,128,183,184,104,123,223,255,152,228,230,236,23,170,11,46,85,174,222,238,196,101,254,253,65,93,238,23,211,197,127,255,226,130,115,50,251,231,123,46,120,53,201,151,
146,124,202,106,15,0,135,165,32,1,0,0,12,76,74,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,
32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,
24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,
0,0,6,10,18,0,0,192,224,255,15,141,103,45,253,203,30,20,123,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* EditorComponent::anticlockwise_arrow_png = (const char*) resource_PluginEditor_anticlockwise_arrow_png;
const int EditorComponent::anticlockwise_arrow_pngSize = 22421;

// JUCER_RESOURCE: clockwise_arrow_png, 22647, "../GUI/clockwise_arrow.png"
static const unsigned char resource_PluginEditor_clockwise_arrow_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,3,72,0,0,3,118,8,6,0,0,0,195,151,169,6,0,0,0,9,112,72,89,115,0,0,11,19,0,0,
11,19,1,0,154,156,24,0,0,88,41,73,68,65,84,120,156,237,253,125,208,102,231,93,31,120,126,187,91,173,23,90,198,30,163,94,144,38,32,153,128,4,1,107,171,134,165,157,20,131,196,178,1,203,144,154,65,154,84,
205,76,172,169,236,31,25,228,205,57,167,42,179,118,102,83,179,197,32,135,154,74,54,137,167,166,234,156,195,32,39,91,169,173,216,80,217,2,139,204,46,65,134,169,128,21,102,93,72,227,45,86,134,128,132,39,
200,192,72,241,168,241,72,182,218,82,171,173,231,217,63,158,211,184,37,245,203,243,114,159,251,58,47,159,79,149,203,66,54,231,254,90,253,188,220,223,251,186,174,223,117,108,119,119,55,0,0,0,36,199,75,
7,0,0,0,152,10,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,
0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,
128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,92,87,58,0,0,243,82,
213,205,61,73,126,42,201,159,75,114,211,37,255,209,245,73,118,242,198,15,223,78,92,242,215,199,222,244,168,55,255,223,7,177,251,166,127,127,179,215,135,127,223,73,242,213,75,254,254,43,73,190,152,228,
111,37,249,229,190,107,95,61,66,6,0,22,232,216,238,238,149,126,183,0,176,6,85,221,220,153,228,239,38,249,254,36,111,203,94,193,121,115,177,57,74,153,153,131,221,188,177,108,189,158,175,149,171,127,157,
164,238,187,246,241,18,193,0,216,46,5,9,96,161,170,186,57,153,228,255,144,228,111,36,249,243,217,43,63,201,215,202,206,210,75,207,88,222,188,122,117,33,201,83,73,254,78,146,255,119,223,181,23,138,164,
2,96,35,20,36,128,25,170,234,230,76,146,31,201,94,241,121,87,146,119,14,255,90,195,106,207,156,236,100,111,91,223,43,73,158,207,94,145,250,133,36,255,172,239,218,157,146,193,0,184,60,5,9,96,162,170,186,
185,35,201,255,39,201,215,103,239,124,207,137,40,64,75,179,147,189,237,124,175,37,249,82,146,31,232,187,246,153,178,145,0,214,77,65,2,152,128,170,110,62,150,228,199,242,181,34,100,202,232,186,93,90,156,
62,222,119,237,67,133,243,0,172,134,130,4,176,101,85,221,60,156,228,67,217,155,0,103,69,136,253,186,248,11,251,181,36,255,188,239,218,7,74,134,1,88,42,5,9,96,36,85,221,220,151,228,111,231,107,227,176,
173,10,49,134,221,36,95,73,242,153,36,127,179,239,218,39,10,231,1,152,53,5,9,224,136,134,129,9,191,148,189,41,113,39,163,8,49,13,59,217,155,176,247,229,36,63,170,56,1,236,143,130,4,112,64,85,221,220,159,
228,31,103,175,16,217,34,199,92,236,102,239,94,167,151,146,220,221,119,237,243,133,243,0,76,146,130,4,112,13,85,221,156,74,242,63,231,107,151,168,194,18,92,188,28,247,139,73,238,232,187,246,92,225,60,
0,147,160,32,1,188,73,85,55,119,38,249,31,178,119,175,144,66,196,154,236,36,249,92,146,239,235,187,246,108,233,48,0,37,40,72,192,170,13,171,67,159,78,242,173,49,72,1,222,108,39,123,43,76,255,42,201,123,
251,174,125,181,112,30,128,209,41,72,192,234,84,117,243,92,246,86,135,174,143,243,67,112,16,187,217,27,51,254,155,125,215,222,91,58,12,192,24,20,36,96,21,170,186,121,33,182,204,193,166,93,72,242,82,223,
181,167,75,7,1,216,20,5,9,88,164,97,244,246,175,196,96,5,216,150,139,219,241,76,200,3,102,77,65,2,22,97,56,75,244,11,73,126,48,123,119,17,1,101,93,200,222,249,190,31,49,33,15,152,19,5,9,152,173,225,44,
209,45,81,136,96,234,46,158,93,122,170,239,218,51,165,195,0,92,141,130,4,204,74,85,55,143,101,111,149,232,186,24,176,0,115,117,33,201,191,232,187,246,190,210,65,0,222,76,65,2,38,175,170,155,15,38,249,
59,177,82,4,75,116,62,201,127,220,119,237,163,165,131,0,36,10,18,48,81,67,41,250,175,146,220,80,58,11,176,53,231,146,124,176,239,218,71,74,7,1,214,75,65,2,38,161,170,155,91,147,124,38,201,55,198,212,57,
96,111,42,222,31,245,93,123,71,233,32,192,186,40,72,64,81,85,221,60,149,228,59,98,251,28,112,101,59,73,126,167,239,218,187,75,7,1,150,79,65,2,182,174,170,155,59,147,252,118,148,34,224,224,206,37,185,163,
239,218,179,165,131,0,203,164,32,1,91,83,213,205,211,73,190,61,166,207,1,71,183,147,228,115,125,215,222,85,58,8,176,44,10,18,48,170,97,44,247,15,197,185,34,96,60,23,146,124,186,239,218,123,75,7,1,230,
79,65,2,54,174,170,155,23,146,188,51,74,17,176,125,59,73,126,67,89,2,14,75,65,2,54,102,40,70,183,148,206,1,144,100,55,201,87,146,124,125,223,181,59,165,195,0,243,161,32,1,71,82,213,205,29,73,158,137,129,
11,192,116,157,79,114,79,223,181,79,148,14,2,76,159,130,4,28,202,48,158,251,187,98,27,29,48,31,187,73,126,219,184,112,224,106,20,36,96,223,170,186,121,54,201,55,71,41,2,230,207,69,180,192,101,41,72,192,
53,57,91,4,44,220,185,190,107,111,46,29,2,152,6,5,9,184,162,170,110,94,78,114,170,116,14,128,45,57,219,119,237,233,210,33,128,178,20,36,224,13,134,161,11,191,29,197,8,88,175,11,73,110,239,187,246,249,
210,65,128,237,83,144,128,36,127,58,116,225,187,147,28,43,157,5,96,34,118,146,124,166,239,218,51,165,131,0,219,163,32,193,202,85,117,243,106,146,27,74,231,0,152,184,243,125,215,222,88,58,4,48,62,5,9,86,
202,249,34,128,67,49,208,1,22,78,65,130,21,169,234,230,120,146,47,69,49,2,56,170,243,73,190,174,239,218,157,210,65,128,205,82,144,96,5,170,186,185,39,201,175,197,253,69,0,155,118,62,201,61,125,215,62,
81,58,8,176,25,10,18,44,216,112,177,235,183,196,224,5,128,177,237,38,249,55,125,215,222,86,58,8,112,52,10,18,44,80,85,55,143,37,249,225,40,70,0,219,182,155,228,87,250,174,189,175,116,16,224,112,20,36,
88,144,170,110,30,74,242,211,177,149,14,160,52,69,9,102,74,65,130,5,168,234,230,225,36,63,17,197,8,96,106,118,146,252,104,223,181,143,149,14,2,236,143,130,4,51,86,213,205,19,73,190,39,138,17,192,212,237,
36,249,157,190,107,239,46,29,4,184,58,5,9,102,168,170,155,231,146,220,90,58,7,0,135,242,100,223,181,103,74,135,0,46,79,65,130,25,169,234,230,206,36,191,27,43,70,0,115,183,147,228,47,247,93,251,104,233,
32,192,27,41,72,48,3,85,221,220,152,228,197,36,55,20,142,2,192,102,237,36,57,213,119,237,171,165,131,0,123,20,36,152,184,170,110,94,78,114,170,116,14,0,70,117,62,201,159,233,187,246,108,233,32,176,118,
10,18,76,208,176,98,244,165,36,39,75,103,1,96,171,118,146,252,239,251,174,125,188,116,16,88,43,5,9,38,198,138,17,0,73,206,246,93,123,186,116,8,88,35,5,9,38,162,170,155,71,146,252,167,73,142,149,206,2,
192,100,152,120,7,91,166,32,65,97,85,221,220,159,228,231,99,50,29,0,151,183,155,228,7,108,187,131,237,80,144,160,144,170,110,238,73,242,107,81,140,0,216,159,157,36,63,213,119,237,195,165,131,192,146,41,
72,80,128,115,70,0,28,193,249,190,107,111,44,29,2,150,74,65,130,45,170,234,230,217,36,183,151,206,1,192,34,60,222,119,237,189,165,67,192,210,40,72,176,5,198,118,3,48,18,99,193,97,195,20,36,24,209,80,140,
206,198,118,58,0,198,117,174,239,218,155,75,135,128,37,80,144,96,36,85,221,188,144,228,150,210,57,0,88,21,69,9,142,72,65,130,13,171,234,230,150,36,95,136,233,116,0,148,243,241,190,107,31,44,29,2,230,72,
65,130,13,170,234,230,197,36,111,47,157,3,0,98,53,9,14,69,65,130,13,168,234,230,131,73,254,126,146,99,165,179,0,192,37,118,147,252,74,223,181,247,149,14,2,115,161,32,193,17,84,117,243,112,146,159,136,
237,116,0,76,155,162,4,251,164,32,193,33,185,236,21,128,25,58,219,119,237,233,210,33,96,202,20,36,56,160,170,110,142,39,185,16,171,70,0,204,211,78,146,239,236,187,246,153,210,65,96,138,188,193,131,3,168,
234,230,169,36,175,199,247,14,0,243,117,60,201,211,195,78,8,224,77,172,32,193,62,56,107,4,192,66,237,38,249,135,125,215,62,84,58,8,76,133,130,4,215,80,213,205,171,73,110,40,157,3,0,70,180,211,119,237,
137,210,33,96,10,20,36,184,130,170,110,238,73,242,235,49,186,27,128,245,120,188,239,218,123,75,135,128,146,20,36,184,12,23,190,2,176,98,38,221,177,106,10,18,92,162,170,155,83,73,190,20,103,141,0,88,183,
221,36,127,187,239,218,135,75,7,129,109,83,144,96,224,94,35,0,120,11,103,147,88,29,5,137,213,171,234,230,142,36,255,58,206,26,1,192,149,188,175,239,218,199,74,135,128,109,80,144,88,181,170,110,62,149,
228,158,210,57,0,96,6,158,239,187,246,182,210,33,96,108,10,18,171,101,75,29,0,28,216,78,146,83,125,215,190,90,58,8,140,69,65,98,117,170,186,121,42,201,187,75,231,0,128,25,251,124,223,181,119,148,14,1,
99,80,144,88,21,171,70,0,176,49,6,56,176,72,10,18,171,80,213,205,241,36,23,98,124,55,0,108,218,39,251,174,189,175,116,8,216,20,111,22,89,188,97,16,195,235,241,245,14,0,99,120,111,85,55,207,150,14,1,155,
98,5,137,69,179,165,14,0,182,230,66,246,6,56,92,40,29,4,142,66,65,98,145,170,186,249,96,146,127,80,58,7,0,172,208,217,190,107,79,151,14,1,135,165,32,177,56,85,221,188,144,228,150,210,57,0,96,197,206,245,
93,123,115,233,16,112,24,10,18,139,82,213,205,107,73,78,150,206,1,0,228,66,223,181,215,151,14,1,7,165,32,177,8,166,212,1,192,100,125,188,239,218,7,75,135,128,253,82,144,152,61,91,234,0,96,242,108,185,
99,54,20,36,102,173,170,27,227,187,1,96,30,92,44,203,44,40,72,204,150,114,4,0,179,244,158,190,107,159,40,29,2,174,68,65,98,118,170,186,185,51,201,211,165,115,0,0,135,246,76,223,181,119,149,14,1,151,163,
32,49,43,195,77,221,183,151,206,1,0,28,153,115,73,76,146,130,196,108,216,82,7,0,139,179,155,228,186,190,107,119,74,7,129,139,188,217,100,22,148,35,0,88,164,99,73,94,175,234,230,225,210,65,224,34,43,72,
76,154,243,70,0,176,26,159,237,187,246,238,210,33,64,65,98,178,170,186,121,34,201,247,150,206,1,0,108,205,217,190,107,79,151,14,193,186,41,72,76,82,85,55,47,39,57,85,58,7,0,176,117,238,75,162,40,5,137,
201,113,222,8,0,72,242,64,223,181,143,150,14,193,250,120,19,202,164,40,71,0,192,224,19,85,221,60,84,58,4,211,84,213,205,61,195,251,198,141,179,130,196,36,24,198,0,0,92,193,39,251,174,189,175,116,8,166,
163,170,155,215,146,156,76,146,190,107,143,109,250,249,10,18,197,85,117,243,137,36,247,151,206,1,108,220,155,127,193,236,38,185,144,228,171,151,252,189,175,14,127,239,90,78,38,185,238,77,127,239,186,225,
239,39,123,163,130,47,181,241,95,152,64,81,47,245,93,251,142,210,33,40,171,170,155,23,147,188,253,210,191,167,32,177,56,151,251,66,7,38,101,247,77,127,253,122,146,151,146,124,38,201,163,73,62,153,228,
15,231,112,201,99,85,55,183,36,249,115,73,222,159,228,189,73,110,73,114,211,37,255,149,99,81,172,96,202,12,111,88,169,170,110,94,77,114,195,229,254,51,5,137,69,185,218,23,59,176,53,187,249,90,241,73,246,
202,207,127,210,119,237,99,229,34,149,55,148,169,127,158,189,66,117,125,146,139,111,202,148,40,40,75,73,90,145,170,110,62,149,228,158,171,253,119,20,36,22,195,48,6,216,170,75,75,208,107,73,254,231,36,
223,215,119,237,217,162,169,102,172,170,155,15,38,249,91,73,222,150,189,159,101,39,226,103,26,108,139,146,180,112,85,221,220,154,228,143,179,143,159,171,10,18,139,112,233,193,58,96,163,118,146,188,50,
252,235,92,146,223,232,187,246,193,178,145,214,165,170,155,51,73,186,36,239,202,222,246,189,139,171,79,202,19,108,222,7,250,174,125,164,116,8,54,235,160,119,97,42,72,204,218,240,198,225,55,75,231,128,
5,184,88,132,126,49,201,223,176,18,52,31,195,207,193,127,146,228,246,236,149,39,219,245,224,104,12,111,88,136,195,126,128,174,32,49,91,85,221,124,44,123,7,163,129,131,217,201,222,148,183,223,236,187,246,
222,210,97,216,188,170,110,238,72,242,100,246,6,214,88,109,130,131,251,108,223,181,119,151,14,193,225,84,117,243,66,246,134,230,28,138,130,196,44,85,117,243,88,246,38,70,1,215,118,113,117,232,63,113,131,
252,122,85,117,243,84,146,59,179,247,105,170,194,4,215,246,249,190,107,239,40,29,130,253,27,46,1,254,233,28,241,103,156,130,196,236,184,227,8,174,106,55,123,247,0,125,37,201,63,237,187,214,141,241,92,
214,80,152,190,53,123,231,154,20,38,184,60,219,237,102,96,152,18,250,199,217,204,36,227,221,190,107,55,254,51,81,65,98,52,71,93,50,133,133,185,56,73,238,143,146,252,103,86,135,56,138,170,110,78,38,249,
191,38,169,146,188,51,74,19,92,100,194,221,68,109,184,24,93,164,32,49,31,238,56,130,63,45,68,95,76,114,71,223,181,231,10,231,97,225,170,186,185,63,123,3,32,172,50,177,118,74,210,196,28,116,50,221,1,40,
72,204,131,114,196,202,93,72,242,123,14,12,83,90,85,55,207,38,249,230,40,75,172,147,146,52,1,195,246,224,119,143,248,18,10,18,211,87,213,205,139,217,155,196,4,107,177,155,189,203,87,127,222,157,67,76,
213,112,27,253,123,226,195,43,214,101,148,55,207,92,219,65,46,122,61,162,81,138,176,130,196,198,88,57,98,69,206,39,249,124,146,239,115,7,17,115,83,213,205,7,147,124,48,201,55,198,234,18,235,240,190,190,
107,31,43,29,98,13,134,107,11,126,59,227,108,167,187,28,5,137,233,170,234,102,39,46,60,100,185,118,147,252,73,146,239,84,136,88,154,170,110,238,73,242,207,179,189,55,52,80,194,189,125,215,62,94,58,196,
82,13,131,99,206,229,16,23,189,30,145,130,196,52,85,117,227,139,136,37,218,77,242,165,36,223,166,20,177,22,195,182,152,63,136,251,151,88,166,119,245,93,251,108,233,16,75,83,213,205,115,73,110,45,244,242,
10,18,211,83,213,205,235,241,75,148,101,57,159,228,239,246,93,251,112,233,32,80,210,176,21,239,239,100,251,159,8,195,152,222,211,119,237,19,165,67,44,193,240,51,226,239,167,236,14,34,5,137,105,81,142,
88,144,157,36,191,99,242,28,92,222,112,175,157,251,150,88,138,15,251,16,236,240,170,186,185,49,123,59,44,166,240,225,137,130,196,52,12,23,125,189,80,58,7,28,193,197,237,115,255,151,190,107,31,41,29,6,
230,100,248,212,248,191,138,161,60,204,219,227,125,215,222,91,58,196,156,12,231,21,255,251,76,163,24,93,164,32,81,222,240,205,241,169,210,57,224,144,118,146,252,148,79,14,97,51,170,186,249,68,146,191,
148,105,189,97,130,253,250,104,223,181,15,149,14,49,7,19,158,84,172,32,81,158,129,12,204,212,217,190,107,79,151,14,1,75,85,213,205,169,236,77,122,156,226,27,40,184,154,15,216,73,112,101,85,221,60,150,
228,189,165,115,92,133,130,68,89,70,121,51,51,59,73,190,168,24,193,118,85,117,243,116,146,111,139,243,74,204,135,51,73,111,82,213,205,125,73,126,41,211,255,62,86,144,40,99,56,140,247,149,40,71,76,223,
110,146,63,236,187,246,142,210,65,96,237,170,186,57,147,189,55,88,134,59,48,7,79,246,93,123,166,116,136,210,134,81,255,159,207,124,182,205,42,72,108,159,51,71,204,196,185,36,63,104,116,43,76,83,85,55,
119,36,249,159,162,40,49,109,143,246,93,251,64,233,16,37,12,3,184,190,144,249,125,143,42,72,108,159,51,71,76,216,110,146,47,245,93,251,142,210,65,128,253,171,234,230,197,36,111,47,157,3,174,224,67,125,
215,126,164,116,136,109,170,234,230,229,36,167,74,231,56,36,5,137,237,114,207,17,19,246,124,223,181,183,149,14,1,28,222,112,183,210,45,165,115,192,101,60,208,119,237,163,165,67,140,173,170,155,135,146,
252,116,230,253,94,239,66,223,181,215,111,250,161,10,18,151,165,28,49,65,59,73,62,215,119,237,93,165,131,0,155,51,20,165,111,136,115,174,76,203,98,71,128,79,244,62,163,195,82,144,24,95,85,55,39,147,156,
143,95,84,76,199,243,73,254,76,223,181,59,165,131,0,227,169,234,230,120,246,46,112,158,235,86,31,150,231,227,125,215,62,88,58,196,166,204,104,50,221,65,40,72,140,207,153,35,38,196,54,58,88,169,170,110,
158,75,242,77,241,97,29,229,189,171,239,218,103,75,135,56,170,9,95,244,122,84,10,18,227,178,173,142,137,112,169,43,144,196,57,37,38,227,166,190,107,95,45,29,226,48,170,186,121,54,201,237,165,115,140,72,
65,98,60,51,159,96,194,50,40,70,192,101,153,124,199,4,92,223,119,237,133,210,33,246,107,56,103,244,107,89,254,7,223,10,18,227,80,142,40,104,39,201,175,246,93,123,95,233,32,192,244,85,117,243,88,146,247,
150,206,193,106,157,238,187,246,108,233,16,87,83,213,205,195,73,254,139,44,99,0,195,126,156,239,187,246,198,77,63,244,186,77,63,144,121,169,234,230,181,172,231,155,136,233,216,73,242,215,251,174,125,164,
116,16,96,62,46,126,152,82,213,205,253,73,126,62,203,255,116,156,105,121,33,19,61,23,55,12,217,122,53,190,39,54,194,10,210,138,45,248,192,30,211,182,218,155,202,129,205,170,234,230,99,73,254,74,38,250,
166,149,69,26,101,75,215,81,172,252,172,222,40,43,72,10,210,74,173,224,208,30,211,243,100,223,181,103,74,135,0,150,103,229,111,16,217,190,81,222,148,31,84,85,55,159,72,242,99,89,247,7,4,10,18,155,81,213,
205,167,146,220,83,58,7,171,97,248,2,176,21,138,18,91,116,174,239,218,155,75,188,240,176,197,244,159,198,17,137,68,65,98,19,170,186,121,42,201,187,75,231,96,241,118,146,252,70,223,181,247,150,14,2,172,
143,223,117,108,201,86,183,219,13,31,112,127,127,214,189,98,244,102,10,18,71,51,44,197,222,95,58,7,139,103,197,8,152,4,43,74,108,193,86,46,53,55,113,248,138,70,89,201,83,144,86,162,170,155,59,147,60,93,
58,7,139,54,185,131,171,0,137,139,208,25,221,71,251,174,125,104,140,7,87,117,243,116,146,59,199,120,246,66,140,82,144,252,176,88,143,223,43,29,128,197,218,73,242,33,229,8,152,170,190,107,79,36,121,50,
137,79,133,25,195,143,15,231,130,54,166,170,155,15,86,117,179,19,229,168,8,43,72,43,224,147,51,70,178,147,228,231,250,174,125,176,116,16,128,253,170,234,230,145,36,127,45,126,47,178,121,31,63,234,239,
68,95,159,7,102,139,29,7,167,28,49,146,79,94,188,176,17,96,142,170,186,121,46,201,173,165,115,176,56,119,245,93,251,204,65,255,159,170,186,57,147,228,211,241,158,237,160,70,41,72,215,109,250,129,76,199,
112,17,172,111,52,54,105,18,119,63,0,28,213,197,131,245,14,191,179,97,191,151,3,190,247,50,76,100,122,188,121,94,168,225,7,254,13,165,115,176,24,187,217,91,53,82,142,128,69,25,62,125,254,112,233,28,44,
198,177,225,61,216,53,85,117,243,177,225,156,145,114,52,49,182,216,45,80,85,55,143,37,121,111,233,28,44,198,51,125,215,222,85,58,4,192,216,124,146,207,6,189,212,119,237,59,46,247,31,84,117,243,80,146,
159,142,133,138,77,112,6,137,107,27,190,233,126,166,116,14,22,161,216,45,225,0,37,85,117,243,98,146,183,151,206,193,236,189,225,247,104,85,55,31,76,242,247,162,24,109,146,130,196,181,85,117,227,15,148,
163,218,77,242,29,135,57,100,10,176,20,195,216,230,95,72,114,172,116,22,102,237,124,146,31,78,242,107,81,140,198,160,32,113,117,195,62,86,63,200,57,138,179,125,215,158,46,29,2,96,42,12,113,128,73,115,
81,44,87,54,140,243,86,142,56,172,221,36,79,42,71,0,111,52,188,249,250,64,92,50,11,83,244,213,49,30,106,5,105,1,134,113,222,38,214,113,88,143,247,93,123,111,233,16,0,83,87,213,205,179,73,190,37,62,144,
132,169,184,226,48,140,163,112,15,210,204,85,117,243,84,148,35,14,199,157,70,0,7,208,119,237,29,137,15,38,97,66,46,140,241,80,91,236,102,172,170,155,251,146,188,187,116,14,102,233,243,202,17,192,225,12,
63,63,31,47,157,3,24,135,45,118,51,85,213,205,201,36,175,149,206,193,236,236,244,93,123,162,116,8,128,37,24,62,168,252,231,177,229,14,74,25,229,174,70,43,72,243,165,28,113,80,79,42,71,0,155,211,119,237,
99,125,215,30,79,242,124,233,44,176,82,47,141,241,80,103,144,102,104,152,88,7,251,181,147,228,100,223,181,59,165,131,0,44,81,223,181,183,37,73,85,55,175,37,57,89,56,14,172,201,23,199,120,168,21,164,153,
169,234,230,233,248,115,99,255,62,219,119,237,9,229,8,96,124,125,215,94,159,145,62,209,6,46,235,243,99,60,212,27,237,25,25,246,58,223,89,58,7,179,113,87,223,181,119,151,14,1,176,38,195,200,225,15,151,
206,1,43,241,204,24,15,53,164,97,38,170,186,57,158,196,214,58,246,227,194,240,41,38,0,5,13,91,226,125,24,13,227,121,95,223,181,143,109,250,161,190,105,231,67,57,226,90,118,147,124,92,57,2,152,134,97,48,
142,113,224,48,158,81,206,32,89,65,154,1,159,64,177,15,46,125,5,152,48,191,203,97,20,183,245,93,187,241,41,146,190,81,39,174,170,155,231,226,207,137,171,59,167,28,1,76,219,176,154,100,128,3,108,214,151,
198,120,168,55,222,19,86,213,205,173,73,110,45,157,131,73,123,180,239,218,155,75,135,0,224,218,134,1,14,163,76,221,130,149,122,101,140,135,218,98,55,97,85,221,236,196,237,220,92,158,65,12,0,51,85,213,
205,153,36,159,142,15,170,225,72,250,174,29,229,125,178,130,52,81,246,42,115,21,103,251,174,61,93,58,4,0,71,83,213,205,11,73,110,41,157,3,230,106,172,130,228,13,248,4,85,117,243,114,252,217,112,121,31,
85,142,0,150,97,248,121,254,104,233,28,192,27,89,65,154,152,170,110,62,145,228,254,210,57,152,156,221,190,107,149,102,128,5,242,187,31,14,199,10,210,122,248,1,201,155,157,87,142,0,150,171,239,218,7,146,
220,148,100,167,116,22,64,65,154,148,225,220,17,92,234,25,35,188,1,150,175,239,218,87,135,81,224,166,220,65,97,10,210,68,84,117,243,116,252,121,240,70,239,233,187,246,174,210,33,0,216,158,190,107,239,
72,242,241,210,57,96,205,156,65,154,128,170,110,238,72,242,7,165,115,48,25,206,27,1,172,92,85,55,15,37,249,153,210,57,96,202,156,65,90,54,229,136,139,118,148,35,0,250,174,125,36,201,245,73,124,146,13,
91,230,141,88,97,85,221,188,86,58,3,147,113,118,216,127,14,0,233,187,246,194,240,161,217,185,210,89,96,77,20,164,130,170,186,121,42,201,201,210,57,152,4,247,27,1,112,89,125,215,222,156,228,108,233,28,
176,22,10,82,89,239,46,29,128,73,120,87,223,181,15,149,14,1,192,116,13,31,162,125,182,116,14,152,144,209,182,159,42,72,133,216,90,199,224,116,223,181,207,150,14,1,192,244,245,93,123,119,146,15,149,206,
1,75,103,138,93,1,85,221,188,144,228,150,210,57,40,106,199,121,35,0,14,163,170,155,91,146,252,47,73,70,153,224,5,51,49,218,212,95,43,72,91,86,213,205,39,162,28,173,157,97,12,0,28,90,223,181,103,135,55,
134,59,165,179,192,18,41,72,219,119,127,233,0,20,245,188,97,12,0,108,194,240,97,155,146,4,27,166,32,109,81,85,55,175,150,206,64,81,231,250,174,189,173,116,8,0,150,99,40,73,198,128,195,6,41,72,91,82,213,
205,61,73,110,40,157,131,98,206,14,99,90,1,96,163,134,223,47,47,149,206,1,75,161,32,109,207,175,151,14,64,49,239,179,173,14,128,49,245,93,251,142,36,207,151,206,1,91,100,204,247,156,85,117,243,114,76,
154,89,163,221,36,55,247,93,251,88,233,32,0,44,223,176,141,251,147,165,115,192,220,41,72,35,27,166,214,157,42,157,131,34,254,55,125,215,218,23,14,192,214,244,93,123,95,146,15,151,206,1,115,166,32,141,
207,212,186,117,186,173,239,218,179,165,67,0,176,62,125,215,62,28,23,202,194,161,93,87,58,192,146,85,117,243,122,233,12,108,157,11,96,1,40,174,239,218,143,84,117,243,182,36,63,89,58,11,204,141,21,164,
145,12,91,235,252,243,93,151,243,202,17,0,83,49,172,36,221,91,58,7,204,141,55,240,227,249,177,210,1,216,170,157,190,107,111,44,29,2,0,46,213,119,237,227,73,174,47,157,3,230,68,65,26,65,85,55,47,196,212,
186,53,217,181,114,4,192,84,245,93,123,33,201,77,165,115,192,92,40,72,27,86,213,205,125,73,110,41,157,131,173,217,233,187,214,247,17,0,147,214,119,237,171,177,221,14,246,197,27,187,205,251,229,210,1,216,
154,115,86,142,0,152,139,97,187,221,187,74,231,128,169,83,144,54,168,170,155,215,74,103,96,107,206,247,93,123,115,233,16,0,112,16,125,215,62,155,196,135,123,112,21,10,210,134,84,117,243,72,146,147,165,
115,176,21,6,50,0,48,91,125,215,238,196,224,6,184,34,5,105,115,254,90,233,0,108,133,123,142,0,152,189,97,112,195,93,165,115,192,20,41,72,27,80,213,205,19,241,207,114,13,148,35,0,22,163,239,218,103,146,
124,168,116,14,152,26,111,234,55,227,123,75,7,96,116,46,129,5,96,113,250,174,253,72,146,103,74,231,128,67,120,125,172,7,43,72,71,84,213,205,171,165,51,48,186,93,103,142,0,88,176,39,75,7,128,41,81,144,
142,96,24,204,112,67,233,28,140,203,61,71,0,44,213,240,94,230,253,165,115,192,148,92,87,58,192,204,253,120,233,0,140,206,40,111,0,22,167,170,155,59,146,252,94,124,208,11,111,161,32,29,146,59,143,22,111,
215,202,17,0,75,83,213,205,195,73,126,34,118,17,193,21,41,72,135,48,124,234,226,206,163,101,251,186,210,1,0,96,147,170,186,121,49,201,219,75,231,128,169,83,144,14,231,247,74,7,96,84,143,246,93,107,248,
6,0,139,80,213,205,99,73,222,91,58,7,204,133,130,116,64,85,221,60,20,251,117,151,236,249,190,107,31,40,29,2,0,142,170,170,155,251,147,252,124,108,167,131,3,81,144,14,238,191,45,29,128,209,60,217,119,237,
153,210,33,0,224,40,134,98,244,79,227,56,0,28,138,130,116,0,195,222,221,99,165,115,48,138,103,148,35,0,230,172,170,155,91,147,252,113,172,24,177,14,59,99,61,88,65,218,167,170,110,78,197,193,198,165,122,
169,239,218,187,74,135,0,128,195,170,234,230,133,36,183,148,206,1,75,160,32,237,223,151,74,7,96,20,59,125,215,190,163,116,8,0,56,140,97,108,247,127,25,59,92,96,99,20,164,125,24,110,153,182,92,189,60,187,
125,215,158,40,29,2,0,14,170,170,155,251,146,252,82,188,63,97,189,70,251,218,87,144,246,231,175,149,14,192,230,185,8,22,128,185,25,166,233,254,116,20,35,112,6,169,148,170,110,62,17,63,132,150,232,230,
210,1,0,224,32,170,186,121,45,38,211,193,232,20,164,107,251,177,210,1,216,184,143,247,93,123,174,116,8,0,216,143,170,110,158,74,242,238,210,57,96,45,20,164,171,24,38,194,56,244,184,44,231,250,174,125,
176,116,8,0,184,150,170,110,62,152,228,239,197,78,22,184,156,175,142,245,96,5,233,234,140,203,92,150,115,125,215,218,90,7,192,164,85,117,243,88,146,31,138,98,4,69,40,72,87,48,236,243,101,57,118,148,35,
0,166,108,184,115,241,75,81,140,96,63,94,25,235,193,10,210,101,12,119,10,56,4,185,28,198,121,3,48,105,85,221,188,156,228,84,233,28,48,35,23,198,122,176,130,116,121,63,81,58,0,27,245,29,165,3,0,192,229,
12,219,233,126,56,206,60,195,65,125,121,172,7,43,72,111,82,213,205,61,177,180,189,36,79,246,93,251,76,233,16,0,112,169,225,26,145,127,63,222,115,192,97,157,31,235,193,10,210,91,253,90,233,0,108,204,185,
190,107,207,148,14,1,0,23,13,197,232,199,98,197,8,142,234,127,29,235,193,10,210,37,134,219,169,125,146,179,12,134,50,0,48,41,85,221,188,154,228,134,210,57,96,33,254,100,172,7,43,72,111,244,211,165,3,176,
25,134,50,0,48,21,195,189,138,174,14,129,205,122,97,172,7,43,72,131,170,110,62,21,171,71,75,241,129,210,1,0,160,170,155,143,37,249,43,177,157,14,198,240,252,88,15,86,144,190,230,158,210,1,216,136,207,
246,93,251,72,233,16,0,172,215,176,98,244,13,81,140,96,76,10,210,152,134,187,7,152,191,157,190,107,239,46,29,2,128,117,170,234,230,206,36,191,27,59,82,96,27,62,63,214,131,21,164,61,46,102,91,0,231,142,
0,40,165,170,155,23,147,188,189,116,14,88,145,47,142,245,224,213,23,164,170,110,158,45,157,129,141,248,112,233,0,0,172,79,85,55,79,39,185,179,116,14,88,161,23,199,122,240,234,11,82,146,219,75,7,224,200,
158,233,187,246,225,210,33,0,88,143,170,110,158,72,242,189,165,115,192,138,125,121,172,7,175,186,32,13,159,250,48,111,231,250,174,189,171,116,8,0,214,97,40,70,255,187,24,192,0,165,93,24,235,193,171,46,
72,73,190,189,116,0,142,196,101,176,0,108,77,85,55,175,199,0,6,152,138,209,134,172,173,182,32,13,171,71,62,253,153,183,127,86,58,0,0,203,87,213,205,115,73,110,45,157,3,120,131,215,198,122,240,106,11,82,
28,168,156,187,115,125,215,62,80,58,4,0,203,229,156,17,76,87,223,181,59,99,61,123,149,5,201,189,71,179,103,107,29,0,163,25,38,220,26,226,4,43,181,202,130,20,247,30,205,154,251,142,0,24,67,85,55,31,76,
242,247,99,11,62,172,218,234,10,146,213,163,217,251,100,233,0,0,44,79,85,55,175,37,57,89,58,7,80,222,234,10,82,172,30,205,217,78,223,181,247,149,14,1,192,114,184,232,21,120,179,85,21,164,170,110,62,81,
58,3,135,103,107,29,0,155,82,213,205,11,73,110,41,157,3,152,158,85,21,164,36,63,86,58,0,135,246,76,233,0,0,204,95,85,55,143,37,121,111,233,28,192,116,173,166,32,85,117,115,127,28,186,156,171,157,190,107,
239,42,29,2,128,249,170,234,230,214,36,127,28,23,189,2,215,176,154,130,148,228,231,75,7,224,112,108,173,3,224,40,170,186,121,53,201,13,165,115,0,243,176,138,130,52,140,237,244,137,209,60,61,90,58,0,0,
243,228,62,35,224,48,86,81,144,146,252,189,210,1,56,148,115,125,215,62,80,58,4,0,243,50,92,233,97,106,45,112,40,139,47,72,85,221,60,28,171,71,115,180,219,119,237,205,165,67,0,48,31,46,122,5,54,97,241,
5,41,201,79,148,14,192,161,252,126,233,0,0,204,135,139,94,129,77,89,116,65,170,234,230,120,172,30,205,145,169,117,0,236,75,85,55,47,38,121,123,233,28,192,114,44,186,32,37,249,66,233,0,28,202,141,165,3,
0,48,109,195,57,163,175,139,237,116,176,70,187,99,62,124,233,5,201,13,217,243,243,100,223,181,23,74,135,0,96,154,170,186,121,33,126,191,195,218,141,250,193,200,98,11,210,112,83,54,243,178,211,119,237,
153,210,33,0,152,158,170,110,238,76,242,187,177,117,30,176,130,116,104,63,84,58,0,7,246,245,165,3,0,48,61,198,118,3,219,180,200,130,84,213,205,253,241,9,211,220,156,237,187,246,92,233,16,0,76,135,1,12,
64,9,139,44,72,73,126,190,116,0,14,166,239,218,211,165,51,0,48,13,86,140,128,146,22,87,144,170,186,185,53,86,143,230,230,249,210,1,0,40,175,170,155,167,147,220,89,58,7,176,110,139,43,72,73,254,184,116,
0,14,100,183,239,218,219,74,135,0,160,156,170,110,78,38,249,114,146,27,74,103,1,88,98,65,178,122,52,47,63,80,58,0,0,101,12,197,232,127,141,237,116,192,132,44,170,32,13,135,57,153,143,179,125,215,62,94,
58,4,0,219,85,213,205,241,36,175,38,57,89,58,11,192,155,45,109,181,197,152,232,249,216,53,152,1,96,125,134,139,94,95,143,114,4,76,212,98,86,144,134,225,12,163,222,170,203,70,253,195,210,1,0,216,158,170,
110,62,150,228,175,196,239,106,224,232,92,20,187,79,191,95,58,0,251,182,211,119,237,67,165,67,0,48,190,97,59,221,151,226,156,17,48,19,75,218,98,231,7,239,124,252,92,233,0,0,108,205,175,37,249,186,210,
33,0,246,235,216,238,238,168,43,84,91,81,213,205,83,73,222,93,58,7,251,178,211,119,237,137,210,33,0,216,46,151,191,2,27,52,234,251,201,165,172,32,125,87,233,0,236,219,55,150,14,0,192,246,245,93,123,115,
146,155,146,236,148,206,2,112,53,179,47,72,85,221,220,159,5,252,239,88,137,115,125,215,158,45,29,2,128,50,250,174,125,117,248,212,247,153,210,89,0,174,100,9,197,226,159,150,14,192,254,12,159,30,2,176,
114,125,215,222,149,228,68,146,243,165,179,0,188,217,172,11,82,85,55,119,196,61,10,115,241,201,210,1,0,152,142,190,107,119,250,174,189,49,201,233,40,74,192,132,204,186,32,37,249,237,210,1,216,151,157,
190,107,239,43,29,2,128,233,233,187,246,236,80,148,62,90,58,11,64,50,255,130,100,108,232,60,252,106,233,0,0,76,91,223,181,15,245,93,123,44,86,147,128,194,102,91,144,134,237,117,110,227,158,1,171,71,0,
236,215,176,154,244,64,76,187,3,10,153,109,65,74,242,100,233,0,236,203,231,75,7,0,96,94,250,174,125,116,152,118,247,225,36,243,191,176,17,152,149,57,23,164,111,40,29,128,107,218,237,187,246,142,210,33,
0,152,167,190,107,31,238,187,246,120,140,5,7,182,104,150,5,169,170,155,59,99,123,221,28,252,72,233,0,0,204,95,223,181,119,57,159,4,108,203,44,11,82,146,255,111,233,0,92,211,78,223,181,143,149,14,1,192,
114,12,231,147,92,56,14,140,106,174,5,233,84,233,0,92,211,159,45,29,0,128,229,233,187,246,244,176,154,244,82,233,44,192,50,205,174,32,85,117,243,108,233,12,92,211,249,190,107,159,45,29,2,128,229,234,187,
246,29,182,221,1,99,152,93,65,74,242,205,165,3,112,77,119,151,14,0,192,58,12,219,238,62,16,211,238,128,13,57,182,187,59,159,159,39,85,221,220,154,228,185,210,57,184,170,157,97,52,43,0,108,85,85,55,207,
37,185,181,116,14,96,116,23,250,174,189,126,172,135,207,109,5,233,247,75,7,224,154,254,168,116,0,0,214,169,239,218,219,146,188,39,46,153,133,165,27,245,123,124,110,5,201,112,134,105,115,239,17,0,69,245,
93,251,196,176,147,225,3,81,148,96,169,190,58,230,195,103,83,144,170,186,249,88,233,12,92,211,207,150,14,0,0,73,210,119,237,35,67,81,122,188,116,22,96,94,102,83,144,146,252,165,210,1,184,170,157,190,107,
31,44,29,2,0,46,213,119,237,189,195,180,59,171,73,176,28,86,144,6,111,43,29,128,171,250,103,165,3,0,192,149,12,171,73,31,143,105,119,176,4,23,198,124,248,44,166,216,85,117,115,60,201,235,165,115,112,69,
38,215,1,48,27,195,157,138,183,151,206,1,28,218,243,195,80,150,81,204,101,5,233,143,75,7,224,170,254,81,233,0,0,176,95,125,215,222,49,108,187,123,169,116,22,224,80,190,60,230,195,231,82,144,190,177,116,
0,174,104,183,239,218,135,74,135,0,128,131,234,187,246,29,73,110,142,243,73,192,37,230,82,144,230,146,115,141,254,176,116,0,0,56,172,190,107,207,13,219,196,207,149,206,2,236,219,249,49,31,62,249,226,81,
213,205,11,165,51,112,85,223,94,58,0,0,28,85,223,181,55,103,111,53,105,212,195,223,192,70,188,58,230,195,39,95,144,146,188,179,116,0,174,232,66,223,181,126,145,0,176,8,195,106,210,245,73,110,138,162,4,
83,246,202,152,15,159,116,65,170,234,230,198,76,60,227,202,53,165,3,0,192,166,245,93,251,234,80,148,62,89,58,11,176,125,83,47,31,255,166,116,0,174,104,167,239,218,71,74,135,0,128,177,244,93,123,159,105,
119,48,73,127,50,230,195,167,94,144,92,14,59,93,63,87,58,0,0,108,195,48,237,238,67,113,201,44,76,197,168,51,10,38,91,144,170,186,249,96,38,156,111,229,118,251,174,125,176,116,8,0,216,150,190,107,63,210,
119,237,241,36,79,70,81,130,69,155,114,1,249,112,233,0,92,209,47,150,14,0,0,37,244,93,123,102,40,74,207,148,206,2,43,54,234,247,223,148,11,210,77,165,3,112,121,125,215,62,80,58,3,0,148,212,119,237,93,
217,123,175,226,146,89,216,190,151,199,124,248,148,11,210,148,179,173,217,217,210,1,0,96,10,134,105,119,39,178,183,237,14,216,158,255,101,204,135,79,178,132,84,117,243,80,233,12,92,209,29,165,3,0,192,
148,12,219,238,142,37,57,87,58,11,172,196,42,167,216,253,223,74,7,224,178,118,251,174,245,195,31,0,46,163,239,218,155,147,92,159,228,124,233,44,176,112,175,142,249,240,169,22,164,175,47,29,128,203,250,
74,233,0,0,48,101,125,215,94,232,187,246,198,36,15,196,180,59,24,203,139,99,62,124,170,5,233,88,233,0,188,213,240,201,24,0,112,13,125,215,62,58,76,187,179,243,2,54,239,181,49,31,62,185,130,84,213,205,
83,165,51,112,89,23,74,7,0,128,185,25,62,92,124,32,166,221,193,38,141,186,141,117,114,5,41,201,119,149,14,192,101,253,197,210,1,0,96,142,134,213,164,19,73,62,16,69,9,54,97,212,15,238,167,88,144,166,152,
105,237,118,250,174,125,188,116,8,0,152,179,190,107,31,25,138,210,243,165,179,192,204,189,50,230,195,39,85,70,170,186,121,172,116,6,46,235,55,74,7,0,128,165,232,187,246,182,97,44,184,105,119,112,56,175,
143,249,240,73,21,164,36,63,88,58,0,111,177,219,119,237,189,165,67,0,192,210,12,211,238,62,94,58,7,240,70,83,43,72,215,149,14,192,91,24,237,13,0,35,233,187,246,193,97,53,233,108,233,44,48,35,235,152,98,
87,213,205,195,49,222,123,138,238,40,29,0,0,150,174,239,218,211,67,81,122,169,116,22,152,186,190,107,71,29,118,50,153,130,148,228,67,165,3,240,22,59,125,215,250,68,11,0,182,164,239,218,119,36,185,43,166,
221,65,49,83,42,72,55,149,14,192,91,124,177,116,0,0,88,155,190,107,159,49,237,14,202,153,82,65,154,82,22,246,252,221,210,1,0,96,173,46,153,118,231,178,118,216,162,73,148,146,170,110,62,81,58,3,111,213,
119,237,71,74,103,0,128,181,235,187,246,250,36,239,139,109,119,176,21,147,40,72,73,190,191,116,0,222,226,92,233,0,0,192,158,190,107,31,27,182,221,25,11,14,35,155,74,65,122,123,233,0,188,197,143,148,14,
0,0,188,209,37,99,193,125,144,9,35,153,74,65,58,89,58,0,111,176,211,119,237,227,165,67,0,0,151,215,119,237,205,73,30,77,178,91,58,11,44,77,241,130,84,213,141,114,52,61,166,215,1,192,196,245,93,251,64,
223,181,199,147,60,25,69,9,54,166,120,65,74,242,223,151,14,192,27,245,93,123,186,116,6,0,96,127,250,174,61,51,20,37,151,204,194,6,76,161,32,253,133,210,1,120,3,19,114,0,96,134,250,174,125,135,177,224,
112,116,83,40,72,215,149,14,192,27,252,78,233,0,0,192,225,13,99,193,157,37,134,67,154,66,65,58,86,58,0,95,211,119,237,221,165,51,0,0,71,211,119,237,189,73,78,196,106,18,28,88,209,130,84,213,205,179,37,
95,159,183,176,189,14,0,22,162,239,218,157,97,53,233,166,24,11,14,251,86,122,5,233,182,194,175,207,27,125,166,116,0,0,96,179,250,174,125,117,24,11,254,225,152,118,7,215,84,186,32,57,127,52,45,247,148,
14,0,0,140,163,239,218,135,135,105,119,86,147,224,42,138,21,164,225,254,35,231,143,38,164,239,218,87,75,103,0,0,198,53,172,38,221,22,91,235,225,178,74,174,32,61,87,240,181,121,43,135,56,1,96,37,250,174,
125,190,239,218,19,73,62,30,219,238,224,13,74,22,164,183,23,124,109,222,234,108,233,0,0,192,118,245,93,251,224,176,237,238,201,210,89,96,42,74,22,36,231,143,38,164,239,90,3,51,0,96,165,250,174,61,51,92,
50,123,190,116,22,40,173,72,65,114,254,104,114,236,65,6,0,210,119,237,141,73,158,41,157,3,74,42,181,130,244,249,66,175,203,229,125,177,116,0,0,96,26,250,174,189,107,88,77,50,237,142,85,42,85,144,190,190,
208,235,114,25,125,215,158,46,157,1,0,152,150,190,107,111,86,148,88,163,82,5,233,166,66,175,203,91,217,94,7,0,92,209,48,22,252,93,241,158,129,149,40,85,144,156,63,154,142,47,151,14,0,0,76,91,223,181,207,
14,99,193,157,79,98,241,20,36,190,173,116,0,0,96,30,250,174,189,43,46,153,165,172,209,239,237,218,122,65,170,234,230,204,182,95,147,43,218,233,187,214,253,71,0,192,190,93,114,201,236,7,162,40,177,64,37,
86,144,254,126,129,215,228,242,46,148,14,0,0,204,83,223,181,143,12,69,233,147,165,179,192,38,149,40,72,223,83,224,53,185,188,159,47,29,0,0,152,183,190,107,239,27,166,221,249,224,149,69,40,81,144,190,174,
192,107,114,25,125,215,62,88,58,3,0,176,12,125,215,94,159,228,67,217,194,25,17,24,83,137,130,100,64,3,0,192,2,245,93,251,145,190,107,143,39,249,108,233,44,112,88,165,166,216,81,158,101,112,0,96,20,125,
215,222,61,108,187,51,12,138,217,217,106,65,170,234,230,145,109,190,30,87,245,92,233,0,0,192,178,245,93,123,58,201,137,248,96,150,205,89,220,152,239,247,111,249,245,184,50,19,103,0,128,209,245,93,187,
51,156,79,178,154,196,44,108,187,32,221,180,229,215,227,10,250,174,125,168,116,6,0,96,61,250,174,61,109,218,29,115,176,237,130,100,64,195,52,152,46,3,0,20,49,172,38,221,20,151,204,50,81,10,210,58,125,
165,116,0,0,96,189,250,174,125,117,184,100,246,209,248,224,150,131,121,125,236,23,216,90,65,170,234,230,19,219,122,45,174,233,55,74,7,0,0,232,187,246,129,97,44,184,243,73,76,198,54,87,144,126,120,139,
175,197,85,244,93,123,95,233,12,0,0,23,13,211,238,238,141,213,36,174,109,244,173,153,219,44,72,215,111,241,181,184,50,251,125,1,128,201,233,187,246,241,97,53,233,201,40,74,20,180,205,130,116,98,139,175,
197,149,153,28,3,0,76,86,223,181,103,134,162,244,76,233,44,172,211,54,11,210,182,7,66,112,121,191,94,58,0,0,192,181,244,93,123,215,48,22,220,238,23,182,106,43,165,165,170,155,51,219,120,29,174,205,249,
35,0,96,78,134,105,119,46,184,231,162,175,142,253,2,219,90,213,249,165,45,189,14,87,231,19,24,0,96,118,250,174,189,111,88,77,58,87,58,11,203,183,173,130,116,211,150,94,135,171,27,125,110,60,0,192,88,250,
174,189,57,123,231,218,21,37,70,179,173,130,100,130,221,52,252,65,233,0,0,0,71,209,119,237,206,80,148,238,141,221,49,140,96,91,5,201,4,187,105,248,129,210,1,0,0,54,97,24,11,126,34,201,75,165,179,176,44,
219,42,72,38,216,77,64,223,181,207,151,206,0,0,176,73,125,215,190,35,201,109,177,154,180,22,175,140,253,2,163,23,151,170,110,110,28,251,53,216,23,23,174,1,0,139,212,119,237,243,195,106,210,3,81,148,56,
162,109,172,236,188,111,11,175,193,181,141,62,18,17,0,160,164,190,107,31,29,138,210,147,165,179,48,95,219,40,72,255,193,22,94,131,107,51,160,1,0,88,133,190,107,207,24,11,206,97,109,163,32,221,189,133,
215,224,218,62,90,58,0,0,192,54,13,211,238,62,92,58,7,243,178,141,130,116,235,22,94,131,107,251,217,210,1,0,0,182,173,239,218,135,135,213,164,179,165,179,176,17,23,198,126,129,109,20,164,183,109,225,53,
184,6,19,236,0,128,53,235,187,246,180,162,196,126,108,163,32,185,36,182,60,19,236,0,0,178,87,148,146,220,28,211,238,184,130,109,20,164,99,91,120,13,174,78,65,2,0,24,244,93,123,110,152,118,247,249,210,
89,56,176,47,143,253,2,46,112,93,135,209,247,106,2,0,204,77,223,181,119,12,219,238,206,151,206,194,116,40,72,235,224,14,36,0,128,43,232,187,246,198,236,221,221,105,219,29,227,22,164,170,110,238,24,243,
249,236,219,191,46,29,0,0,96,202,250,174,125,108,216,118,247,161,56,158,48,101,163,175,246,141,189,130,244,127,28,249,249,236,67,223,181,238,162,2,0,216,135,190,107,63,210,119,237,241,184,100,118,181,
20,36,0,0,120,147,225,146,217,7,98,219,221,212,188,58,246,11,140,93,144,110,27,249,249,92,155,111,106,0,128,67,232,187,246,209,97,219,221,147,177,237,110,53,198,46,72,39,70,126,62,215,102,130,29,0,192,
17,244,93,123,102,216,118,231,146,217,21,48,197,110,249,76,176,3,0,216,128,190,107,79,15,99,193,125,0,93,206,43,99,191,192,216,5,201,37,177,229,253,171,210,1,0,0,150,164,239,218,235,147,124,178,116,14,
198,161,32,45,223,247,149,14,0,0,176,52,125,215,222,231,146,217,34,102,191,130,68,97,125,215,90,2,6,0,24,201,112,201,236,137,24,11,190,24,163,21,164,170,110,110,29,235,217,236,155,9,118,0,0,35,235,187,
118,103,24,11,254,190,152,118,55,182,175,140,253,2,99,174,32,245,35,62,155,253,121,189,116,0,0,128,181,232,187,246,49,211,238,70,55,235,130,244,253,35,62,155,253,177,130,4,0,176,101,125,215,158,78,114,
125,188,23,155,165,49,11,210,219,70,124,54,251,99,196,55,0,64,1,125,215,94,24,46,153,125,52,138,210,38,141,126,214,107,204,130,100,0,68,121,159,41,29,0,0,96,205,250,174,125,96,40,74,143,151,206,194,254,
140,89,98,78,140,248,108,246,231,175,150,14,0,0,64,210,119,237,189,195,88,112,211,238,38,206,42,207,130,245,93,251,108,233,12,0,0,124,205,48,237,238,209,210,57,184,50,5,9,0,0,182,104,216,118,103,53,233,
112,158,31,251,5,198,44,72,199,70,124,54,0,0,204,90,223,181,55,43,74,211,163,32,45,151,75,202,0,0,102,96,216,118,119,58,166,221,237,199,151,199,126,1,91,236,150,75,65,2,0,152,137,190,107,207,14,211,238,
62,91,58,203,218,141,82,144,170,186,185,99,140,231,2,0,192,146,245,93,123,119,92,50,91,212,88,43,72,255,245,72,207,101,255,94,41,29,0,0,128,131,187,228,146,217,15,68,81,122,179,127,61,246,11,140,85,144,
190,127,164,231,178,127,163,127,241,0,0,48,158,190,107,31,25,138,210,71,227,248,196,214,140,85,144,110,26,233,185,236,223,167,75,7,0,0,224,232,250,174,125,168,239,218,227,49,237,110,43,12,105,88,46,23,
144,1,0,44,200,48,237,238,222,172,123,53,105,244,99,36,10,210,114,253,203,210,1,0,0,216,172,190,107,31,31,86,147,62,95,58,203,82,141,85,144,174,27,233,185,236,83,223,181,150,96,1,0,22,170,239,218,59,134,
75,102,159,47,157,101,203,190,50,246,11,140,85,144,172,76,1,0,192,200,250,174,189,109,40,74,231,75,103,89,138,177,138,204,137,145,158,11,0,0,188,73,223,181,55,198,182,187,141,176,210,3,0,0,11,112,201,
182,187,37,175,38,189,58,246,11,40,72,0,0,176,32,195,106,210,137,36,23,74,103,25,193,139,99,191,128,130,4,0,0,11,211,119,237,78,223,181,215,39,249,80,214,61,22,252,192,20,36,0,0,88,168,190,107,63,50,140,
5,95,202,180,187,215,198,126,129,177,10,210,177,145,158,203,254,236,148,14,0,0,192,116,244,93,123,91,146,119,197,251,196,107,178,130,4,0,0,43,208,119,237,179,125,215,158,72,242,76,230,187,237,238,229,
177,95,64,65,90,166,215,75,7,0,0,96,154,250,174,189,107,216,118,247,217,210,89,14,225,139,99,191,128,45,118,0,0,176,66,125,215,222,61,140,5,159,205,180,187,190,107,71,223,34,104,5,105,153,236,45,5,0,96,
95,134,105,119,31,206,124,183,221,109,212,198,11,82,85,55,74,87,121,95,45,29,0,0,128,249,232,187,246,225,97,219,221,75,165,179,148,54,70,153,121,231,8,207,4,0,0,70,214,119,237,59,146,92,159,228,92,225,
40,197,140,81,144,110,30,225,153,28,140,21,36,0,0,14,165,239,218,11,125,215,222,156,228,182,172,240,232,198,24,5,233,91,70,120,38,0,0,176,69,125,215,62,63,140,5,95,213,182,187,49,10,210,119,142,240,76,
0,0,160,128,97,219,221,137,172,100,53,105,140,130,244,239,140,240,76,14,102,54,163,26,1,0,152,190,190,107,119,134,213,164,15,100,225,69,105,140,130,116,251,8,207,228,96,78,150,14,0,0,192,242,244,93,251,
200,80,148,158,44,157,101,44,99,20,164,155,70,120,38,0,0,48,17,125,215,158,25,46,153,61,187,197,151,221,202,61,77,99,20,164,111,26,225,153,28,140,45,118,0,0,140,174,239,218,211,73,222,151,5,93,50,59,70,
65,122,219,8,207,4,0,0,38,168,239,218,199,134,75,102,23,113,119,210,24,5,9,0,0,88,153,190,107,111,30,182,221,61,95,58,203,81,140,81,144,190,60,194,51,1,0,128,25,232,187,246,182,36,167,179,249,105,119,
179,61,131,244,111,70,120,38,0,0,48,19,125,215,158,29,166,221,61,94,58,203,65,141,81,144,254,100,132,103,2,0,0,51,211,119,237,189,195,182,187,243,165,179,236,215,24,5,233,169,17,158,9,0,0,204,84,223,181,
55,102,111,218,221,228,139,210,24,5,233,255,55,194,51,57,24,23,197,2,0,48,41,195,180,187,27,147,60,144,205,159,79,218,24,91,236,150,233,186,210,1,0,0,224,114,250,174,125,116,56,159,244,82,233,44,151,51,
70,65,250,253,17,158,201,193,124,181,116,0,0,0,184,154,190,107,223,145,228,222,76,108,53,105,140,130,244,165,17,158,201,193,88,65,2,0,96,242,250,174,125,124,88,77,122,178,116,150,139,198,40,72,175,140,
240,76,0,0,96,161,250,174,61,51,149,75,102,55,94,144,250,174,157,212,18,217,74,89,65,2,0,96,118,250,174,189,173,244,88,240,49,86,144,40,207,159,43,0,0,179,53,76,187,251,104,137,215,246,70,26,0,0,152,156,
190,107,31,26,86,147,206,109,243,117,199,42,72,187,35,61,151,253,57,81,58,0,0,0,108,66,223,181,55,15,69,233,115,219,120,61,43,72,0,0,192,228,245,93,123,215,54,94,199,10,18,0,0,192,192,10,210,50,249,115,
5,0,128,67,240,70,26,0,0,96,160,32,1,0,0,12,20,36,0,0,128,193,88,5,233,245,145,158,11,0,0,48,154,177,10,210,206,72,207,5,0,0,24,205,88,5,233,171,35,61,151,125,170,234,230,214,210,25,0,0,96,110,156,65,
90,174,111,47,29,0,0,0,230,70,65,90,174,247,151,14,0,0,0,115,51,86,65,122,101,164,231,178,127,127,161,116,0,0,0,152,155,177,10,210,39,71,122,46,251,247,173,165,3,0,0,192,220,140,85,144,254,198,72,207,
101,255,110,42,29,0,0,0,230,102,148,130,212,119,237,217,49,158,11,0,0,48,38,67,26,150,235,88,233,0,0,0,48,55,10,18,0,0,192,96,204,130,180,59,226,179,185,54,43,72,0,0,112,64,86,144,0,0,0,6,86,144,22,172,
170,155,51,165,51,0,0,192,156,140,89,144,94,31,241,217,236,207,63,42,29,0,0,0,230,100,204,130,180,51,226,179,217,31,151,197,2,0,192,1,140,89,144,190,60,226,179,217,159,235,75,7,0,0,128,57,25,179,32,253,
203,17,159,205,254,156,40,29,0,0,0,230,100,204,130,244,127,30,241,217,0,0,0,27,55,90,65,234,187,246,217,177,158,205,190,25,227,14,0,0,7,224,13,244,194,85,117,115,178,116,6,0,0,152,139,177,11,146,187,144,
202,251,76,233,0,0,0,48,23,10,210,242,221,89,58,0,0,0,204,133,45,118,203,231,207,24,0,0,246,105,236,55,207,175,143,252,124,174,205,168,111,0,0,216,167,177,11,210,115,35,63,159,107,59,86,58,0,0,0,204,197,
216,5,169,29,249,249,92,219,177,170,110,108,179,3,0,128,125,24,251,141,243,207,142,252,124,246,231,183,74,7,0,0,128,57,24,181,32,245,93,251,252,152,207,103,223,190,181,116,0,0,0,152,3,91,175,214,225,186,
210,1,0,0,96,14,20,164,117,56,89,58,0,0,0,204,193,54,10,146,203,98,203,51,201,14,0,0,246,97,27,5,233,43,91,120,13,174,78,65,2,0,128,125,216,70,65,122,101,11,175,193,53,84,117,115,75,233,12,0,0,48,117,
219,40,72,38,217,77,195,95,45,29,0,0,0,166,110,27,5,233,169,45,188,6,215,246,227,165,3,0,0,192,212,109,163,32,125,108,11,175,193,181,221,94,58,0,0,0,76,221,54,10,210,175,111,225,53,184,182,235,75,7,0,
0,128,169,27,189,32,245,93,251,234,216,175,193,190,152,100,7,0,0,215,176,173,139,98,119,182,244,58,92,69,85,55,183,150,206,0,0,0,83,182,173,130,244,250,150,94,135,171,251,76,233,0,0,0,48,101,219,42,72,
175,109,233,117,184,58,119,33,1,0,192,85,108,171,32,185,44,118,26,174,43,29,0,0,0,166,108,91,5,233,71,183,244,58,92,157,65,13,0,0,112,21,91,41,72,125,215,62,177,141,215,225,218,170,186,241,103,1,0,0,87,
176,173,21,164,196,36,187,169,184,187,116,0,0,0,152,170,109,22,36,166,225,100,233,0,0,0,48,85,219,44,72,6,53,76,131,82,12,0,0,87,176,205,55,203,191,184,197,215,226,42,170,186,249,84,233,12,0,0,48,69,91,
43,72,125,215,62,184,173,215,226,154,190,167,116,0,0,0,152,162,109,111,183,218,221,242,235,113,121,55,149,14,0,0,0,83,164,32,173,147,115,72,0,0,112,25,219,126,163,108,80,195,68,84,117,243,72,233,12,0,
0,48,53,219,46,72,31,223,242,235,113,101,239,45,29,0,0,0,166,102,171,5,169,239,218,135,182,249,122,92,213,55,149,14,0,0,0,83,227,44,202,122,93,95,58,0,0,0,76,77,137,130,180,83,224,53,121,171,99,165,3,
0,0,192,212,148,40,72,6,53,76,132,65,13,0,0,240,70,37,10,210,103,10,188,38,151,247,87,75,7,0,0,128,41,41,81,144,254,102,129,215,228,242,156,67,2,0,128,75,28,219,221,221,254,221,173,85,221,184,48,118,58,
110,235,187,246,249,210,33,0,0,96,10,74,77,177,83,144,166,227,247,75,7,0,0,128,169,80,144,184,169,116,0,0,0,152,138,82,5,201,36,187,233,112,23,22,0,0,12,74,189,57,254,82,161,215,229,50,170,186,121,161,
116,6,0,0,152,130,82,5,233,207,20,122,93,46,239,157,165,3,0,0,192,20,20,41,72,125,215,238,148,120,93,174,200,54,59,0,0,72,217,55,198,23,10,190,54,111,82,213,205,115,165,51,0,0,64,105,37,11,210,75,5,95,
155,183,186,165,116,0,0,0,40,173,100,65,250,230,130,175,205,91,157,44,29,0,0,0,74,43,86,144,250,174,125,53,238,67,154,148,170,110,78,149,206,0,0,0,37,149,62,156,255,213,194,175,207,27,125,186,116,0,0,
0,40,169,116,65,250,131,194,175,207,27,125,87,233,0,0,0,80,82,209,130,212,119,237,93,37,95,159,183,40,93,152,1,0,160,168,41,188,33,118,14,105,66,170,186,121,162,116,6,0,0,40,101,10,5,201,57,164,105,249,
158,210,1,0,0,160,148,41,20,164,223,42,29,128,55,152,194,215,4,0,0,20,49,133,55,195,127,190,116,0,222,168,170,155,23,74,103,0,0,128,18,138,23,164,190,107,119,74,103,224,45,222,89,58,0,0,0,148,80,188,32,
13,46,148,14,192,27,28,175,234,230,190,210,33,0,0,96,219,166,82,144,206,150,14,192,91,252,124,233,0,0,0,176,109,83,41,72,255,175,210,1,120,139,83,165,3,0,0,192,182,77,162,32,245,93,251,80,233,12,188,85,
85,55,15,151,206,0,0,0,219,52,137,130,52,48,172,97,122,170,210,1,0,0,96,155,166,84,144,94,41,29,128,183,48,205,14,0,128,85,153,82,65,250,201,210,1,120,139,227,85,221,220,82,58,4,0,0,108,203,100,10,82,
223,181,31,73,178,91,58,7,111,241,108,233,0,0,0,176,45,147,41,72,131,175,150,14,192,91,152,102,7,0,192,106,76,173,32,253,139,210,1,120,171,170,110,158,40,157,1,0,0,182,97,82,5,169,239,218,251,74,103,224,
178,190,167,116,0,0,0,216,134,73,21,164,193,133,210,1,120,139,227,85,221,40,175,0,0,44,222,20,11,210,111,149,14,192,101,253,119,165,3,0,0,192,216,142,237,238,78,111,112,92,85,55,211,11,69,250,174,61,86,
58,3,0,192,38,85,117,243,90,146,19,73,62,215,119,237,93,165,243,80,158,130,196,65,156,235,187,246,230,210,33,0,0,54,161,170,155,227,73,94,127,211,223,62,151,228,142,190,107,207,22,136,196,4,76,113,139,
93,146,248,130,156,166,175,43,29,0,0,96,131,190,116,153,191,119,42,201,11,85,221,188,94,213,205,199,182,29,136,242,166,90,144,126,188,116,0,46,235,88,85,55,183,148,14,1,0,176,33,87,187,239,241,120,146,
247,87,117,179,91,213,205,107,85,221,60,181,173,80,148,53,201,45,118,137,109,118,19,118,182,239,218,211,165,67,0,0,28,69,85,55,47,231,234,5,233,74,206,37,249,183,250,174,53,121,121,161,166,186,130,148,
36,59,165,3,112,89,86,144,0,128,37,56,236,209,129,83,73,94,27,182,224,221,191,201,64,76,195,148,11,210,43,165,3,112,121,85,221,124,162,116,6,0,128,195,170,234,230,161,36,71,157,206,123,60,201,39,134,162,
244,236,209,83,49,21,215,149,14,112,21,31,76,242,51,165,67,112,89,255,126,233,0,0,0,71,240,145,13,62,235,120,146,219,135,227,33,23,146,252,63,251,174,125,112,131,207,103,203,38,123,6,41,73,170,186,121,
61,211,94,229,90,179,71,251,174,125,160,116,8,0,128,131,218,210,89,247,11,73,62,221,119,237,189,91,120,45,54,104,234,229,227,203,165,3,112,69,86,145,0,128,217,169,234,230,83,91,122,169,147,73,238,25,182,
224,153,128,55,35,83,47,72,223,84,58,0,87,116,124,216,191,11,0,48,39,255,238,150,95,239,120,146,119,15,227,194,95,222,242,107,115,8,147,222,98,151,216,102,55,113,23,250,174,189,190,116,8,0,128,253,24,
46,126,125,127,233,28,217,155,214,252,171,125,215,222,87,58,8,111,53,135,130,244,92,146,91,75,231,224,138,110,238,187,246,92,233,16,0,0,215,50,209,15,222,119,146,124,174,239,218,187,74,7,97,207,212,190,
64,222,162,239,218,219,74,103,224,170,126,191,116,0,0,128,107,169,234,230,177,76,243,189,239,241,36,119,218,130,55,29,83,30,243,125,169,157,76,243,11,26,231,196,0,128,121,248,161,210,1,246,225,212,37,
227,194,255,195,190,107,31,45,29,104,141,230,82,58,190,80,58,0,87,116,108,216,207,11,0,48,73,195,37,247,115,121,223,155,236,77,192,251,68,85,55,59,85,221,60,93,58,204,218,76,254,12,210,69,91,154,87,207,
225,236,244,93,123,162,116,8,0,128,203,153,232,217,163,131,186,144,228,183,250,174,61,83,58,200,210,205,233,11,101,167,116,0,174,232,248,176,175,23,0,96,138,230,244,158,247,74,78,38,249,222,225,172,210,
107,118,240,140,103,78,95,44,46,141,157,182,57,236,235,5,0,86,102,152,136,188,52,39,147,188,127,184,132,214,22,188,13,155,83,65,250,23,165,3,112,85,199,135,253,189,0,0,83,178,228,129,82,111,152,128,87,
213,205,156,222,219,79,214,108,206,32,37,73,85,55,59,73,142,149,206,193,21,237,246,93,235,27,19,0,152,132,97,117,229,206,210,57,182,108,39,201,103,156,85,58,188,185,21,164,151,147,156,42,157,131,171,250,
124,223,181,119,148,14,1,0,96,200,151,75,104,15,99,46,247,32,93,116,71,146,23,74,135,224,170,190,185,116,0,0,128,170,110,94,44,157,97,2,254,116,11,94,146,115,73,110,233,187,246,213,194,153,38,111,86,219,
161,250,174,61,27,211,236,166,238,120,85,55,107,91,202,6,0,166,231,237,165,3,76,204,169,36,175,12,19,240,30,42,29,102,202,102,85,144,6,159,43,29,128,107,122,170,116,0,0,96,189,134,99,25,92,222,201,36,
63,51,92,66,251,108,233,48,83,52,171,51,72,23,217,79,58,11,119,245,93,251,76,233,16,0,192,250,120,175,120,96,59,73,190,208,119,237,109,165,131,76,193,220,206,32,93,116,46,134,53,76,221,239,38,57,81,58,
4,0,176,46,11,189,247,104,108,199,147,220,58,20,203,11,73,154,190,107,31,41,156,169,152,57,110,177,75,146,239,46,29,128,107,58,94,213,205,7,75,135,0,0,86,231,214,210,1,102,238,226,22,188,215,215,90,54,
103,185,197,46,113,39,210,76,184,23,9,0,216,154,170,110,158,74,242,238,210,57,22,232,92,146,111,236,187,246,92,233,32,219,48,231,55,175,127,82,58,0,215,116,204,225,63,0,96,139,236,50,26,199,169,36,47,
15,171,74,79,148,14,51,182,185,158,65,74,146,187,147,172,114,217,111,102,110,47,29,0,0,88,190,225,222,35,187,139,198,117,60,201,247,14,103,149,22,59,216,97,182,91,236,18,19,74,102,228,201,190,107,207,
148,14,1,0,44,151,247,133,197,236,38,249,74,223,181,55,151,14,178,41,115,94,65,74,76,179,155,139,239,41,29,0,0,88,174,170,110,94,45,157,97,197,142,37,57,117,201,170,210,79,245,93,251,112,217,72,71,51,
231,51,72,137,125,166,115,113,188,170,155,79,149,14,1,0,44,79,85,55,247,37,185,161,116,14,146,236,117,139,159,156,251,37,180,179,222,98,151,36,85,221,188,150,189,113,132,76,92,223,181,246,5,3,0,27,85,
213,205,235,153,255,135,254,75,182,155,228,79,250,174,61,93,58,200,126,45,225,139,233,223,43,29,128,253,169,234,230,229,210,25,0,128,229,168,234,230,76,150,241,126,118,201,142,37,185,101,24,193,62,11,
179,255,130,234,187,246,177,236,53,83,166,239,84,85,55,183,148,14,1,0,44,198,167,75,7,96,127,250,174,189,187,116,134,253,154,125,65,26,252,118,233,0,236,219,23,74,7,0,0,230,175,170,155,135,178,156,247,
178,75,119,190,116,128,131,88,196,23,213,156,26,41,57,94,213,205,99,165,67,0,0,179,247,211,165,3,176,111,179,122,175,190,136,130,52,120,169,116,0,246,237,135,74,7,0,0,230,171,170,155,23,178,172,247,177,
75,182,211,119,237,51,165,67,28,196,146,190,176,190,179,116,0,246,237,120,85,55,31,43,29,2,0,152,45,103,154,231,227,63,47,29,224,160,22,83,144,250,174,125,62,134,53,204,201,95,41,29,0,0,152,159,225,138,
23,230,97,183,239,218,143,148,14,113,80,139,41,72,131,175,148,14,192,190,29,27,150,199,1,0,246,165,170,155,83,113,255,229,156,252,126,233,0,135,177,168,130,212,119,237,205,165,51,112,32,183,12,19,104,
0,0,246,227,75,165,3,176,127,125,215,222,85,58,195,97,44,170,32,13,206,149,14,192,129,252,183,165,3,0,0,211,87,213,205,35,89,230,123,215,165,154,237,0,181,37,126,145,221,81,58,0,7,114,172,170,155,231,
74,135,0,0,38,239,63,45,29,128,3,249,143,74,7,56,172,197,21,164,190,107,207,38,217,41,157,131,3,185,181,116,0,0,96,186,134,115,203,199,74,231,96,223,118,251,174,157,237,189,151,139,43,72,131,31,45,29,
128,131,49,176,1,0,184,156,225,188,178,177,222,243,242,39,165,3,28,197,34,11,210,208,88,173,34,205,203,45,85,221,248,225,7,0,188,153,243,202,51,211,119,237,233,210,25,142,98,145,5,105,240,115,165,3,112,
96,95,40,29,0,0,152,142,170,110,158,141,173,117,115,51,219,225,12,23,45,182,32,245,93,251,96,233,12,28,216,241,170,110,158,40,29,2,0,152,140,111,41,29,128,131,233,187,246,29,165,51,28,213,98,11,210,224,
108,233,0,28,216,247,86,117,115,99,233,16,0,64,89,6,51,204,210,249,210,1,54,97,209,5,105,238,251,31,87,204,93,86,0,176,98,85,221,220,19,131,25,102,167,239,218,69,124,200,189,232,130,52,48,172,97,126,142,
15,123,142,1,128,117,250,181,210,1,56,176,197,124,192,189,134,130,244,159,151,14,192,161,220,94,58,0,0,176,125,85,221,188,152,117,188,71,93,154,127,167,116,128,77,57,182,187,187,91,58,195,232,170,186,
121,61,190,209,230,232,165,37,28,244,3,0,246,167,170,155,135,147,252,100,233,28,28,216,110,223,181,139,121,175,189,152,255,33,215,240,151,75,7,224,80,222,94,213,205,39,74,135,0,0,182,70,57,154,167,63,
44,29,96,147,86,81,144,250,174,125,52,206,34,205,213,253,165,3,0,0,227,171,234,230,185,210,25,56,156,190,107,239,40,157,97,147,86,81,144,6,86,145,102,106,216,34,9,0,44,84,85,55,39,147,220,90,58,7,135,
178,184,107,117,86,83,144,134,85,164,229,31,184,90,166,227,85,221,60,93,58,4,0,48,154,197,76,64,91,155,37,94,171,179,154,130,52,248,197,210,1,56,180,59,75,7,0,0,54,111,184,218,227,100,233,28,28,202,226,
86,143,146,149,76,177,187,84,85,55,235,250,31,188,44,59,125,215,158,40,29,2,0,216,140,170,110,30,73,242,227,165,115,112,40,139,154,92,119,169,69,254,143,186,6,75,184,243,117,188,170,155,199,74,135,0,0,
54,70,57,154,175,255,177,116,128,177,172,174,32,245,93,123,115,233,12,28,201,123,75,7,0,0,142,174,170,155,151,75,103,224,240,250,174,61,83,58,195,88,86,87,144,6,86,145,102,204,84,59,0,152,183,170,110,
238,75,114,170,116,14,14,109,145,103,143,46,90,101,65,178,138,52,123,199,125,234,4,0,179,246,203,165,3,112,120,75,156,92,119,169,85,22,164,193,103,75,7,224,72,78,57,143,4,0,243,99,39,200,236,157,47,29,
96,108,171,45,72,125,215,222,157,100,167,116,14,142,228,135,75,7,0,0,246,111,184,215,112,181,239,63,23,226,109,165,3,140,109,237,95,160,159,43,29,128,35,57,102,171,29,0,204,67,85,55,247,196,189,134,115,
119,190,239,218,11,165,67,140,109,213,5,169,239,218,187,74,103,224,200,78,13,23,204,1,0,211,246,235,165,3,112,52,125,215,222,88,58,195,54,172,186,32,13,158,41,29,128,35,187,189,170,155,135,75,135,0,0,
46,175,170,155,23,147,28,43,157,131,35,249,124,233,0,219,178,250,130,100,21,105,49,126,178,116,0,0,224,173,134,15,49,223,94,58,7,71,211,119,237,29,165,51,108,203,234,11,210,192,189,72,11,96,42,14,0,76,
146,15,49,231,111,53,171,71,73,114,108,119,119,183,116,134,73,168,234,198,63,136,101,120,198,170,32,0,76,195,240,225,165,15,228,103,174,239,218,85,109,143,244,5,251,53,143,151,14,192,70,220,233,60,18,
0,148,167,28,45,198,234,118,90,249,162,29,244,93,123,111,220,139,180,20,150,242,1,160,160,97,194,172,247,153,11,208,119,237,205,165,51,108,155,47,220,55,250,203,165,3,176,25,206,35,1,64,25,85,221,220,
146,228,246,210,57,216,136,179,165,3,148,160,32,93,162,239,218,71,99,21,105,41,142,187,68,22,0,138,248,66,233,0,108,196,110,223,181,167,75,135,40,65,65,122,171,191,80,58,0,27,115,170,170,155,39,74,135,
0,128,181,168,234,230,213,120,127,185,20,63,91,58,64,41,166,216,93,134,67,133,139,243,190,190,107,31,43,29,2,0,150,108,216,185,113,170,116,14,54,226,66,223,181,215,151,14,81,138,18,112,121,127,189,116,
0,54,234,151,75,7,0,128,37,171,234,230,185,40,71,75,242,221,165,3,148,100,5,233,10,134,37,226,27,74,231,96,99,118,250,174,61,81,58,4,0,44,77,85,55,55,38,121,165,116,14,54,102,213,171,71,137,21,164,43,
234,187,246,198,210,25,216,168,227,67,233,5,0,54,107,117,247,228,44,92,83,58,64,105,10,210,213,61,83,58,0,27,117,131,201,118,0,176,57,85,221,236,196,251,201,37,217,233,187,246,145,210,33,74,243,5,125,
21,125,215,222,21,99,191,151,230,84,85,55,31,43,29,2,0,230,174,170,155,215,146,28,43,157,131,141,58,89,58,192,20,40,72,215,246,115,165,3,176,113,239,31,46,177,3,0,14,161,170,155,23,226,205,244,210,156,
237,187,214,194,64,20,164,107,234,187,246,193,88,69,90,162,23,74,7,0,128,57,170,234,230,131,73,124,208,184,44,171,189,20,246,114,20,164,253,249,169,210,1,216,188,97,223,52,0,176,79,85,221,156,74,242,15,
74,231,96,227,254,97,233,0,83,162,32,237,67,223,181,15,199,42,210,18,29,27,46,5,6,0,246,199,176,163,229,57,223,119,237,67,165,67,76,137,130,180,127,46,63,91,166,227,38,219,1,192,181,217,121,177,88,63,
92,58,192,212,184,40,246,0,170,186,121,49,201,219,75,231,96,20,207,12,83,11,1,128,55,25,118,92,248,96,125,121,118,250,174,61,81,58,196,212,248,66,63,128,190,107,223,145,68,163,92,166,59,171,186,121,170,
116,8,0,152,154,225,162,117,239,25,151,233,71,75,7,152,34,95,236,7,247,31,148,14,192,104,222,93,213,205,115,165,67,0,192,84,12,187,103,110,40,157,131,81,60,223,119,237,99,165,67,76,145,45,118,135,48,156,
89,113,38,105,185,30,237,187,246,129,210,33,0,160,164,97,103,197,187,75,231,96,20,182,214,93,133,21,164,195,249,238,210,1,24,213,253,195,24,83,0,88,165,170,110,62,22,229,104,201,108,173,187,10,5,233,16,
250,174,125,54,201,133,210,57,24,213,151,75,7,0,128,18,170,186,121,36,201,251,75,231,96,52,231,109,173,187,58,91,236,142,160,170,27,255,240,150,239,230,190,107,207,149,14,1,0,219,80,213,205,67,73,126,
166,116,14,198,211,119,237,177,210,25,166,206,10,210,209,124,184,116,0,70,247,178,237,118,0,172,193,240,251,78,57,90,182,179,165,3,204,129,130,116,4,125,215,62,156,228,124,233,28,140,206,118,59,0,214,
192,197,233,11,215,119,237,233,210,25,230,64,65,58,162,190,107,111,44,157,129,209,29,27,38,23,2,192,34,57,54,176,10,79,150,14,48,23,10,210,102,60,94,58,0,163,59,53,220,34,14,0,139,81,213,205,41,229,104,
21,206,245,93,123,166,116,136,185,80,144,54,160,239,218,123,147,236,148,206,193,232,142,43,73,0,44,140,29,18,203,183,219,119,237,205,165,67,204,137,130,180,57,63,85,58,0,91,161,36,1,176,8,126,159,173,
198,223,44,29,96,110,140,249,222,160,170,110,94,75,114,178,116,14,182,226,188,243,103,0,204,213,80,142,124,80,190,124,222,175,28,130,111,140,13,234,187,246,250,210,25,216,154,27,12,110,0,96,142,148,163,
85,121,87,233,0,115,228,155,99,243,222,87,58,0,91,99,112,3,0,179,50,12,100,240,254,111,29,206,246,93,251,124,233,16,115,100,139,221,8,170,186,121,54,201,237,165,115,176,53,187,125,215,250,101,3,192,164,
85,117,179,147,228,88,233,28,108,133,247,38,71,224,31,220,8,250,174,189,35,137,230,185,30,199,172,36,1,48,101,202,209,234,124,71,233,0,115,166,32,141,231,23,75,7,96,171,142,87,117,243,106,233,16,0,240,
102,195,135,120,202,209,122,156,237,187,246,153,210,33,230,204,22,187,17,57,4,185,74,59,125,215,158,40,29,2,0,170,186,57,147,228,55,75,231,96,171,46,24,26,118,116,222,188,143,200,27,229,85,58,94,213,205,
238,240,75,9,0,138,168,234,230,84,148,163,53,58,85,58,192,18,40,72,227,251,120,233,0,20,241,155,85,221,220,90,58,4,0,235,51,148,35,87,81,172,207,75,125,215,94,40,29,98,9,108,177,219,130,225,190,28,141,
126,157,110,238,187,246,92,233,16,0,172,67,85,55,119,38,121,186,116,14,182,175,239,90,231,204,54,196,10,210,22,244,93,123,115,76,181,91,171,151,171,186,185,191,116,8,0,150,175,170,155,79,68,57,90,43,59,
150,54,72,65,218,158,63,95,58,0,197,124,162,170,155,23,75,135,0,96,185,134,59,24,125,32,183,78,207,244,93,251,96,233,16,75,98,139,221,22,13,99,160,111,40,157,131,98,206,247,93,123,99,233,16,0,44,75,85,
55,207,37,113,238,117,157,76,173,27,129,130,180,101,85,221,248,7,190,110,103,251,174,61,93,58,4,0,203,224,156,243,186,57,119,52,14,91,236,182,239,209,210,1,40,234,150,170,110,94,40,29,2,128,249,171,234,
230,181,40,71,107,246,217,210,1,150,202,10,82,1,195,27,228,91,74,231,160,40,23,202,2,112,104,46,163,95,61,239,35,70,164,32,21,98,73,156,193,187,250,174,125,182,116,8,0,230,161,170,155,227,73,190,154,196,
214,170,21,179,181,110,92,62,121,40,100,24,253,13,127,80,213,205,61,165,67,0,48,125,195,239,139,215,163,28,173,221,71,75,7,88,58,5,169,44,123,71,73,146,79,85,117,243,72,233,16,0,76,87,85,55,79,37,249,
84,233,28,20,119,174,239,218,135,74,135,88,58,91,236,10,27,14,88,158,44,157,131,73,48,225,14,128,183,48,198,155,129,115,71,91,98,5,169,176,97,118,189,150,74,178,55,225,110,167,116,8,0,166,99,24,198,160,
28,145,36,238,82,220,18,5,105,26,190,181,116,0,38,227,152,146,4,64,85,55,23,63,52,243,94,141,100,111,151,201,133,210,33,214,194,22,187,137,168,234,230,233,36,119,150,206,193,164,220,219,119,237,227,165,
67,0,176,93,195,48,6,231,141,184,200,214,186,45,83,144,38,196,157,6,92,198,51,125,215,222,85,58,4,0,219,81,213,205,39,146,220,95,58,7,211,97,164,247,246,121,51,62,33,62,29,224,50,238,172,234,230,213,210,
33,0,24,223,112,71,162,114,196,165,222,83,58,192,26,93,87,58,0,111,241,104,252,112,228,141,110,24,246,161,127,93,223,181,202,18,192,194,84,117,115,38,201,111,150,206,193,228,60,222,119,237,19,165,67,172,
145,21,164,137,233,187,246,129,36,47,149,206,193,228,28,75,242,202,176,245,2,128,133,168,234,230,161,40,71,188,213,249,190,107,239,45,29,98,173,156,65,154,40,231,145,184,10,247,37,1,44,192,112,249,235,
187,75,231,96,122,156,59,42,203,27,240,137,26,206,35,25,247,204,229,220,50,92,48,12,192,12,85,117,115,114,248,57,174,28,113,57,159,44,29,96,237,20,164,105,251,198,210,1,152,172,147,85,221,236,86,117,99,
111,50,192,140,84,117,243,177,36,175,37,57,89,58,11,147,244,217,190,107,239,43,29,98,237,108,177,155,184,170,110,158,77,114,123,233,28,76,218,185,190,107,111,46,29,2,128,171,115,231,33,215,224,247,249,
68,40,72,51,224,60,18,251,112,161,239,218,235,75,135,0,224,242,134,43,27,110,40,157,131,201,218,237,187,214,123,189,137,80,144,102,162,170,27,127,80,236,199,71,251,174,125,168,116,8,0,246,84,117,243,193,
36,255,160,116,14,38,239,93,125,215,62,91,58,4,123,20,164,25,81,146,216,167,29,151,14,3,148,55,12,98,112,214,136,107,121,166,239,218,187,74,135,224,107,20,164,25,169,234,230,190,36,191,92,58,7,179,241,
30,23,204,1,108,159,139,95,57,0,231,142,38,72,65,154,25,7,60,57,32,159,74,1,108,81,85,55,207,37,185,181,116,14,102,193,142,143,137,82,144,102,200,208,6,14,104,39,201,141,125,215,94,40,29,4,96,201,170,
186,121,57,201,169,210,57,152,5,67,25,38,204,31,204,12,249,180,129,3,58,158,228,53,119,38,1,140,163,170,155,39,170,186,217,137,114,196,254,93,87,58,0,87,230,15,103,190,78,36,121,189,116,8,102,229,123,
135,213,71,171,73,0,27,80,213,205,141,73,206,197,7,206,28,204,163,125,215,238,148,14,193,149,249,134,158,169,225,27,235,125,165,115,48,59,23,87,147,158,43,29,4,96,206,170,186,121,36,201,43,241,94,138,
131,57,215,119,237,3,165,67,112,117,206,32,205,220,176,109,234,123,75,231,96,150,206,247,93,123,99,233,16,0,115,227,172,17,135,100,40,195,76,40,72,11,224,7,53,71,244,100,223,181,103,74,135,0,152,58,31,
74,114,4,202,209,140,40,72,11,97,178,29,71,180,155,228,127,84,148,0,222,170,170,155,91,147,252,113,252,158,229,112,76,172,155,25,127,88,11,49,124,42,161,237,114,88,199,178,55,196,225,133,210,65,0,166,
100,56,179,249,92,188,103,226,240,190,163,116,0,14,198,10,210,194,12,99,70,143,149,206,193,172,237,38,249,145,190,107,31,43,29,4,160,148,170,110,238,72,242,63,69,49,226,104,206,246,93,123,186,116,8,14,
198,55,253,242,252,159,74,7,96,246,142,37,249,229,225,108,27,192,234,12,171,233,127,16,239,147,56,154,243,202,209,60,89,65,90,160,170,110,30,75,242,222,210,57,88,140,103,250,174,189,171,116,8,128,177,
13,219,233,110,45,157,131,69,120,169,239,218,119,148,14,193,225,40,72,11,101,178,29,35,120,188,239,218,123,75,135,0,216,180,170,110,238,79,242,11,177,69,157,205,56,215,119,237,205,165,67,112,120,10,210,
130,85,117,243,106,146,27,74,231,96,81,46,244,93,123,125,233,16,0,155,82,213,205,139,73,222,94,58,7,139,97,98,221,2,40,72,11,103,252,55,35,216,77,242,47,173,38,1,115,102,59,58,35,121,79,223,181,79,148,
14,193,209,40,72,43,160,36,49,146,157,36,191,218,119,237,125,165,131,0,236,151,237,116,140,232,227,125,215,62,88,58,4,71,167,32,173,132,241,223,140,104,55,201,223,238,187,246,225,210,65,0,174,100,88,49,
250,161,248,192,144,113,124,168,239,218,143,148,14,193,102,40,72,43,81,213,205,157,73,158,46,157,131,69,219,73,114,99,223,181,23,74,7,1,184,84,85,55,175,37,57,89,58,7,139,101,229,104,97,20,164,21,169,
234,230,99,73,222,95,58,7,139,103,180,41,48,9,85,221,60,155,228,246,210,57,88,180,207,247,93,123,71,233,16,108,150,130,180,50,85,221,60,149,228,221,165,115,176,120,206,39,1,197,84,117,243,169,36,255,110,
108,167,99,92,38,187,46,148,130,180,66,38,247,176,101,86,148,128,173,24,86,140,190,37,206,220,50,62,119,29,45,152,130,180,82,182,29,80,192,147,125,215,158,41,29,2,88,158,170,110,94,72,242,13,81,140,216,
142,243,125,215,222,88,58,4,227,81,144,86,204,69,178,20,176,155,228,23,251,174,125,160,116,16,96,254,170,186,121,36,201,143,151,206,193,170,236,244,93,123,162,116,8,198,165,32,173,156,201,62,20,178,147,
228,175,247,93,251,72,233,32,192,252,84,117,115,95,146,95,138,51,70,108,89,223,181,86,41,87,64,65,34,85,221,188,156,228,84,233,28,172,146,97,14,192,190,85,117,243,112,146,159,136,98,68,25,167,251,174,
61,91,58,4,227,83,144,72,162,36,49,9,103,251,174,61,93,58,4,48,61,206,24,49,1,215,187,231,111,61,20,36,254,84,85,55,175,199,167,114,148,167,40,1,73,254,180,24,221,82,58,7,171,119,162,239,218,157,210,33,
216,30,5,137,55,168,234,198,23,4,83,161,40,193,74,13,147,86,191,57,62,180,163,188,119,245,93,251,108,233,16,108,215,117,165,3,48,57,215,39,57,31,219,24,40,239,150,161,176,159,77,242,141,62,189,131,229,
179,221,155,137,249,168,114,180,78,86,144,184,44,219,237,152,160,157,36,159,235,187,246,174,210,65,128,205,169,234,230,233,36,223,22,191,115,152,150,15,247,93,251,112,233,16,148,161,32,113,69,74,18,19,
102,251,29,204,156,193,11,76,216,3,125,215,62,90,58,4,229,40,72,92,149,51,73,76,220,75,125,215,190,163,116,8,96,255,12,94,96,226,62,224,142,62,20,36,174,170,170,155,123,146,124,170,116,14,184,134,115,
73,126,176,239,218,39,74,7,1,222,170,170,155,91,146,60,23,23,147,51,109,31,239,187,246,193,210,33,40,79,65,226,154,170,186,185,49,201,87,98,27,4,211,183,155,228,15,147,124,171,161,14,80,86,85,55,183,38,
121,42,201,59,99,187,54,211,247,100,223,181,103,74,135,96,26,20,36,246,205,153,36,102,102,39,201,23,157,85,130,237,170,234,230,169,36,223,21,191,47,152,15,3,25,120,3,5,137,3,113,38,137,153,114,86,9,70,
52,108,161,251,227,36,55,148,206,2,7,244,190,190,107,31,43,29,130,105,113,15,18,7,117,111,156,73,98,126,222,62,148,251,157,36,255,168,239,218,135,74,7,130,37,168,234,230,177,36,63,20,171,69,204,211,135,
149,35,46,199,10,18,7,54,236,43,127,174,116,14,56,130,221,236,157,171,251,201,190,107,63,82,58,12,204,73,85,55,247,39,249,39,113,161,43,243,246,104,223,181,15,148,14,193,52,41,72,28,154,51,73,44,200,133,
36,127,224,18,90,184,188,97,52,183,97,11,44,133,51,71,92,149,130,196,145,40,73,44,208,133,36,255,216,54,60,214,174,170,155,15,38,249,59,49,154,155,101,121,87,223,181,207,150,14,193,180,41,72,28,153,193,
13,44,212,110,146,47,37,249,206,190,107,159,47,29,6,182,161,170,155,51,73,126,35,123,103,148,93,237,192,210,156,238,187,246,108,233,16,76,159,130,196,70,84,117,179,19,191,76,89,174,221,36,127,146,189,
178,228,151,43,139,50,92,8,254,223,37,249,250,248,57,206,114,221,219,119,237,227,165,67,48,15,10,18,27,83,213,205,171,49,226,149,229,219,77,242,213,236,13,42,249,1,91,53,152,155,97,235,220,7,147,220,18,
219,231,88,7,229,136,3,81,144,216,168,170,110,94,142,201,70,172,203,110,146,215,146,252,102,223,181,247,150,14,3,151,51,92,222,250,29,81,136,88,151,157,190,107,79,148,14,193,252,40,72,108,156,149,36,86,
236,226,234,210,111,245,93,123,166,116,24,214,173,170,155,231,146,124,99,12,210,97,157,148,35,14,77,65,98,20,74,18,36,217,187,152,246,139,113,118,137,45,24,182,206,125,56,201,77,81,138,88,55,229,136,35,
81,144,24,205,112,111,198,45,165,115,192,132,236,36,249,66,146,143,184,160,150,163,168,234,230,100,146,46,201,127,24,195,21,224,82,202,17,71,166,32,49,170,170,110,62,145,228,254,210,57,96,162,46,110,201,
123,45,201,175,184,213,157,43,169,234,230,233,36,255,118,172,14,193,213,156,237,187,246,116,233,16,204,159,130,196,232,170,186,121,44,201,123,75,231,128,153,216,73,242,74,146,159,180,202,180,94,195,10,
252,219,178,55,84,65,33,130,107,251,108,223,181,119,151,14,193,50,40,72,108,69,85,55,143,36,249,241,210,57,96,134,118,146,92,72,242,235,125,215,222,87,58,12,155,87,213,205,173,73,126,55,123,133,232,88,
108,151,131,131,122,210,96,28,54,73,65,98,107,170,186,185,51,201,211,165,115,192,2,92,92,101,250,149,36,85,223,181,207,23,206,195,62,85,117,243,80,146,159,204,222,116,57,101,8,142,206,182,58,54,78,65,
98,235,170,186,121,45,238,226,128,49,92,60,211,244,82,146,115,73,126,47,201,131,38,232,109,79,85,55,103,178,55,60,225,207,100,239,231,220,59,99,139,28,140,97,55,201,143,244,93,251,88,233,32,44,143,130,
68,17,85,221,188,30,111,26,96,155,118,146,188,158,189,129,16,95,74,242,61,86,158,14,111,184,120,245,91,147,92,151,175,125,224,227,103,26,108,135,73,117,140,74,65,162,24,119,37,193,36,92,252,37,240,213,
236,149,168,47,39,249,241,190,107,31,45,23,169,188,170,110,238,73,242,15,179,55,57,238,250,36,23,223,140,41,65,80,150,114,196,232,20,36,138,170,234,230,229,36,167,74,231,0,46,107,247,50,127,253,122,246,
182,240,125,38,201,163,73,126,45,201,31,244,93,123,97,203,217,14,164,170,155,227,217,59,247,243,191,77,242,96,146,31,204,222,61,109,151,22,159,221,56,19,4,83,166,28,177,21,10,18,197,85,117,243,169,36,
247,148,206,1,108,220,155,127,193,236,230,107,219,252,46,245,213,236,77,234,187,146,139,91,216,174,187,228,239,93,151,175,173,230,156,200,94,177,185,88,112,46,253,107,96,25,12,99,96,107,20,36,38,97,216,
206,242,169,210,57,0,128,201,249,112,223,181,15,151,14,193,122,40,72,76,138,225,13,0,192,37,222,211,119,237,19,165,67,176,46,10,18,147,99,12,56,0,144,228,93,125,215,62,91,58,4,235,163,32,49,73,134,55,
0,192,106,25,198,64,81,182,50,49,73,125,215,222,156,228,201,210,57,0,128,173,122,70,57,162,52,5,137,201,234,187,246,76,146,247,148,206,1,0,108,197,39,251,174,189,171,116,8,176,197,142,89,48,188,1,0,22,
107,55,201,159,55,140,129,169,80,144,152,13,37,9,0,22,199,121,35,38,199,155,77,102,99,248,1,250,249,210,57,0,128,141,56,171,28,49,69,10,18,179,210,119,237,29,73,236,79,6,128,121,251,100,223,181,167,75,
135,128,203,177,197,142,217,178,229,14,0,102,103,55,201,117,125,215,238,148,14,2,87,162,32,49,107,74,18,0,204,134,243,70,204,130,55,150,204,218,240,131,246,108,233,28,0,192,85,157,83,142,152,11,5,137,
217,27,246,48,95,159,196,114,61,0,76,207,3,195,5,240,48,11,182,216,177,40,85,221,188,150,228,100,233,28,0,64,206,247,93,123,99,233,16,112,80,86,144,88,148,190,107,175,143,45,119,0,80,218,89,229,136,185,
82,144,88,156,97,203,221,135,75,231,0,128,149,122,222,8,111,230,204,22,59,22,173,170,155,87,147,220,80,58,7,0,172,192,57,103,141,88,2,43,72,44,218,176,188,255,100,233,28,0,176,112,159,85,142,88,10,5,137,
197,235,187,246,76,146,19,217,187,156,14,0,216,172,15,247,93,123,119,233,16,176,41,182,216,177,42,182,220,1,192,198,184,248,149,69,178,130,196,170,12,91,238,62,91,58,7,0,204,220,147,202,17,75,101,5,137,
213,170,234,230,229,36,167,74,231,0,128,25,177,106,196,226,89,65,98,181,134,195,164,86,147,0,96,127,62,171,28,177,6,10,18,171,54,28,42,125,95,233,28,0,48,113,239,50,136,129,181,176,197,14,6,6,56,0,192,
91,92,232,187,246,250,210,33,96,155,172,32,193,96,24,224,240,174,36,59,165,179,0,64,97,59,73,30,80,142,88,35,43,72,112,25,85,221,188,152,228,237,165,115,0,64,1,103,251,174,61,93,58,4,148,98,5,9,46,163,
239,218,119,36,249,64,233,28,0,176,101,31,85,142,88,59,43,72,112,13,206,38,1,176,2,206,26,193,192,10,18,92,195,112,54,233,195,73,124,154,0,192,210,236,36,249,144,114,4,95,99,5,9,14,160,170,155,167,147,
220,89,58,7,0,108,128,179,70,112,25,10,18,28,80,85,55,39,147,188,26,43,176,0,204,211,78,146,63,219,119,237,179,165,131,192,20,41,72,112,72,85,221,188,156,228,84,233,28,0,112,0,207,247,93,123,91,233,16,
48,101,62,1,135,67,234,187,246,230,236,157,77,114,111,18,0,83,183,147,189,9,117,202,17,92,131,21,36,216,128,170,110,30,78,242,95,38,57,86,56,10,0,92,106,39,201,207,245,93,251,96,233,32,48,23,10,18,108,
144,109,119,0,76,136,33,12,112,8,182,216,193,6,13,219,238,238,141,145,224,0,148,179,155,228,125,202,17,28,142,21,36,24,137,213,36,0,10,176,106,4,71,164,32,193,136,170,186,185,37,201,179,81,148,0,24,215,
185,97,23,3,112,68,10,18,108,65,85,55,183,38,249,124,146,147,165,179,0,176,40,59,73,190,177,239,218,179,165,131,192,82,40,72,176,69,85,221,60,155,228,246,210,57,0,88,132,199,251,174,189,183,116,8,88,26,
5,9,10,168,234,230,213,36,55,148,206,1,192,44,157,239,187,246,198,210,33,96,169,76,177,131,2,134,95,108,247,198,37,179,0,236,223,78,246,166,211,41,71,48,34,43,72,80,88,85,55,15,37,249,233,248,192,2,128,
203,219,73,242,103,251,174,125,182,116,16,88,3,5,9,38,162,170,155,143,37,249,143,163,40,1,176,103,55,201,47,246,93,251,64,233,32,176,38,10,18,76,140,251,147,0,72,242,249,190,107,239,40,29,2,214,200,39,
213,48,49,195,61,22,167,147,92,40,157,5,128,173,219,73,114,90,57,130,114,172,32,193,132,85,117,115,42,201,159,196,196,59,128,165,123,41,201,191,221,119,237,185,210,65,96,237,20,36,152,129,170,110,110,
73,242,92,92,52,11,176,52,23,250,174,189,190,116,8,224,107,20,36,152,145,170,110,238,73,242,107,177,61,22,96,238,118,146,252,133,190,107,159,40,29,4,120,35,5,9,102,168,170,155,231,146,220,90,58,7,0,7,
102,50,29,76,156,130,4,51,86,213,205,19,73,190,39,86,148,0,166,110,39,201,239,244,93,123,119,233,32,192,213,41,72,176,0,85,221,60,146,228,175,69,81,2,152,154,11,73,254,98,223,181,143,151,14,2,236,143,
130,4,11,82,213,205,195,73,126,34,138,18,64,105,59,73,126,174,239,218,7,75,7,1,14,70,65,130,5,170,234,230,169,36,223,157,228,88,233,44,0,43,179,155,228,103,21,35,152,47,5,9,22,172,170,155,23,146,124,67,
20,37,128,177,237,38,249,55,125,215,222,86,58,8,112,52,10,18,172,64,85,55,247,37,249,165,216,122,7,176,105,231,146,252,160,113,221,176,28,10,18,172,72,85,55,167,146,124,33,201,169,210,89,0,102,238,124,
223,181,55,150,14,1,108,158,130,4,43,85,213,205,203,81,148,0,14,234,92,223,181,55,151,14,1,140,71,65,130,21,171,234,230,198,36,47,38,185,161,112,20,128,169,83,140,96,37,20,36,32,73,82,213,205,211,73,190,
61,6,58,0,92,180,147,228,51,125,215,158,41,29,4,216,30,5,9,120,131,170,110,206,36,121,60,86,149,128,245,186,144,228,246,190,107,159,47,29,4,216,62,5,9,184,34,231,148,128,149,57,219,119,237,233,210,33,
128,178,20,36,224,154,170,186,121,49,201,219,75,231,0,24,137,243,69,192,159,82,144,128,125,171,234,230,185,36,223,20,231,148,128,249,219,73,242,185,190,107,239,42,29,4,152,22,5,9,56,148,170,110,158,74,
242,93,113,249,44,48,31,187,73,126,187,239,218,187,75,7,1,166,75,65,2,142,164,170,155,135,146,180,73,78,150,206,2,112,5,231,147,124,71,223,181,207,150,14,2,76,159,130,4,108,140,179,74,192,132,236,36,249,
114,223,181,239,40,29,4,152,23,5,9,216,184,170,110,94,72,242,206,216,126,7,108,223,78,146,95,237,187,246,190,210,65,128,121,82,144,128,81,85,117,243,68,146,239,137,178,4,140,231,66,146,79,247,93,123,111,
233,32,192,252,41,72,192,214,24,236,0,108,144,41,116,192,40,20,36,96,235,170,186,57,147,228,55,98,176,3,112,112,231,146,124,123,223,181,207,151,14,2,44,147,130,4,20,85,213,205,211,73,190,45,86,149,128,
43,219,73,242,133,190,107,111,43,29,4,88,62,5,9,152,132,170,110,110,76,242,71,49,220,1,216,179,147,228,143,250,174,189,163,116,16,96,93,20,36,96,146,170,186,121,56,201,127,17,219,240,96,77,206,37,249,
203,125,215,62,86,58,8,176,94,10,18,48,121,67,89,250,91,73,174,79,114,172,108,26,96,195,206,39,249,187,125,215,62,92,58,8,64,162,32,1,51,83,213,205,99,73,126,48,86,150,96,206,140,229,6,38,75,65,2,102,
171,170,155,231,146,220,18,101,9,230,224,66,146,223,235,187,246,238,210,65,0,174,70,65,2,22,161,170,155,83,73,254,73,146,191,20,133,9,166,224,66,146,223,75,242,94,35,185,129,57,81,144,128,69,170,234,230,
214,36,191,155,228,109,49,21,15,182,97,39,201,151,147,252,71,134,44,0,115,166,32,1,171,80,213,205,11,49,66,28,54,237,98,41,122,103,223,181,59,165,195,0,108,130,130,4,172,206,112,118,233,157,49,21,15,14,
106,55,201,87,147,252,86,223,181,103,74,135,1,24,131,130,4,172,90,85,55,183,36,249,23,73,190,53,201,77,177,194,4,151,186,184,66,244,76,146,31,233,187,246,108,225,60,0,163,83,144,0,222,164,170,155,59,147,
252,15,217,91,101,58,22,171,76,172,199,78,146,63,74,242,3,125,215,62,91,56,11,64,17,10,18,192,53,84,117,115,99,246,222,52,58,195,196,210,92,92,33,250,225,190,107,159,40,29,6,96,10,20,36,128,3,170,234,
230,254,36,255,56,123,19,242,172,48,49,39,59,73,190,152,228,251,250,174,125,166,116,24,128,41,82,144,0,142,104,24,41,254,84,246,10,211,201,88,101,98,26,118,146,188,146,189,161,10,63,213,119,237,71,10,
231,1,152,5,5,9,96,36,85,221,220,151,228,111,39,249,115,49,0,130,241,236,38,121,45,201,111,38,249,137,190,107,31,47,156,7,96,214,20,36,128,45,171,234,230,225,36,31,202,94,105,178,69,143,253,218,29,254,
245,122,146,79,39,249,139,125,215,94,40,27,9,96,121,20,36,128,9,168,234,230,99,73,126,44,123,119,51,157,136,213,166,181,187,120,233,234,43,73,126,165,239,218,7,74,134,1,88,19,5,9,96,162,134,59,154,158,
74,242,245,249,90,113,178,226,180,44,59,217,91,17,122,45,123,103,133,254,155,190,107,31,46,154,8,96,229,20,36,128,25,26,206,55,221,159,228,59,146,220,158,228,150,56,231,52,53,23,183,196,189,50,252,235,
249,236,21,222,95,72,242,207,250,174,221,185,202,255,47,0,133,40,72,0,11,86,213,205,61,73,254,70,146,31,204,222,148,189,228,107,43,80,86,162,14,103,247,77,255,254,74,146,127,149,228,255,158,228,255,209,
119,237,171,69,82,1,176,17,10,18,192,202,85,117,115,71,146,255,58,201,247,103,175,68,93,92,133,58,49,252,251,26,182,245,93,92,237,185,232,245,236,109,127,251,106,146,207,36,249,155,46,82,5,88,7,5,9,128,
3,25,86,165,254,65,146,119,101,111,91,223,69,215,103,175,84,188,185,96,37,87,46,88,135,41,94,187,87,248,235,55,187,180,228,92,244,213,36,47,38,249,207,98,155,27,0,151,161,32,1,0,0,12,28,230,5,0,0,24,40,
72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,
10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,
129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,64,65,2,0,0,24,40,72,0,0,0,3,5,9,0,0,96,160,32,1,0,0,12,20,36,0,0,128,129,130,4,0,0,48,80,144,0,0,0,6,10,18,0,0,192,224,255,15,131,107,116,19,230,127,174,
90,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* EditorComponent::clockwise_arrow_png = (const char*) resource_PluginEditor_clockwise_arrow_png;
const int EditorComponent::clockwise_arrow_pngSize = 22647;

// JUCER_RESOURCE: load_icon_png, 3825, "../GUI/load_icon.png"
static const unsigned char resource_PluginEditor_load_icon_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,3,152,0,0,2,0,4,3,0,0,0,21,164,222,20,0,0,0,18,80,76,84,69,230,230,230,255,255,255,
0,0,0,142,142,142,76,76,76,197,197,197,47,1,32,45,0,0,14,154,73,68,65,84,120,218,237,221,79,147,219,54,18,5,112,82,181,123,7,40,233,46,82,156,187,201,201,222,67,101,125,143,228,205,247,255,42,59,254,51,
49,186,225,221,42,105,40,128,253,250,177,124,121,73,202,158,241,111,196,144,232,70,163,137,223,175,208,124,191,24,45,71,254,77,16,147,145,152,140,196,100,36,38,49,25,137,201,72,76,70,98,50,18,147,152,
140,196,100,36,38,35,49,25,223,227,219,213,254,248,71,140,134,35,49,137,201,72,76,70,98,50,18,147,152,140,196,100,36,38,35,49,25,137,73,76,70,98,50,18,147,145,152,140,223,48,89,12,100,113,154,145,152,
140,196,100,36,38,49,25,137,201,72,76,70,98,50,18,147,152,107,199,248,161,139,63,128,27,194,140,225,183,203,240,248,245,199,167,72,189,205,96,254,99,248,224,245,59,215,147,183,130,57,15,31,190,62,7,234,
109,2,115,5,203,97,88,168,247,127,98,169,178,219,58,150,195,208,178,128,89,189,56,221,118,235,88,14,231,64,189,218,152,97,88,235,90,168,87,25,179,189,173,134,57,6,234,213,197,60,12,195,186,31,77,234,85,
195,108,231,21,49,191,125,52,169,87,13,243,56,172,121,77,212,171,137,185,91,21,243,20,168,87,17,115,88,247,34,102,69,204,110,101,204,133,122,245,48,111,43,99,158,3,245,170,97,14,107,95,87,234,213,194,
220,175,142,217,7,234,85,194,236,86,199,28,136,89,11,115,94,31,115,162,158,198,44,83,103,107,215,183,124,187,207,178,128,89,167,56,45,23,227,94,31,188,230,124,181,157,152,197,49,197,243,207,244,120,111,
222,44,159,103,137,89,3,51,125,254,89,194,227,191,149,248,161,232,169,87,27,243,71,155,192,99,191,149,40,189,188,223,103,137,89,22,115,214,93,2,143,254,86,135,95,220,103,137,89,13,243,99,31,167,246,23,
247,89,98,150,197,188,232,187,236,195,152,187,97,165,59,54,49,63,142,249,209,167,150,125,222,115,73,76,171,152,237,37,187,207,18,179,44,102,246,252,243,56,102,151,221,103,137,105,21,179,17,189,68,145,
152,53,49,167,15,47,243,94,116,203,37,49,205,98,54,217,125,150,152,118,49,179,231,89,98,166,241,249,101,183,124,149,253,3,191,179,120,158,101,61,179,120,113,122,85,204,244,62,59,18,211,54,166,184,207,
94,137,105,26,83,116,250,245,196,180,141,41,118,58,16,211,54,166,44,81,19,211,52,166,108,15,107,227,70,46,98,62,132,185,27,182,119,141,159,127,172,80,17,243,190,120,24,182,120,141,215,72,204,251,99,59,
108,243,250,61,16,51,86,156,117,177,242,70,195,64,204,187,227,126,216,172,38,49,239,158,15,181,85,204,225,74,204,106,195,190,86,191,206,145,152,247,198,227,102,63,154,75,69,76,123,245,204,239,113,222,
172,102,100,113,250,222,184,217,251,108,205,214,108,171,152,113,179,152,99,32,230,189,113,187,31,205,137,152,119,199,205,98,190,4,98,222,27,183,251,209,228,39,243,254,184,221,183,19,98,222,29,15,91,197,
60,5,98,222,27,183,251,174,73,204,251,99,184,108,117,129,150,152,119,199,118,171,154,61,49,239,143,91,213,252,250,63,77,98,222,27,219,24,191,188,190,190,126,249,246,235,239,171,66,252,237,162,230,167,
18,243,145,184,149,222,188,155,122,2,34,166,225,40,52,3,119,129,217,142,71,57,81,142,197,105,203,49,237,50,91,136,105,60,30,106,111,103,34,230,122,49,16,19,39,182,196,4,194,156,137,137,136,121,34,166,
117,204,159,143,179,47,196,180,30,119,196,36,38,49,137,73,76,98,18,147,152,222,158,102,137,201,79,102,116,191,11,108,139,177,75,155,218,89,156,38,38,49,55,18,119,196,228,39,147,152,196,36,38,111,179,196,
228,39,147,152,196,228,109,150,152,252,100,18,147,152,196,228,109,150,159,76,98,18,147,152,254,110,179,172,103,178,211,128,152,196,36,38,49,109,68,246,0,241,147,73,76,98,18,147,152,197,48,223,131,213,
107,199,45,125,239,177,141,255,124,253,183,233,235,231,96,167,241,143,191,218,232,24,51,238,183,58,214,240,225,19,78,130,87,204,13,159,109,241,240,245,41,248,196,68,180,44,126,206,219,86,48,33,45,223,
190,219,224,16,115,55,128,94,147,63,204,61,170,229,48,198,146,152,91,168,103,198,11,44,230,208,7,95,197,233,182,27,128,175,214,23,102,68,182,252,113,254,144,27,76,232,15,230,251,17,140,78,48,219,11,54,
102,239,9,115,143,109,249,227,8,70,31,152,237,13,28,243,251,183,236,4,19,221,178,224,209,96,213,49,247,240,152,163,31,204,14,30,179,220,209,96,181,49,209,159,101,223,159,103,125,96,226,91,126,91,55,112,
129,121,112,128,57,120,193,236,136,9,131,41,43,153,159,34,202,21,102,245,166,89,230,239,185,114,61,51,253,166,167,8,212,70,155,126,99,139,147,226,180,124,78,0,194,76,107,65,189,63,204,43,84,131,123,90,
165,125,241,129,153,156,83,120,14,80,152,233,209,96,103,31,152,123,249,106,141,132,153,44,135,248,195,156,112,183,107,142,62,48,15,226,101,12,10,51,89,117,38,166,249,152,124,111,62,48,59,98,34,98,142,
192,183,89,31,152,73,207,8,49,205,99,206,226,53,147,15,64,166,49,127,190,139,157,26,52,204,155,179,247,204,86,181,151,66,189,103,206,206,150,243,160,49,47,197,23,218,43,215,51,69,153,168,68,185,175,96,
20,63,168,14,138,211,71,245,145,37,166,97,204,100,105,246,138,134,153,84,77,22,23,152,226,85,12,12,115,95,190,109,132,152,207,138,7,81,118,39,166,233,216,121,235,206,19,239,213,196,180,141,41,222,171,
97,49,71,31,152,226,189,26,12,243,230,13,115,0,198,156,189,237,53,65,198,148,53,4,124,204,160,86,243,160,48,189,109,233,211,171,121,196,52,140,153,188,87,183,104,152,114,53,207,1,166,94,51,64,194,84,29,
193,197,254,220,106,245,76,213,155,7,85,207,84,119,29,252,226,244,78,77,62,66,194,84,119,29,120,76,221,155,71,76,203,152,179,158,253,8,132,41,118,154,120,192,84,189,121,80,152,179,172,33,224,99,234,209,
143,72,152,23,89,67,32,166,229,168,86,42,225,49,143,242,189,154,152,150,49,247,217,90,2,49,205,98,38,239,213,13,26,166,94,118,134,199,204,22,128,128,48,117,19,41,49,13,199,131,90,118,134,199,204,86,243,
128,48,59,103,152,122,115,38,49,45,99,94,244,106,30,16,166,90,205,195,223,5,166,123,243,128,234,153,217,93,7,190,56,61,0,99,234,187,14,49,13,99,122,59,113,232,168,123,243,136,105,23,115,159,63,22,225,
244,0,233,101,103,116,204,116,53,15,13,243,232,13,51,93,0,66,195,148,155,51,137,137,130,217,184,192,76,87,243,208,48,179,101,103,116,204,100,167,45,49,173,99,166,239,213,96,152,121,19,41,58,102,250,42,
134,134,153,213,16,136,105,23,51,185,235,52,206,48,23,56,204,172,239,16,28,243,72,204,103,196,58,245,76,209,37,99,191,128,41,99,182,236,12,94,156,46,62,143,190,96,204,182,132,163,99,118,192,152,7,213,
155,71,76,195,49,219,18,14,142,153,190,87,19,211,58,230,92,122,30,61,49,159,135,153,108,206,132,195,212,3,30,225,49,139,31,252,90,48,234,1,143,196,52,28,243,38,82,240,167,217,226,231,107,23,140,131,51,
76,217,206,69,76,211,152,242,189,26,11,51,228,77,164,216,152,242,233,29,11,115,255,63,143,152,36,166,185,152,175,230,17,211,108,204,215,12,192,119,129,85,24,97,94,44,230,91,194,193,139,211,178,75,6,11,
51,223,18,14,142,89,97,132,121,169,216,222,178,45,225,224,152,21,166,94,23,195,204,183,132,19,19,0,179,119,129,89,99,132,121,49,204,193,25,102,173,17,230,196,124,2,166,218,243,6,133,153,54,145,186,192,
84,175,98,80,152,233,93,135,152,198,99,122,112,130,51,204,17,14,51,93,205,243,128,169,247,188,17,211,50,166,58,56,1,10,51,29,181,230,2,83,29,156,128,132,41,154,72,93,96,170,109,82,80,152,233,193,9,197,
191,140,26,245,204,10,35,204,139,197,244,7,181,228,159,91,11,83,119,201,16,211,48,230,190,194,60,122,98,62,9,83,31,131,138,132,41,182,132,123,192,172,49,245,186,84,20,53,4,98,218,142,98,212,154,47,204,
17,14,83,44,59,123,192,212,123,222,136,105,24,83,29,72,8,133,41,122,243,124,97,246,112,152,162,137,212,3,230,0,140,41,122,243,136,105,59,138,101,103,7,152,217,208,35,98,218,197,212,7,18,130,98,46,46,48,
245,129,132,72,152,199,122,152,117,234,153,117,166,94,151,137,114,115,166,131,226,180,19,204,232,2,115,87,101,132,121,153,216,57,195,172,52,194,156,152,79,193,204,246,188,225,96,70,49,224,209,3,102,189,
169,215,207,199,20,3,30,137,105,27,83,12,120,244,240,0,164,207,176,67,194,148,43,149,248,152,71,98,226,96,238,245,129,132,72,79,179,178,137,20,31,51,59,144,16,8,83,53,145,226,99,230,67,143,112,48,213,
168,53,98,90,142,170,239,16,30,51,63,144,144,152,118,49,179,213,60,98,174,22,139,215,51,229,123,53,86,61,83,221,117,240,139,211,117,166,94,23,137,250,174,67,76,203,152,106,75,184,39,204,5,14,83,45,59,
195,99,30,235,140,48,39,230,51,48,43,141,48,47,18,245,178,179,39,204,136,134,185,215,213,62,116,204,14,24,83,13,120,36,166,229,168,87,42,225,49,111,106,167,45,49,13,99,206,217,233,146,56,152,186,137,20,
30,51,121,175,110,193,48,179,26,2,60,102,242,42,6,135,169,155,72,137,9,129,121,106,60,96,166,93,50,112,152,186,137,180,40,102,133,122,102,181,17,230,37,162,174,33,160,23,167,171,141,48,47,16,179,99,80,
209,49,171,77,189,46,16,179,99,80,137,105,55,234,1,143,240,152,213,70,152,23,136,217,74,37,56,102,122,32,33,49,173,99,206,53,71,152,23,195,28,125,96,166,163,214,208,48,245,128,71,120,204,1,24,83,15,120,
36,38,4,102,239,2,51,237,146,129,195,28,156,97,138,247,106,98,218,198,20,239,213,96,152,121,19,41,56,166,120,21,3,195,204,6,60,18,211,110,204,155,72,139,127,25,101,235,153,93,173,17,230,5,98,190,139,24,
188,56,125,171,53,194,156,152,235,99,86,27,97,254,252,248,139,45,225,110,48,251,6,13,51,223,18,14,142,57,0,99,38,237,92,196,180,142,153,205,4,4,199,172,55,194,156,152,171,99,202,61,111,96,79,179,217,76,
64,112,76,57,106,13,11,243,152,29,148,14,142,41,95,197,176,48,15,217,128,71,98,34,96,70,23,152,114,207,27,22,102,231,13,83,190,87,19,211,52,166,220,243,6,133,153,174,230,249,192,148,175,98,88,152,233,
150,240,26,152,197,235,153,21,71,152,63,61,166,155,51,107,124,25,21,49,23,56,204,244,174,227,1,243,95,21,71,152,19,115,101,204,215,138,35,204,11,98,46,46,48,255,83,121,80,242,51,163,88,205,243,128,121,
3,198,20,171,121,196,132,193,12,46,48,103,185,231,13,10,179,243,139,121,38,166,117,76,117,112,2,20,166,232,205,243,128,169,118,214,64,97,138,26,2,49,109,71,49,106,205,23,230,2,135,41,106,8,190,48,39,98,
226,96,94,209,48,131,232,205,171,245,101,148,172,103,86,156,122,253,236,40,71,173,121,40,78,3,99,202,81,107,196,52,29,229,116,46,87,152,35,49,113,48,207,112,152,59,177,236,236,10,243,212,128,97,182,114,
212,154,43,204,30,14,115,22,163,214,136,105,26,83,142,90,115,133,185,192,97,14,126,49,39,98,226,96,94,209,48,143,178,134,224,10,51,160,97,170,81,107,196,180,28,213,193,9,158,48,71,56,76,53,107,159,152,
196,252,16,102,173,122,102,141,17,230,207,141,157,252,65,245,84,156,238,209,48,219,155,92,118,38,166,101,76,117,12,42,49,45,99,170,149,74,79,152,11,49,113,48,39,98,226,96,182,104,152,71,85,67,240,132,
25,209,48,247,170,134,64,76,195,81,14,120,116,133,57,194,97,234,89,251,142,48,207,196,196,193,124,65,195,204,14,78,112,132,217,195,97,234,131,19,136,105,24,83,173,230,17,211,50,166,238,59,172,245,85,85,
168,103,214,152,71,255,220,168,127,80,29,21,167,175,192,152,139,55,204,136,134,153,29,131,74,76,187,49,59,6,149,152,118,99,118,12,170,31,204,17,14,179,243,139,121,38,38,14,230,9,14,115,167,107,8,126,48,
123,56,204,89,215,16,136,105,55,170,1,143,158,48,23,56,204,108,165,210,15,230,68,76,28,204,43,48,230,226,13,51,162,97,102,171,121,196,180,27,15,122,53,207,209,46,176,0,114,42,205,223,81,119,0,57,42,78,
143,196,196,193,60,19,19,7,243,165,1,195,204,122,243,28,97,246,112,152,186,55,143,152,134,49,47,106,192,163,35,204,5,14,83,247,230,57,194,156,136,137,131,121,69,195,60,230,77,164,110,48,3,26,166,222,156,
73,76,195,49,29,181,70,76,227,49,95,51,112,131,57,18,19,7,243,12,135,185,203,22,128,220,96,158,26,48,76,61,106,173,230,87,85,186,158,217,71,180,122,230,156,109,9,119,83,156,198,195,76,86,243,90,103,152,
11,28,102,242,131,234,13,115,34,38,14,102,139,134,153,30,131,234,13,51,162,97,166,171,121,196,52,30,211,81,107,206,48,71,56,204,116,1,200,25,230,153,152,56,152,47,192,152,163,55,204,30,14,51,93,205,35,
166,241,152,142,90,35,38,12,102,239,237,105,118,129,195,28,252,98,78,196,196,217,5,246,39,64,53,90,68,177,57,211,89,113,58,160,97,138,81,107,196,180,29,197,168,53,95,152,35,28,102,231,23,243,76,76,28,
204,23,52,204,116,115,166,55,204,83,131,134,41,70,173,249,194,236,225,48,197,168,53,98,218,198,20,43,149,190,48,23,98,226,96,78,104,152,141,88,118,246,133,217,160,97,202,81,107,190,48,3,26,166,60,56,129,
152,166,163,60,56,193,21,230,8,135,217,109,6,179,120,61,179,222,8,243,103,69,185,211,214,85,113,250,4,135,41,119,218,186,194,236,225,48,229,168,53,98,154,142,114,212,154,43,204,5,14,83,174,84,186,194,
156,136,137,131,121,5,198,92,188,97,70,52,204,163,92,118,38,166,229,184,151,135,47,19,211,114,60,200,149,74,79,152,35,28,102,231,23,243,76,76,28,204,19,26,166,62,56,193,19,102,15,135,121,145,29,193,196,
180,140,169,154,72,107,98,150,174,103,214,156,71,255,156,168,126,80,61,21,167,137,73,76,98,18,179,64,12,170,32,132,143,121,81,219,134,128,48,247,170,32,132,143,57,111,226,68,194,167,196,78,21,132,92,97,
78,96,152,23,57,122,213,1,230,78,111,207,132,193,220,235,26,2,62,102,151,182,26,4,36,204,139,174,33,224,99,30,212,32,232,38,188,255,219,208,54,95,127,89,141,55,61,224,209,1,230,81,140,27,121,253,179,121,
251,171,104,191,254,122,255,143,77,198,240,229,146,141,145,115,128,217,14,14,46,55,152,23,7,152,147,23,204,157,3,204,232,4,83,62,1,97,94,47,219,120,198,46,81,118,115,240,191,204,234,85,214,98,152,51,60,
230,213,15,102,7,143,25,252,96,6,248,187,172,35,76,248,251,108,219,56,194,4,127,158,125,137,158,48,35,246,186,193,228,10,179,129,126,4,26,163,47,76,232,143,230,228,12,51,221,251,134,118,157,162,55,204,
246,6,189,44,235,11,19,247,70,59,57,196,108,2,166,230,18,61,98,182,71,92,75,119,152,77,27,224,22,130,198,105,51,123,211,138,151,221,226,30,235,86,251,121,75,61,131,197,255,204,183,159,160,227,95,32,87,
245,22,246,218,152,95,239,8,63,219,22,227,183,166,197,182,53,28,157,99,50,18,147,145,152,196,100,36,38,35,49,25,137,201,72,76,98,50,18,147,145,152,140,196,100,172,93,133,99,52,90,156,102,36,38,35,49,137,
201,72,76,70,98,50,18,147,145,152,196,100,36,38,35,49,25,137,201,72,76,98,50,110,21,147,197,64,22,167,25,55,24,255,11,211,41,245,221,150,37,75,200,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* EditorComponent::load_icon_png = (const char*) resource_PluginEditor_load_icon_png;
const int EditorComponent::load_icon_pngSize = 3825;

// JUCER_RESOURCE: save_icon_png, 2362, "../GUI/save_icon.png"
static const unsigned char resource_PluginEditor_save_icon_png[] = { 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,200,0,0,0,200,8,6,0,0,0,173,88,174,158,0,0,0,4,115,66,73,84,8,8,8,8,124,8,100,136,
0,0,0,9,112,72,89,115,0,0,11,19,0,0,11,19,1,0,154,156,24,0,0,8,220,73,68,65,84,120,156,237,221,91,172,92,85,29,199,241,111,105,75,91,160,156,82,141,130,87,164,88,34,197,75,172,137,183,16,83,12,18,106,
160,68,140,62,232,67,69,84,66,140,62,163,86,225,69,145,39,77,212,23,132,68,94,8,9,149,120,9,13,47,24,35,168,245,26,67,91,165,6,58,90,37,26,161,86,108,233,221,227,195,110,211,147,147,89,255,115,102,102,
205,94,123,102,125,63,201,122,217,115,186,214,127,186,247,111,102,237,125,206,94,27,36,73,146,36,73,146,36,73,19,108,201,152,251,95,5,108,6,182,0,111,7,94,5,92,52,230,49,53,25,142,3,59,128,219,128,127,
20,174,165,117,231,0,159,2,254,14,204,218,108,65,123,154,230,131,179,26,51,52,159,12,165,255,227,109,147,211,58,27,146,220,83,172,25,224,9,224,170,204,253,106,250,237,5,54,1,207,149,46,100,174,115,50,
247,245,32,134,67,195,89,15,252,132,142,125,147,228,12,200,39,129,235,51,246,167,250,116,50,36,57,172,34,62,33,127,146,230,74,214,218,69,246,151,234,167,22,61,250,191,255,75,51,143,51,232,255,115,174,
186,190,22,140,221,233,115,146,97,221,76,250,205,126,139,193,191,169,12,200,116,7,4,42,11,201,3,164,191,57,134,153,198,25,144,233,15,8,84,20,146,93,244,127,131,55,12,217,159,1,169,35,32,80,73,72,14,208,
255,205,13,251,91,115,3,82,79,64,160,130,144,228,62,160,13,72,93,1,129,41,15,137,1,201,171,71,125,1,129,41,14,137,1,201,171,71,157,1,129,41,13,137,1,201,171,71,189,1,129,41,12,137,1,201,171,71,221,1,129,
41,11,137,1,201,171,135,1,129,41,10,137,1,201,171,135,1,57,99,42,66,98,64,242,234,97,64,230,154,248,144,24,144,188,122,24,144,249,38,58,36,6,36,175,30,6,164,159,137,13,137,1,201,171,135,1,73,153,200,144,
24,144,188,122,24,144,200,98,66,114,113,203,53,133,12,72,94,61,12,200,66,22,10,201,247,11,212,148,100,64,242,234,97,64,22,35,10,201,145,28,3,228,188,39,93,106,219,29,192,221,137,215,86,230,24,192,128,
104,210,221,53,206,206,13,136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,88,86,186,0,13,100,95,233,2,106,227,55,136,20,48,32,
82,192,128,116,211,127,10,142,125,176,224,216,157,99,64,186,233,129,130,99,127,175,224,216,83,203,69,27,242,90,2,124,25,248,39,241,202,29,57,219,191,129,111,0,231,6,117,245,18,255,246,210,209,223,242,
208,86,38,106,234,212,241,98,64,234,208,163,178,128,56,197,146,2,93,13,136,223,22,221,180,36,177,253,127,173,86,209,162,174,6,228,165,196,246,97,31,43,173,60,214,38,182,31,110,181,138,22,117,53,32,127,
75,108,127,87,171,85,104,174,171,128,11,250,108,63,12,28,104,185,150,214,116,53,32,191,79,108,191,165,213,42,52,215,39,18,219,255,128,83,226,5,245,187,130,48,202,188,116,107,208,231,251,71,41,84,67,217,
0,28,165,255,62,217,86,176,46,152,224,203,188,163,4,228,66,224,80,162,223,23,128,141,163,20,171,129,172,163,249,35,201,126,251,226,20,112,89,185,210,128,74,3,2,112,79,162,223,89,154,149,187,191,4,204,
140,56,134,210,86,1,159,165,249,5,98,106,63,60,88,172,186,179,198,26,144,212,101,187,65,245,43,102,150,209,206,113,102,128,63,2,151,4,63,115,20,248,37,240,44,112,98,132,177,116,214,82,224,117,192,187,
129,213,193,207,29,2,174,4,246,183,81,84,96,37,233,71,29,228,58,190,71,54,142,111,16,128,107,128,147,137,254,109,101,219,199,130,253,214,166,106,167,88,103,108,165,153,235,150,62,32,108,103,219,23,162,
29,214,178,234,3,2,240,33,224,197,196,56,182,246,218,49,224,51,11,236,171,182,25,144,211,214,1,59,18,99,217,198,223,126,14,188,109,193,189,212,62,3,50,207,123,104,238,151,56,152,24,215,150,175,29,2,30,
6,174,91,212,158,41,163,218,171,88,11,57,135,230,42,202,122,96,13,241,125,12,90,188,147,52,119,52,62,3,60,69,247,175,14,86,125,21,75,90,136,247,131,72,165,24,16,41,96,64,164,128,1,145,2,6,68,10,24,16,
41,96,64,164,128,1,145,2,6,68,10,24,16,41,96,64,164,128,1,145,2,6,68,10,24,16,41,96,64,164,128,1,145,2,6,68,10,24,16,41,96,64,164,128,1,145,2,6,68,10,24,16,41,176,172,116,1,153,157,15,92,77,243,252,144,
215,2,231,225,135,64,155,78,208,60,46,225,105,224,9,96,119,217,114,186,163,244,186,88,87,0,247,147,126,232,142,173,76,219,3,220,14,172,72,239,186,145,185,244,104,224,92,224,235,52,159,92,165,15,6,91,186,
237,5,222,155,216,135,163,50,32,9,107,129,39,19,99,219,186,215,78,48,158,149,225,13,72,31,171,129,223,38,198,181,117,187,229,14,137,1,233,227,225,196,152,182,238,183,19,228,157,110,185,186,251,60,31,1,
30,10,94,63,2,252,0,248,5,83,252,128,251,142,90,78,243,212,219,27,129,183,4,63,183,23,120,51,112,60,195,152,174,238,62,199,82,224,207,137,241,102,129,237,192,43,199,48,174,6,119,19,240,47,210,251,234,
211,153,198,153,216,41,214,169,49,140,179,57,49,214,44,112,47,29,250,196,16,208,92,126,79,133,228,169,76,99,24,144,57,238,75,140,181,27,31,160,211,85,55,147,62,128,215,103,232,223,128,204,177,55,49,214,
199,199,48,150,242,217,69,255,253,118,75,134,190,125,128,206,105,203,104,78,0,251,249,113,155,133,104,96,169,253,115,69,171,85,12,97,146,2,50,67,115,146,62,223,11,52,15,244,84,119,61,147,216,126,81,171,
85,12,97,146,2,178,60,177,253,100,171,85,104,24,169,7,129,118,254,188,113,146,2,34,181,206,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,129,105,91,23,107,177,214,
0,183,2,235,240,30,146,65,28,3,30,3,30,45,93,72,91,106,12,200,90,96,39,112,121,233,66,38,212,231,128,187,128,59,11,215,209,138,26,167,88,183,99,56,70,245,69,224,101,165,139,104,67,141,1,49,28,163,91,6,
188,190,116,17,109,168,49,32,191,42,93,192,20,120,17,248,83,233,34,218,80,99,64,190,75,115,162,169,225,28,163,89,145,228,165,210,133,180,161,198,147,244,227,192,245,192,38,154,233,86,141,31,18,195,58,
2,60,14,236,47,93,72,91,106,12,8,52,55,244,63,126,186,73,73,126,122,74,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,
136,20,48,32,82,160,214,27,166,0,174,166,121,40,168,235,98,45,222,49,224,167,192,115,165,11,105,75,141,1,89,14,60,2,124,176,116,33,19,234,40,205,99,183,183,151,46,164,13,227,156,98,117,234,65,238,115,
220,138,225,24,197,74,224,62,96,85,233,66,218,80,227,57,200,59,75,23,48,5,102,128,55,149,46,162,13,53,6,100,95,233,2,166,192,41,224,175,165,139,104,67,141,1,249,54,208,43,93,196,132,187,27,120,190,116,
17,109,168,241,36,253,121,96,35,112,27,240,70,188,138,53,136,35,192,14,224,135,165,11,105,75,141,1,1,56,0,124,181,116,17,234,190,26,167,88,210,162,25,16,41,96,64,164,128,1,145,2,6,68,10,24,16,41,96,64,
164,128,1,145,2,6,68,10,24,16,41,96,64,164,128,1,145,2,6,68,10,24,16,41,96,64,164,128,1,145,2,181,222,48,5,112,30,112,9,222,81,56,136,99,192,254,210,69,180,169,214,128,220,1,108,163,146,165,107,50,219,
3,124,20,216,85,186,144,54,212,56,197,186,129,230,118,91,195,49,156,43,105,22,222,171,226,216,169,226,77,206,115,99,233,2,166,192,229,192,134,210,69,180,161,198,128,28,42,93,192,148,248,111,233,2,218,
80,99,64,238,165,89,190,70,195,251,17,149,172,45,86,227,73,250,30,96,19,240,21,92,23,107,80,103,214,197,186,179,112,29,173,169,49,32,0,59,129,205,165,139,80,247,213,56,197,146,22,205,128,72,1,3,34,5,12,
136,20,48,32,82,192,128,72,1,3,34,5,12,136,20,48,32,82,192,128,72,1,3,34,5,12,136,38,93,234,143,77,103,115,116,158,43,32,135,251,108,91,10,172,201,212,191,148,242,138,196,246,44,247,253,228,10,200,95,
18,219,175,203,212,191,148,242,129,196,246,125,57,58,207,21,144,157,137,237,219,128,21,153,198,144,230,187,128,102,1,142,126,82,199,228,64,114,5,228,145,196,246,13,192,67,192,249,153,198,145,206,184,16,
216,14,188,33,241,122,234,152,28,72,174,27,166,30,5,158,5,46,235,243,218,22,96,55,240,77,224,103,192,193,33,199,120,121,98,251,82,154,69,4,212,93,169,243,132,213,12,190,239,214,2,239,3,62,15,188,58,241,
51,123,129,199,6,236,119,236,62,76,115,229,192,102,43,221,182,144,201,210,92,29,209,220,235,189,14,120,107,198,62,165,65,221,15,220,147,171,179,220,11,22,172,160,89,241,226,218,204,253,74,139,177,3,184,
9,56,158,171,195,156,223,32,0,167,104,78,202,47,6,54,102,238,91,138,124,7,216,10,156,40,92,199,162,93,11,252,154,242,243,81,219,116,183,157,192,53,140,73,27,107,66,189,131,102,61,220,141,192,107,104,46,
249,186,22,149,134,49,75,243,87,27,251,129,223,208,76,231,127,87,180,34,73,146,36,73,146,212,142,255,3,17,204,81,113,83,66,1,127,0,0,0,0,73,69,78,68,174,66,96,130,0,0};

const char* EditorComponent::save_icon_png = (const char*) resource_PluginEditor_save_icon_png;
const int EditorComponent::save_icon_pngSize = 2362;

WrappedEditor::WrappedEditor(PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts)
: AudioProcessorEditor(p), editorComponent(p, vts)
{
    addAndMakeVisible(editorComponent);
    
    PropertiesFile::Options options;
    options.applicationName = ProjectInfo::projectName;
    options.commonToAllUsers = true;
    options.filenameSuffix = "settings";
    options.osxLibrarySubFolder = "Application Support";
    
    applicationProperties.setStorageParameters(options);
    
    if(auto* constrainer = getConstrainer())
    {
        constrainer->setFixedAspectRatio(static_cast<double>(originalWidth) / static_cast<double>(originalHeight));
        constrainer->setSizeLimits(originalWidth * 0.25, originalHeight * 0.25, originalWidth + originalWidth * 0.20, originalHeight + originalHeight * 0.20);
    }
    
    auto sizeRatio = 1.0;
    if (auto* properties = applicationProperties.getCommonSettings(true))
        sizeRatio = properties->getDoubleValue("sizeRatio", 1.0);
    
    setResizable(true, true);
    setSize(static_cast<int>(originalWidth * sizeRatio), static_cast<int>(originalHeight * sizeRatio));
}

void WrappedEditor::resized()
{
    const auto scaleFactor = static_cast<float> (getWidth()) / originalWidth;
    if (auto* properties = applicationProperties.getCommonSettings(true))
        properties->setValue("sizeRatio", scaleFactor);
    
    editorComponent.setTransform(AffineTransform::scale(scaleFactor));
    editorComponent.setBounds(0, 0, originalWidth, originalHeight);
}
