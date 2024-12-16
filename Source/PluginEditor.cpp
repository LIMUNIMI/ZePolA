#include "Parameters.h"
#include <JuceHeader.h>
#include <cmath>

#include "PluginEditor.h"

EditorComponent::EditorComponent(PolesAndZerosEQAudioProcessor& p,
                                 AudioProcessorValueTreeState& vts)
    : processor(p), valueTreeState(vts)
{
    backgroundColour = juce::Colour(0xffecf0f1);
    warning_label.reset(new juce::Label(
        "Warning label",
        TRANS("Caution! The current plugin configuration has caused an "
              "excessively high output, resulting in the audio stream being "
              "stopped. Please reset the plugin parameters to values that "
              "allow for a lower output volume.")));
    addAndMakeVisible(warning_label.get());
    warning_label->setFont(juce::Font("Gill Sans", 18.00f, juce::Font::plain)
                               .withTypefaceStyle("SemiBold"));
    warning_label->setJustificationType(juce::Justification::centred);
    warning_label->setEditable(false, false, false);
    warning_label->setColour(juce::Label::textColourId,
                             juce::Colour(0xff383838));
    warning_label->setColour(juce::TextEditor::textColourId,
                             juce::Colours::black);
    warning_label->setColour(juce::TextEditor::backgroundColourId,
                             juce::Colour(0x00000000));
    warning_label->setBounds(595, 370, 340, 120);
    warning_label->setVisible(false);

    p.editorCallback = [this]()
    {
        getFrequencyResponse();
        magnitude_response->updateValues(magnitudes, referenceFrequencies,
                                         processor.getSampleRate(), ampDb);
        phase_response->updateValues(phases, referenceFrequencies,
                                     processor.getSampleRate(), true);
        updateElements();
        gaussian_plane->updateConjugate(processor.getFilterElementsChain());
    };

    warningRectangle.reset(new WarningRectangle());
    addAndMakeVisible(warningRectangle.get());
    warningRectangle->setBounds(525, 55, 480, 720);
    warningRectangle->setVisible(false);

    startTimer(50);

    selectable_filter_types       = SELECTABLE_FILTER_TYPES;
    selectable_orders_butterworth = SELECTABLE_ORDERS_BUTTERWORTH;

    getFrequencyResponse();
    updateReferenceFrequencies();

    magnitudesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    phasesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    typesAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    activeAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);
    gainsAttachments.resize(NUMBER_OF_FILTER_ELEMENTS);

    e1_gain.reset(new CustomSlider("Element 1 gain"));
    addAndMakeVisible(e1_gain.get());
    e1_gain->setRange(0, 10, 0);
    e1_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e1_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 30, 10);
    e1_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e1_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e1_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e1_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e1_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e1_gain->addListener(this);

    e1_gain->setBounds(310, 5, 97, 25);

    gaussian_plane.reset(new GaussianPlane(processor.getFilterElementsChain()));
    addAndMakeVisible(gaussian_plane.get());
    gaussian_plane->setName("gaussianPlane");

    gaussian_plane->setBounds(30, 424, 316, 316);

    e1.reset(new DraggableElement(processor.multiChannelCascade[0][0], 1,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e1.get());
    e1->setName("Element 1");
    e1->setBounds(152, 602, 16, 16);

    e2.reset(new DraggableElement(processor.multiChannelCascade[0][1], 2,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e2.get());
    e2->setName("Element 2");
    e2->setBounds(152, 602, 16, 16);

    e3.reset(new DraggableElement(processor.multiChannelCascade[0][2], 3,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e3.get());
    e3->setName("Element 3");
    e3->setBounds(152, 602, 16, 16);

    e4.reset(new DraggableElement(processor.multiChannelCascade[0][3], 4,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e4.get());
    e4->setName("Element 4");
    e4->setBounds(152, 602, 16, 16);

    e5.reset(new DraggableElement(processor.multiChannelCascade[0][4], 5,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e5.get());
    e5->setName("Element 5");
    e5->setBounds(152, 602, 16, 16);

    e6.reset(new DraggableElement(processor.multiChannelCascade[0][5], 6,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e6.get());
    e6->setName("Element 6");
    e6->setBounds(152, 602, 16, 16);

    e7.reset(new DraggableElement(processor.multiChannelCascade[0][6], 7,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e7.get());
    e7->setName("Element 7");
    e7->setBounds(152, 602, 16, 16);

    e8.reset(new DraggableElement(processor.multiChannelCascade[0][7], 8,
                                  gaussian_plane.get(), &processor));
    addAndMakeVisible(e8.get());
    e8->setName("Element 8");
    e8->setBounds(152, 602, 16, 16);

    passbandAmplitude_label.reset(new juce::Label(
        "Passband Amplitude", TRANS("PASSBAND AMPLITUDE (DB)")));
    addAndMakeVisible(passbandAmplitude_label.get());
    passbandAmplitude_label->setFont(
        juce::Font("Gill Sans", 12.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    passbandAmplitude_label->setJustificationType(juce::Justification::centred);
    passbandAmplitude_label->setEditable(false, false, false);
    passbandAmplitude_label->setColour(juce::Label::textColourId,
                                       juce::Colour(0xff383838));
    passbandAmplitude_label->setColour(juce::TextEditor::textColourId,
                                       juce::Colours::black);
    passbandAmplitude_label->setColour(juce::TextEditor::backgroundColourId,
                                       juce::Colour(0x00000000));

    passbandAmplitude_label->setBounds(1020, 360, 159, 20);

    stopbandAmplitude_slider.reset(new CustomSlider("Stopband Amplitude"));
    addAndMakeVisible(stopbandAmplitude_slider.get());
    stopbandAmplitude_slider->setRange(-35, -21, 0.1);
    stopbandAmplitude_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    stopbandAmplitude_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                              50, 20);
    stopbandAmplitude_slider->setColour(juce::Slider::thumbColourId,
                                        juce::Colours::white);
    stopbandAmplitude_slider->setColour(juce::Slider::textBoxTextColourId,
                                        juce::Colour(0xff333333));
    stopbandAmplitude_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                                        juce::Colour(0x00000000));
    stopbandAmplitude_slider->setColour(juce::Slider::textBoxHighlightColourId,
                                        juce::Colour(0x66686868));
    stopbandAmplitude_slider->setColour(juce::Slider::textBoxOutlineColourId,
                                        juce::Colour(0x00000000));
    stopbandAmplitude_slider->addListener(this);

    stopbandAmplitude_slider->setBounds(1022, 366, 135, 25);

    magnitude_response.reset(new MagnitudeResponse(
        magnitudes, referenceFrequencies, processor.getSampleRate(), ampDb));
    addAndMakeVisible(magnitude_response.get());
    magnitude_response->setName("frequencyResponse");

    magnitude_response->setBounds(540, 70, 450, 316);

    mg_response_label.reset(
        new juce::Label("Magnitude response", TRANS("MAGNITUDE RESPONSE\n")));
    addAndMakeVisible(mg_response_label.get());
    mg_response_label->setFont(
        juce::Font("Gill Sans", 13.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    mg_response_label->setJustificationType(juce::Justification::centred);
    mg_response_label->setEditable(false, false, false);
    mg_response_label->setColour(juce::Label::textColourId,
                                 juce::Colour(0xff383838));
    mg_response_label->setColour(juce::TextEditor::textColourId,
                                 juce::Colours::black);
    mg_response_label->setColour(juce::TextEditor::backgroundColourId,
                                 juce::Colour(0x00000000));

    mg_response_label->setBounds(695, 391, 140, 24);

    phase_response.reset(new PhaseResponse(phases, referenceFrequencies,
                                           processor.getSampleRate(), ampDb));
    addAndMakeVisible(phase_response.get());
    phase_response->setName("phaseResponse");

    phase_response->setBounds(540, 424, 450, 316);

    ph_response_label.reset(
        new juce::Label("Phase response", TRANS("PHASE RESPONSE")));
    addAndMakeVisible(ph_response_label.get());
    ph_response_label->setFont(
        juce::Font("Gill Sans", 13.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    ph_response_label->setJustificationType(juce::Justification::centred);
    ph_response_label->setEditable(false, false, false);
    ph_response_label->setColour(juce::Label::textColourId,
                                 juce::Colour(0xff383838));
    ph_response_label->setColour(juce::TextEditor::textColourId,
                                 juce::Colours::black);
    ph_response_label->setColour(juce::TextEditor::backgroundColourId,
                                 juce::Colour(0x00000000));

    ph_response_label->setBounds(710, 745, 110, 24);

    m1_slider.reset(new CustomSlider("Element 1 magnitude"));
    addAndMakeVisible(m1_slider.get());
    m1_slider->setRange(0, 10, 0);
    m1_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m1_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m1_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    m1_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    m1_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    m1_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    m1_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m1_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m1_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m1_slider->addListener(this);

    m1_slider->setBounds(20, 89, 120, 25);

    p1_slider.reset(new CustomSlider("Element 1 phase"));
    addAndMakeVisible(p1_slider.get());
    p1_slider->setRange(0, 10, 0);
    p1_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p1_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p1_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    p1_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    p1_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    p1_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    p1_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p1_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p1_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p1_slider->addListener(this);

    p1_slider->setBounds(144, 89, 120, 25);

    radius_label.reset(new juce::Label("Radius", TRANS("RADIUS\n")));
    addAndMakeVisible(radius_label.get());
    radius_label->setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                              .withTypefaceStyle("SemiBold"));
    radius_label->setJustificationType(juce::Justification::centred);
    radius_label->setEditable(false, false, false);
    radius_label->setColour(juce::Label::textColourId,
                            juce::Colour(0xff383838));
    radius_label->setColour(juce::TextEditor::textColourId,
                            juce::Colours::black);
    radius_label->setColour(juce::TextEditor::backgroundColourId,
                            juce::Colour(0x00000000));

    radius_label->setBounds(35, 57, 90, 24);

    angles_label.reset(new juce::Label("Angle", TRANS("ANGLE\n")));
    addAndMakeVisible(angles_label.get());
    angles_label->setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                              .withTypefaceStyle("SemiBold"));
    angles_label->setJustificationType(juce::Justification::centred);
    angles_label->setEditable(false, false, false);
    angles_label->setColour(juce::Label::textColourId,
                            juce::Colour(0xff383838));
    angles_label->setColour(juce::TextEditor::textColourId,
                            juce::Colours::black);
    angles_label->setColour(juce::TextEditor::backgroundColourId,
                            juce::Colour(0x00000000));

    angles_label->setBounds(195, 57, 80, 24);

    type_label.reset(new juce::Label("Type", TRANS("TYPE")));
    addAndMakeVisible(type_label.get());
    type_label->setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                            .withTypefaceStyle("SemiBold"));
    type_label->setJustificationType(juce::Justification::centred);
    type_label->setEditable(false, false, false);
    type_label->setColour(juce::Label::textColourId, juce::Colour(0xff383838));
    type_label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    type_label->setColour(juce::TextEditor::backgroundColourId,
                          juce::Colour(0x00000000));

    type_label->setBounds(305, 57, 91, 24);

    active_label.reset(new juce::Label("Active", TRANS("ACTIVE")));
    addAndMakeVisible(active_label.get());
    active_label->setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                              .withTypefaceStyle("SemiBold"));
    active_label->setJustificationType(juce::Justification::centred);
    active_label->setEditable(false, false, false);
    active_label->setColour(juce::Label::textColourId,
                            juce::Colour(0xff383838));
    active_label->setColour(juce::TextEditor::textColourId,
                            juce::Colours::black);
    active_label->setColour(juce::TextEditor::backgroundColourId,
                            juce::Colour(0x00000000));

    active_label->setBounds(395, 57, 55, 24);

    e1_type.reset(new CustomToggleButton("Element 1 type"));
    addAndMakeVisible(e1_type.get());
    e1_type->setButtonText(juce::String());
    e1_type->addListener(this);

    e1_type->setBounds(326, 90, 52, 21);

    e2_type.reset(new CustomToggleButton("Element 2 type"));
    addAndMakeVisible(e2_type.get());
    e2_type->setButtonText(juce::String());
    e2_type->addListener(this);

    e2_type->setBounds(326, 130, 52, 21);

    e3_type.reset(new CustomToggleButton("Element 3 type"));
    addAndMakeVisible(e3_type.get());
    e3_type->setButtonText(juce::String());
    e3_type->addListener(this);

    e3_type->setBounds(326, 170, 52, 21);

    e4_type.reset(new CustomToggleButton("Element 4 type"));
    addAndMakeVisible(e4_type.get());
    e4_type->setButtonText(juce::String());
    e4_type->addListener(this);

    e4_type->setBounds(326, 210, 52, 21);

    e5_type.reset(new CustomToggleButton("Element 5 type"));
    addAndMakeVisible(e5_type.get());
    e5_type->setButtonText(juce::String());
    e5_type->addListener(this);

    e5_type->setBounds(326, 250, 52, 21);

    e6_type.reset(new CustomToggleButton("Element 6 type"));
    addAndMakeVisible(e6_type.get());
    e6_type->setButtonText(juce::String());
    e6_type->addListener(this);

    e6_type->setBounds(326, 290, 52, 21);

    e7_type.reset(new CustomToggleButton("Element 7 type"));
    addAndMakeVisible(e7_type.get());
    e7_type->setButtonText(juce::String());
    e7_type->addListener(this);

    e7_type->setBounds(326, 330, 52, 21);

    e8_type.reset(new CustomToggleButton("Element 8 type"));
    addAndMakeVisible(e8_type.get());
    e8_type->setButtonText(juce::String());
    e8_type->addListener(this);

    e8_type->setBounds(326, 370, 52, 21);

    e1_active.reset(new CustomToggleButton("Element 1 active"));
    addAndMakeVisible(e1_active.get());
    e1_active->setButtonText(juce::String());
    e1_active->addListener(this);

    e1_active->setBounds(393, 90, 49, 21);

    e2_active.reset(new CustomToggleButton("Element 2 active"));
    addAndMakeVisible(e2_active.get());
    e2_active->setButtonText(juce::String());
    e2_active->addListener(this);

    e2_active->setBounds(393, 130, 49, 21);

    e3_active.reset(new CustomToggleButton("Element 3 active"));
    addAndMakeVisible(e3_active.get());
    e3_active->setButtonText(juce::String());
    e3_active->addListener(this);

    e3_active->setBounds(393, 170, 49, 21);

    e4_active.reset(new CustomToggleButton("Element 4 active"));
    addAndMakeVisible(e4_active.get());
    e4_active->setButtonText(juce::String());
    e4_active->addListener(this);

    e4_active->setBounds(393, 210, 49, 21);

    e5_active.reset(new CustomToggleButton("Element 5 active"));
    addAndMakeVisible(e5_active.get());
    e5_active->setButtonText(juce::String());
    e5_active->addListener(this);

    e5_active->setBounds(393, 250, 49, 21);

    e6_active.reset(new CustomToggleButton("Element 6 active"));
    addAndMakeVisible(e6_active.get());
    e6_active->setButtonText(juce::String());
    e6_active->addListener(this);

    e6_active->setBounds(393, 290, 49, 21);

    e7_active.reset(new CustomToggleButton("Element 7 active"));
    addAndMakeVisible(e7_active.get());
    e7_active->setButtonText(juce::String());
    e7_active->addListener(this);

    e7_active->setBounds(393, 330, 49, 21);

    e8_active.reset(new CustomToggleButton("Element 8 active"));
    addAndMakeVisible(e8_active.get());
    e8_active->setButtonText(juce::String());
    e8_active->addListener(this);

    e8_active->setBounds(393, 370, 49, 21);

    gaussian_plane_label.reset(
        new juce::Label("Gaussian plane", TRANS("GAUSSIAN PLANE")));
    addAndMakeVisible(gaussian_plane_label.get());
    gaussian_plane_label->setFont(
        juce::Font("Gill Sans", 13.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    gaussian_plane_label->setJustificationType(juce::Justification::centred);
    gaussian_plane_label->setEditable(false, false, false);
    gaussian_plane_label->setColour(juce::Label::textColourId,
                                    juce::Colour(0xff383838));
    gaussian_plane_label->setColour(juce::TextEditor::textColourId,
                                    juce::Colours::black);
    gaussian_plane_label->setColour(juce::TextEditor::backgroundColourId,
                                    juce::Colour(0x00000000));

    gaussian_plane_label->setBounds(118, 745, 140, 24);

    bypass.reset(new CustomToggleButton("Bypass"));
    addAndMakeVisible(bypass.get());
    bypass->setButtonText(juce::String());
    bypass->addListener(this);

    bypass->setBounds(1074, 718, 49, 22);

    masterGain_slider.reset(new CustomSlider("Master Gain"));
    addAndMakeVisible(masterGain_slider.get());
    masterGain_slider->setRange(0, 10, 0);
    masterGain_slider->setSliderStyle(juce::Slider::LinearVertical);
    masterGain_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100,
                                       20);
    masterGain_slider->setColour(juce::Slider::thumbColourId,
                                 juce::Colours::orange);
    masterGain_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff383838));
    masterGain_slider->setColour(juce::Slider::textBoxHighlightColourId,
                                 juce::Colour(0x66686868));
    masterGain_slider->setColour(juce::Slider::textBoxOutlineColourId,
                                 juce::Colour(0x008e989b));

    masterGain_slider->setBounds(1074, 480, 45, 200);

    linLog_switch.reset(new CustomToggleButton("Linear / Logarithmic"));
    addAndMakeVisible(linLog_switch.get());
    linLog_switch->setButtonText(juce::String());
    linLog_switch->addListener(this);
    linLog_switch->setBounds(931, 395, 52, 21);

    m2_slider.reset(new CustomSlider("Element 2 magnitude"));
    addAndMakeVisible(m2_slider.get());
    m2_slider->setRange(0, 10, 0);
    m2_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m2_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m2_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    m2_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    m2_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    m2_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    m2_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m2_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m2_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m2_slider->addListener(this);

    m2_slider->setBounds(20, 129, 120, 25);

    m3_slider.reset(new CustomSlider("Element 3 magnitude"));
    addAndMakeVisible(m3_slider.get());
    m3_slider->setRange(0, 10, 0);
    m3_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m3_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m3_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    m3_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    m3_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    m3_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    m3_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m3_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m3_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m3_slider->addListener(this);

    m3_slider->setBounds(20, 169, 120, 25);

    m4_slider.reset(new CustomSlider("Element 4 magnitude"));
    addAndMakeVisible(m4_slider.get());
    m4_slider->setRange(0, 10, 0);
    m4_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m4_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m4_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    m4_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    m4_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    m4_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    m4_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m4_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m4_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m4_slider->addListener(this);

    m4_slider->setBounds(20, 209, 120, 25);

    m5_slider.reset(new CustomSlider("Element 5 magnitude"));
    addAndMakeVisible(m5_slider.get());
    m5_slider->setRange(0, 10, 0);
    m5_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m5_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m5_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    m5_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    m5_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m5_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m5_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m5_slider->addListener(this);

    m5_slider->setBounds(20, 249, 120, 25);

    m6_slider.reset(new CustomSlider("Element 6 magnitude"));
    addAndMakeVisible(m6_slider.get());
    m6_slider->setRange(0, 10, 0);
    m6_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m6_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m6_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    m6_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    m6_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m6_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m6_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m6_slider->addListener(this);

    m6_slider->setBounds(20, 289, 120, 25);

    m7_slider.reset(new CustomSlider("Element 7 magnitude"));
    addAndMakeVisible(m7_slider.get());
    m7_slider->setRange(0, 10, 0);
    m7_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m7_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m7_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    m7_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    m7_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m7_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m7_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m7_slider->addListener(this);

    m7_slider->setBounds(20, 329, 120, 25);

    m8_slider.reset(new CustomSlider("Element 8 magnitude"));
    addAndMakeVisible(m8_slider.get());
    m8_slider->setRange(0, 10, 0);
    m8_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    m8_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    m8_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    m8_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    m8_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    m8_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    m8_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    m8_slider->addListener(this);

    m8_slider->setBounds(20, 369, 120, 25);

    p2_slider.reset(new CustomSlider("Element 2 phase"));
    addAndMakeVisible(p2_slider.get());
    p2_slider->setRange(0, 10, 0);
    p2_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p2_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p2_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    p2_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    p2_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    p2_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    p2_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p2_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p2_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p2_slider->addListener(this);

    p2_slider->setBounds(144, 129, 120, 25);

    p3_slider.reset(new CustomSlider("Element 3 phase"));
    addAndMakeVisible(p3_slider.get());
    p3_slider->setRange(0, 10, 0);
    p3_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p3_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p3_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    p3_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    p3_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    p3_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    p3_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p3_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p3_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p3_slider->addListener(this);

    p3_slider->setBounds(144, 169, 120, 25);

    p4_slider.reset(new CustomSlider("Element 4 phase"));
    addAndMakeVisible(p4_slider.get());
    p4_slider->setRange(0, 10, 0);
    p4_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p4_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p4_slider->setColour(juce::Slider::backgroundColourId,
                         juce::Colour(0xff383838));
    p4_slider->setColour(juce::Slider::thumbColourId, juce::Colour(0xff909497));
    p4_slider->setColour(juce::Slider::trackColourId, juce::Colour(0xff383838));
    p4_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff383838));
    p4_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p4_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p4_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p4_slider->addListener(this);

    p4_slider->setBounds(144, 209, 120, 25);

    p5_slider.reset(new CustomSlider("Element 5 phase"));
    addAndMakeVisible(p5_slider.get());
    p5_slider->setRange(0, 10, 0);
    p5_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p5_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p5_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    p5_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    p5_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p5_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p5_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p5_slider->addListener(this);

    p5_slider->setBounds(144, 249, 120, 25);

    p6_slider.reset(new CustomSlider("Element 6 phase"));
    addAndMakeVisible(p6_slider.get());
    p6_slider->setRange(0, 10, 0);
    p6_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p6_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p6_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    p6_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    p6_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p6_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p6_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p6_slider->addListener(this);

    p6_slider->setBounds(144, 289, 120, 25);

    p7_slider.reset(new CustomSlider("Element 7 phase"));
    addAndMakeVisible(p7_slider.get());
    p7_slider->setRange(0, 10, 0);
    p7_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p7_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p7_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    p7_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    p7_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p7_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p7_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p7_slider->addListener(this);

    p7_slider->setBounds(144, 329, 120, 25);

    p8_slider.reset(new CustomSlider("Element 8 phase"));
    addAndMakeVisible(p8_slider.get());
    p8_slider->setRange(0, 10, 0);
    p8_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    p8_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    p8_slider->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    p8_slider->setColour(juce::Slider::textBoxTextColourId,
                         juce::Colour(0xff333333));
    p8_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                         juce::Colour(0x00000000));
    p8_slider->setColour(juce::Slider::textBoxHighlightColourId,
                         juce::Colour(0x66686868));
    p8_slider->setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colour(0x00000000));
    p8_slider->addListener(this);

    p8_slider->setBounds(144, 369, 120, 25);

    p1_freq.reset(new CustomLabel("Element 1 frequency", juce::String()));
    addAndMakeVisible(p1_freq.get());
    p1_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p1_freq->setJustificationType(juce::Justification::centredLeft);
    p1_freq->setEditable(true, true, false);
    p1_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p1_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p1_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p1_freq->addListener(this);

    p1_freq->setBounds(265, 89, 60, 25);

    p2_freq.reset(new CustomLabel("Element 2 frequency", juce::String()));
    addAndMakeVisible(p2_freq.get());
    p2_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p2_freq->setJustificationType(juce::Justification::centredLeft);
    p2_freq->setEditable(true, true, false);
    p2_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p2_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p2_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p2_freq->addListener(this);

    p2_freq->setBounds(265, 129, 60, 25);

    p3_freq.reset(new CustomLabel("Element 3 frequency", juce::String()));
    addAndMakeVisible(p3_freq.get());
    p3_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p3_freq->setJustificationType(juce::Justification::centredLeft);
    p3_freq->setEditable(true, true, false);
    p3_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p3_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p3_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p3_freq->addListener(this);

    p3_freq->setBounds(265, 169, 60, 25);

    p4_freq.reset(new CustomLabel("Element 4 frequency", juce::String()));
    addAndMakeVisible(p4_freq.get());
    p4_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p4_freq->setJustificationType(juce::Justification::centredLeft);
    p4_freq->setEditable(true, true, false);
    p4_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p4_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p4_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p4_freq->addListener(this);

    p4_freq->setBounds(265, 209, 60, 25);

    p5_freq.reset(new CustomLabel("Element 5 frequency", juce::String()));
    addAndMakeVisible(p5_freq.get());
    p5_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p5_freq->setJustificationType(juce::Justification::centredLeft);
    p5_freq->setEditable(true, true, false);
    p5_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p5_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p5_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p5_freq->addListener(this);

    p5_freq->setBounds(265, 249, 60, 25);

    p6_freq.reset(new CustomLabel("Element 6 frequency", juce::String()));
    addAndMakeVisible(p6_freq.get());
    p6_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p6_freq->setJustificationType(juce::Justification::centredLeft);
    p6_freq->setEditable(true, true, false);
    p6_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p6_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p6_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p6_freq->addListener(this);

    p6_freq->setBounds(265, 289, 60, 25);

    p7_freq.reset(new CustomLabel("Element 7 frequency", juce::String()));
    addAndMakeVisible(p7_freq.get());
    p7_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p7_freq->setJustificationType(juce::Justification::centredLeft);
    p7_freq->setEditable(true, true, false);
    p7_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p7_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p7_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p7_freq->addListener(this);

    p7_freq->setBounds(265, 329, 60, 25);

    p8_freq.reset(new CustomLabel("Element 8 frequency", juce::String()));
    addAndMakeVisible(p8_freq.get());
    p8_freq->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                         .withTypefaceStyle("Regular"));
    p8_freq->setJustificationType(juce::Justification::centredLeft);
    p8_freq->setEditable(true, true, false);
    p8_freq->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    p8_freq->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    p8_freq->setColour(juce::TextEditor::backgroundColourId,
                       juce::Colours::black);
    p8_freq->addListener(this);

    p8_freq->setBounds(265, 369, 60, 25);

    type_box.reset(new CustomComboBox("Design type"));
    addAndMakeVisible(type_box.get());
    type_box->setEditableText(false);
    type_box->setJustificationType(juce::Justification::centredLeft);
    type_box->setTextWhenNothingSelected(TRANS("TYPE"));
    type_box->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    type_box->addListener(this);

    type_box->setBounds(1025, 145, 140, 25);

    shape_box.reset(new CustomComboBox("Design shape"));
    addAndMakeVisible(shape_box.get());
    shape_box->setEditableText(false);
    shape_box->setJustificationType(juce::Justification::centredLeft);
    shape_box->setTextWhenNothingSelected(TRANS("SHAPE"));
    shape_box->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    shape_box->addItem(TRANS("LOWPASS"), 1);
    shape_box->addItem(TRANS("HIGHPASS"), 2);
    shape_box->addListener(this);

    shape_box->setBounds(1025, 100, 140, 25);

    update_button.reset(new CustomButton("Calculate"));
    addAndMakeVisible(update_button.get());
    update_button->setButtonText(juce::String());
    update_button->addListener(this);
    update_button->setColour(juce::TextButton::buttonColourId,
                             juce::Colour(0xff909497));
    update_button->setColour(juce::TextButton::buttonOnColourId,
                             juce::Colour(0xff505050));

    update_button->setBounds(1091, 410, 75, 25);

    multiply_phases_button.reset(new CustomButton("Multiply phases"));
    addAndMakeVisible(multiply_phases_button.get());
    multiply_phases_button->setButtonText(juce::String());
    multiply_phases_button->addListener(this);
    multiply_phases_button->setColour(juce::TextButton::buttonColourId,
                                      juce::Colour(0xff909497));
    multiply_phases_button->setColour(juce::TextButton::buttonOnColourId,
                                      juce::Colour(0xff505050));

    multiply_phases_button->setBounds(395.5, 581, 80, 25);

    divide_phases_button.reset(new CustomButton("Divide phases"));
    addAndMakeVisible(divide_phases_button.get());
    divide_phases_button->setButtonText(juce::String());
    divide_phases_button->addListener(this);
    divide_phases_button->setColour(juce::TextButton::buttonColourId,
                                    juce::Colour(0xff909497));
    divide_phases_button->setColour(juce::TextButton::buttonOnColourId,
                                    juce::Colour(0xff727272));

    divide_phases_button->setBounds(395.5, 631, 80, 25);

    swap_button.reset(new CustomButton("Swap poles/zeros"));
    addAndMakeVisible(swap_button.get());
    swap_button->setButtonText(juce::String());
    swap_button->addListener(this);
    swap_button->setColour(juce::TextButton::buttonColourId,
                           juce::Colour(0xff909497));
    swap_button->setColour(juce::TextButton::buttonOnColourId,
                           juce::Colour(0xff505050));

    swap_button->setBounds(395.5, 681, 80, 25);

    turn_on_button.reset(new CustomButton("Turn on all the elements"));
    addAndMakeVisible(turn_on_button.get());
    turn_on_button->setButtonText(juce::String());
    turn_on_button->addListener(this);
    turn_on_button->setColour(juce::TextButton::buttonColourId,
                              juce::Colour(0xff73cc81));
    turn_on_button->setColour(juce::TextButton::buttonOnColourId,
                              juce::Colour(0xff505050));

    turn_on_button->setBounds(395.5, 481, 80, 25);

    turn_off_button.reset(new CustomButton("Turn off all the elements"));
    addAndMakeVisible(turn_off_button.get());
    turn_off_button->setButtonText(juce::String());
    turn_off_button->addListener(this);
    turn_off_button->setColour(juce::TextButton::buttonColourId,
                               juce::Colour(0xffe86d5c));
    turn_off_button->setColour(juce::TextButton::buttonOnColourId,
                               juce::Colour(0xff505050));

    turn_off_button->setBounds(395.5, 531, 80, 25);

    order_box.reset(new CustomComboBox("Design order"));
    addAndMakeVisible(order_box.get());
    order_box->setEditableText(false);
    order_box->setJustificationType(juce::Justification::centredLeft);
    order_box->setTextWhenNothingSelected(TRANS("ORDER"));
    order_box->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    order_box->addListener(this);

    order_box->setBounds(1025, 190, 140, 25);

    design_frequency_label.reset(
        new juce::Label("Design frequency", TRANS("FREQUENCY\n")));
    addAndMakeVisible(design_frequency_label.get());
    design_frequency_label->setFont(
        juce::Font("Gill Sans", 12.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    design_frequency_label->setJustificationType(juce::Justification::centred);
    design_frequency_label->setEditable(false, false, false);
    design_frequency_label->setColour(juce::Label::textColourId,
                                      juce::Colour(0xff383838));
    design_frequency_label->setColour(juce::TextEditor::textColourId,
                                      juce::Colours::black);
    design_frequency_label->setColour(juce::TextEditor::backgroundColourId,
                                      juce::Colour(0x00000000));

    design_frequency_label->setBounds(1063, 228, 72, 20);

    ampDb_switch.reset(new CustomToggleButton("Amplitude / dB"));
    addAndMakeVisible(ampDb_switch.get());
    ampDb_switch->setButtonText(juce::String());
    ampDb_switch->addListener(this);

    ampDb_switch->setBounds(543, 395, 52, 21);

    frequency_design_slider.reset(new CustomSlider("Frequency design slider"));
    addAndMakeVisible(frequency_design_slider.get());
    frequency_design_slider->setRange(0.0001, 1000, 0.0001);
    frequency_design_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    frequency_design_slider->setTextBoxStyle(juce::Slider::NoTextBox, true, 50,
                                             20);
    frequency_design_slider->setColour(juce::Slider::thumbColourId,
                                       juce::Colours::white);
    frequency_design_slider->setColour(juce::Slider::textBoxTextColourId,
                                       juce::Colour(0xff333333));
    frequency_design_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                                       juce::Colour(0x00000000));
    frequency_design_slider->setColour(juce::Slider::textBoxHighlightColourId,
                                       juce::Colour(0x66686868));
    frequency_design_slider->setColour(juce::Slider::textBoxOutlineColourId,
                                       juce::Colour(0x00000000));
    frequency_design_slider->addListener(this);

    frequency_design_slider->setBounds(1022, 250, 83, 25);

    frequency_label.reset(new CustomLabel("Frequency Label", juce::String()));
    addAndMakeVisible(frequency_label.get());
    frequency_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                                 .withTypefaceStyle("Regular"));
    frequency_label->setJustificationType(juce::Justification::centredLeft);
    frequency_label->setEditable(true, true, false);
    frequency_label->setColour(juce::Label::textColourId,
                               juce::Colour(0xff333333));
    frequency_label->setColour(juce::TextEditor::textColourId,
                               juce::Colours::black);
    frequency_label->setColour(juce::TextEditor::backgroundColourId,
                               juce::Colours::black);
    frequency_label->addListener(this);

    frequency_label->setBounds(1110, 250, 60, 25);

    transition_width_label.reset(
        new juce::Label("Transition width", TRANS("TRANSITION WIDTH")));
    addAndMakeVisible(transition_width_label.get());
    transition_width_label->setFont(
        juce::Font("Gill Sans", 12.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    transition_width_label->setJustificationType(juce::Justification::centred);
    transition_width_label->setEditable(false, false, false);
    transition_width_label->setColour(juce::Label::textColourId,
                                      juce::Colour(0xff383838));
    transition_width_label->setColour(juce::TextEditor::textColourId,
                                      juce::Colours::black);
    transition_width_label->setColour(juce::TextEditor::backgroundColourId,
                                      juce::Colour(0x00000000));

    transition_width_label->setBounds(1031, 291, 129, 20);

    transition_width_slider.reset(new CustomSlider("Transition width slider"));
    addAndMakeVisible(transition_width_slider.get());
    transition_width_slider->setRange(0.001, 0.5, 1e-05);
    transition_width_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    transition_width_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                             50, 20);
    transition_width_slider->setColour(juce::Slider::thumbColourId,
                                       juce::Colours::white);
    transition_width_slider->setColour(juce::Slider::textBoxTextColourId,
                                       juce::Colour(0xff333333));
    transition_width_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                                       juce::Colour(0x00000000));
    transition_width_slider->setColour(juce::Slider::textBoxHighlightColourId,
                                       juce::Colour(0x66686868));
    transition_width_slider->setColour(juce::Slider::textBoxOutlineColourId,
                                       juce::Colour(0x00000000));
    transition_width_slider->addListener(this);

    transition_width_slider->setBounds(1022, 315, 135, 25);

    passbandAmplitude_slider.reset(new CustomSlider("Passband Amplitude"));
    addAndMakeVisible(passbandAmplitude_slider.get());
    passbandAmplitude_slider->setRange(-10, -0.1, 0.1);
    passbandAmplitude_slider->setSliderStyle(juce::Slider::LinearHorizontal);
    passbandAmplitude_slider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                              50, 20);
    passbandAmplitude_slider->setColour(juce::Slider::thumbColourId,
                                        juce::Colours::white);
    passbandAmplitude_slider->setColour(juce::Slider::textBoxTextColourId,
                                        juce::Colour(0xff333333));
    passbandAmplitude_slider->setColour(juce::Slider::textBoxBackgroundColourId,
                                        juce::Colour(0x00000000));
    passbandAmplitude_slider->setColour(juce::Slider::textBoxHighlightColourId,
                                        juce::Colour(0x66686868));
    passbandAmplitude_slider->setColour(juce::Slider::textBoxOutlineColourId,
                                        juce::Colour(0x00000000));
    passbandAmplitude_slider->addListener(this);

    passbandAmplitude_slider->setBounds(1022, 379, 135, 25);

    stopbandAmplitude_label.reset(new juce::Label(
        "Stopband Amplitude", TRANS("STOPBAND AMPLITUDE (DB)")));
    addAndMakeVisible(stopbandAmplitude_label.get());
    stopbandAmplitude_label->setFont(
        juce::Font("Gill Sans", 12.00f, juce::Font::plain)
            .withTypefaceStyle("SemiBold"));
    stopbandAmplitude_label->setJustificationType(juce::Justification::centred);
    stopbandAmplitude_label->setEditable(false, false, false);
    stopbandAmplitude_label->setColour(juce::Label::textColourId,
                                       juce::Colour(0xff383838));
    stopbandAmplitude_label->setColour(juce::TextEditor::textColourId,
                                       juce::Colours::black);
    stopbandAmplitude_label->setColour(juce::TextEditor::backgroundColourId,
                                       juce::Colour(0x00000000));

    stopbandAmplitude_label->setBounds(1020, 337, 159, 20);

    autoUpdate_button.reset(new CustomToggleButton("Auto update"));
    addAndMakeVisible(autoUpdate_button.get());
    autoUpdate_button->setButtonText(juce::String());
    autoUpdate_button->addListener(this);

    autoUpdate_button->setBounds(1024, 410, 55, 25);

    undo_button.reset(new CustomButton("Undo"));
    addAndMakeVisible(undo_button.get());
    undo_button->setButtonText(juce::String());
    undo_button->addListener(this);
    undo_button->setColour(juce::TextButton::buttonColourId,
                           juce::Colour(0x00909497));
    undo_button->setColour(juce::TextButton::buttonOnColourId,
                           juce::Colour(0xff505050));

    undo_button->setBounds(18, 9, 60, 25);

    redo_button.reset(new CustomButton("Redo"));
    addAndMakeVisible(redo_button.get());
    redo_button->setButtonText(juce::String());
    redo_button->addListener(this);
    redo_button->setColour(juce::TextButton::buttonColourId,
                           juce::Colour(0x00909497));
    redo_button->setColour(juce::TextButton::buttonOnColourId,
                           juce::Colour(0xff505050));

    redo_button->setBounds(91, 9, 60, 25);

    reset_button.reset(new CustomButton("Reset"));
    addAndMakeVisible(reset_button.get());
    reset_button->setButtonText(juce::String());
    reset_button->addListener(this);
    reset_button->setColour(juce::TextButton::buttonColourId,
                            juce::Colour(0x00909497));
    reset_button->setColour(juce::TextButton::buttonOnColourId,
                            juce::Colour(0xff505050));

    reset_button->setBounds(166, 9, 120, 25);

    saveCoefficients_button.reset(new CustomButton("Export coefficients"));
    addAndMakeVisible(saveCoefficients_button.get());
    saveCoefficients_button->setButtonText(juce::String());
    saveCoefficients_button->addListener(this);
    saveCoefficients_button->setColour(juce::TextButton::buttonColourId,
                                       juce::Colour(0x00909497));
    saveCoefficients_button->setColour(juce::TextButton::buttonOnColourId,
                                       juce::Colour(0xff505050));

    saveCoefficients_button->setBounds(820, 9, 150, 25);

    save_preset_button.reset(new CustomButton("Save preset"));
    addAndMakeVisible(save_preset_button.get());
    save_preset_button->setButtonText(juce::String());
    save_preset_button->addListener(this);
    save_preset_button->setColour(juce::TextButton::buttonColourId,
                                  juce::Colour(0x00909497));
    save_preset_button->setColour(juce::TextButton::buttonOnColourId,
                                  juce::Colour(0xff505050));

    save_preset_button->setBounds(971, 9, 100, 25);

    load_preset_button.reset(new CustomButton("Load preset"));
    addAndMakeVisible(load_preset_button.get());
    load_preset_button->setButtonText(juce::String());
    load_preset_button->addListener(this);
    load_preset_button->setColour(juce::TextButton::buttonColourId,
                                  juce::Colour(0x00909497));
    load_preset_button->setColour(juce::TextButton::buttonOnColourId,
                                  juce::Colour(0xff505050));

    load_preset_button->setBounds(1082, 9, 100, 25);

    gain_label.reset(new juce::Label("Gain", TRANS("GAIN")));
    addAndMakeVisible(gain_label.get());
    gain_label->setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                            .withTypefaceStyle("SemiBold"));
    gain_label->setJustificationType(juce::Justification::centred);
    gain_label->setEditable(false, false, false);
    gain_label->setColour(juce::Label::textColourId, juce::Colour(0xff383838));
    gain_label->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    gain_label->setColour(juce::TextEditor::backgroundColourId,
                          juce::Colour(0x00000000));

    gain_label->setBounds(460, 57, 55, 24);

    autoGain.reset(new CustomToggleButton("Auto Gain"));
    addAndMakeVisible(autoGain.get());
    autoGain->setButtonText(juce::String());
    autoGain->addListener(this);

    autoGain->setBounds(463, 12, 49, 21);

    e2_gain.reset(new CustomSlider("Element 2 gain"));
    addAndMakeVisible(e2_gain.get());
    e2_gain->setRange(0, 10, 0);
    e2_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e2_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e2_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e2_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e2_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e2_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e2_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e2_gain->addListener(this);

    e2_gain->setBounds(310, 5, 97, 25);

    e3_gain.reset(new CustomSlider("Element 3 gain"));
    addAndMakeVisible(e3_gain.get());
    e3_gain->setRange(0, 10, 0);
    e3_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e3_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e3_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e3_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e3_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e3_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e3_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e3_gain->addListener(this);

    e3_gain->setBounds(310, 8, 97, 25);

    e4_gain.reset(new CustomSlider("Element 4 gain"));
    addAndMakeVisible(e4_gain.get());
    e4_gain->setRange(0, 10, 0);
    e4_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e4_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e4_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e4_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e4_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e4_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e4_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e4_gain->addListener(this);

    e4_gain->setBounds(310, 8, 97, 25);

    e5_gain.reset(new CustomSlider("Element 5 gain"));
    addAndMakeVisible(e5_gain.get());
    e5_gain->setRange(0, 10, 0);
    e5_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e5_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e5_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e5_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e5_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e5_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e5_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e5_gain->addListener(this);

    e5_gain->setBounds(310, 8, 97, 25);

    e6_gain.reset(new CustomSlider("Element 6 gain"));
    addAndMakeVisible(e6_gain.get());
    e6_gain->setRange(0, 10, 0);
    e6_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e6_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e6_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e6_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e6_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e6_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e6_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e6_gain->addListener(this);

    e6_gain->setBounds(310, 8, 97, 25);

    e7_gain.reset(new CustomSlider("Element 7 gain"));
    addAndMakeVisible(e7_gain.get());
    e7_gain->setRange(0, 10, 0);
    e7_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e7_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e7_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e7_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e7_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e7_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e7_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e7_gain->addListener(this);

    e7_gain->setBounds(310, 8, 97, 25);

    e8_gain.reset(new CustomSlider("Element 8 gain"));
    addAndMakeVisible(e8_gain.get());
    e8_gain->setRange(0, 10, 0);
    e8_gain->setSliderStyle(juce::Slider::LinearHorizontal);
    e8_gain->setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    e8_gain->setColour(juce::Slider::thumbColourId, juce::Colours::white);
    e8_gain->setColour(juce::Slider::textBoxTextColourId,
                       juce::Colour(0xff333333));
    e8_gain->setColour(juce::Slider::textBoxBackgroundColourId,
                       juce::Colour(0x00000000));
    e8_gain->setColour(juce::Slider::textBoxHighlightColourId,
                       juce::Colour(0x66686868));
    e8_gain->setColour(juce::Slider::textBoxOutlineColourId,
                       juce::Colour(0x00000000));
    e8_gain->addListener(this);

    e8_gain->setBounds(310, 8, 97, 25);

    gain1_label.reset(new DraggableGainLabel("Element 1 Gain", juce::String()));
    addAndMakeVisible(gain1_label.get());
    gain1_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain1_label->setJustificationType(juce::Justification::centredLeft);
    gain1_label->setEditable(true, true, false);
    gain1_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain1_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain1_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain1_label->addListener(this);

    gain1_label->setBounds(466, 89, 50, 25);

    gain2_label.reset(new DraggableGainLabel("Element 2 Gain", juce::String()));
    addAndMakeVisible(gain2_label.get());
    gain2_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain2_label->setJustificationType(juce::Justification::centredLeft);
    gain2_label->setEditable(true, true, false);
    gain2_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain2_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain2_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain2_label->addListener(this);

    gain2_label->setBounds(466, 129, 50, 25);

    gain3_label.reset(new DraggableGainLabel("Element 3 Gain", juce::String()));
    addAndMakeVisible(gain3_label.get());
    gain3_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain3_label->setJustificationType(juce::Justification::centredLeft);
    gain3_label->setEditable(true, true, false);
    gain3_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain3_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain3_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain3_label->addListener(this);

    gain3_label->setBounds(466, 169, 50, 25);

    gain4_label.reset(new DraggableGainLabel("Element 4 Gain", juce::String()));
    addAndMakeVisible(gain4_label.get());
    gain4_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain4_label->setJustificationType(juce::Justification::centredLeft);
    gain4_label->setEditable(true, true, false);
    gain4_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain4_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain4_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain4_label->addListener(this);

    gain4_label->setBounds(466, 209, 50, 25);

    gain5_label.reset(new DraggableGainLabel("Element 5 Gain", juce::String()));
    addAndMakeVisible(gain5_label.get());
    gain5_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain5_label->setJustificationType(juce::Justification::centredLeft);
    gain5_label->setEditable(true, true, false);
    gain5_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain5_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain5_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain5_label->addListener(this);

    gain5_label->setBounds(466, 249, 50, 25);

    gain6_label.reset(new DraggableGainLabel("Element 6 Gain", juce::String()));
    addAndMakeVisible(gain6_label.get());
    gain6_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain6_label->setJustificationType(juce::Justification::centredLeft);
    gain6_label->setEditable(true, true, false);
    gain6_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain6_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain6_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain6_label->addListener(this);

    gain6_label->setBounds(466, 289, 50, 25);

    gain7_label.reset(new DraggableGainLabel("Element 7 Gain", juce::String()));
    addAndMakeVisible(gain7_label.get());
    gain7_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain7_label->setJustificationType(juce::Justification::centredLeft);
    gain7_label->setEditable(true, true, false);
    gain7_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain7_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain7_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain7_label->addListener(this);

    gain7_label->setBounds(466, 329, 50, 25);

    gain8_label.reset(new DraggableGainLabel("Element 8 Gain", juce::String()));
    addAndMakeVisible(gain8_label.get());
    gain8_label->setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                             .withTypefaceStyle("Regular"));
    gain8_label->setJustificationType(juce::Justification::centredLeft);
    gain8_label->setEditable(true, true, false);
    gain8_label->setColour(juce::Label::textColourId, juce::Colour(0xff333333));
    gain8_label->setColour(juce::TextEditor::textColourId,
                           juce::Colours::black);
    gain8_label->setColour(juce::TextEditor::backgroundColourId,
                           juce::Colours::black);
    gain8_label->addListener(this);

    gain8_label->setBounds(466, 369, 50, 25);

    anticlockwise_arrow_svg = juce::Drawable::createFromImageData(
        BinaryData::anticlockwise_arrow_svg,
        BinaryData::anticlockwise_arrow_svgSize);
    clockwise_arrow_svg = juce::Drawable::createFromImageData(
        BinaryData::clockwise_arrow_svg, BinaryData::clockwise_arrow_svgSize);
    load_icon_svg = juce::Drawable::createFromImageData(
        BinaryData::load_icon_svg, BinaryData::load_icon_svgSize);
    save_icon_svg = juce::Drawable::createFromImageData(
        BinaryData::save_icon_svg, BinaryData::save_icon_svgSize);

    magnitudesAttachments[0].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(1), *m1_slider));
    magnitudesAttachments[1].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(2), *m2_slider));
    magnitudesAttachments[2].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(3), *m3_slider));
    magnitudesAttachments[3].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(4), *m4_slider));
    magnitudesAttachments[4].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(5), *m5_slider));
    magnitudesAttachments[5].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(6), *m6_slider));
    magnitudesAttachments[6].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(7), *m7_slider));
    magnitudesAttachments[7].reset(new SliderAttachment(
        valueTreeState, MAGNITUDE_NAME + std::to_string(8), *m8_slider));

    phasesAttachments[0].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(1), *p1_slider));
    phasesAttachments[1].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(2), *p2_slider));
    phasesAttachments[2].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(3), *p3_slider));
    phasesAttachments[3].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(4), *p4_slider));
    phasesAttachments[4].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(5), *p5_slider));
    phasesAttachments[5].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(6), *p6_slider));
    phasesAttachments[6].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(7), *p7_slider));
    phasesAttachments[7].reset(new SliderAttachment(
        valueTreeState, PHASE_NAME + std::to_string(8), *p8_slider));

    typesAttachments[0].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(1), *e1_type));
    typesAttachments[1].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(2), *e2_type));
    typesAttachments[2].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(3), *e3_type));
    typesAttachments[3].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(4), *e4_type));
    typesAttachments[4].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(5), *e5_type));
    typesAttachments[5].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(6), *e6_type));
    typesAttachments[6].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(7), *e7_type));
    typesAttachments[7].reset(new ButtonAttachment(
        valueTreeState, TYPE_NAME + std::to_string(8), *e8_type));

    activeAttachments[0].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(1), *e1_active));
    activeAttachments[1].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(2), *e2_active));
    activeAttachments[2].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(3), *e3_active));
    activeAttachments[3].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(4), *e4_active));
    activeAttachments[4].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(5), *e5_active));
    activeAttachments[5].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(6), *e6_active));
    activeAttachments[6].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(7), *e7_active));
    activeAttachments[7].reset(new ButtonAttachment(
        valueTreeState, ACTIVE_NAME + std::to_string(8), *e8_active));

    gainsAttachments[0].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(1), *e1_gain));
    gainsAttachments[1].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(2), *e2_gain));
    gainsAttachments[2].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(3), *e3_gain));
    gainsAttachments[3].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(4), *e4_gain));
    gainsAttachments[4].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(5), *e5_gain));
    gainsAttachments[5].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(6), *e6_gain));
    gainsAttachments[6].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(7), *e7_gain));
    gainsAttachments[7].reset(new SliderAttachment(
        valueTreeState, GAIN_NAME + std::to_string(8), *e8_gain));

    masterGainAttachment.reset(new SliderAttachment(
        valueTreeState, MASTER_GAIN_NAME, *masterGain_slider));
    bypassAttachment.reset(
        new ButtonAttachment(valueTreeState, FILTER_BYPASS_NAME, *bypass));

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

    juce::Slider* sliders[]
        = {e1_gain.get(), e2_gain.get(), e3_gain.get(), e4_gain.get(),
           e5_gain.get(), e6_gain.get(), e7_gain.get(), e8_gain.get()};
    for (auto& slider : sliders) slider->setVisible(false);

    bypass->setLookAndFeel(&activeSwitchesTheme);

    autoGain->setLookAndFeel(&activeSwitchesTheme);

    updateButtonTheme.setTextToDisplay("UPDATE");
    updateButtonTheme.setFontSize(12.0f);
    update_button->setLookAndFeel(&updateButtonTheme);

    swapButtonTheme.setTextToDisplay("SWAP Ps/Zs");
    swap_button->setLookAndFeel(&swapButtonTheme);

    multiplyPhasesTheme.setTextToDisplay("PHASES x 2");
    multiply_phases_button->setLookAndFeel(&multiplyPhasesTheme);

    dividePhasesTheme.setTextToDisplay("PHASES / 2");
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

    saveCoefficientsButtonTheme.setButtonFunction(
        MenuButtonTheme::buttonFunction::SAVE_COEFFS);
    saveCoefficients_button->setLookAndFeel(&saveCoefficientsButtonTheme);
    saveCoefficients_button->repaint();

    savePresetButtonTheme.setButtonFunction(
        MenuButtonTheme::buttonFunction::SAVE_PRST);
    save_preset_button->setLookAndFeel(&savePresetButtonTheme);
    save_preset_button->repaint();

    loadPresetButtonTheme.setButtonFunction(
        MenuButtonTheme::buttonFunction::LOAD_PRST);
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

    design_frequency_label->setBounds(1058, 248, 72, 20);
    frequency_design_slider->setBounds(1027, 270, 83, 25);
    frequency_label->setBounds(1110, 270, 60, 25);

    transition_width_label->setVisible(false);
    transition_width_slider->setVisible(false);

    passbandAmplitude_label->setVisible(false);
    passbandAmplitude_slider->setVisible(false);

    stopbandAmplitude_label->setVisible(false);
    stopbandAmplitude_slider->setVisible(false);

    update_button->setEnabled(false);

    updateDesignSliderFromFrequency(DESIGN_FREQUENCY_FLOOR,
                                    frequency_design_slider.get(), sampleRate);

    autoUpdate_button->setLookAndFeel(&autoUpdateSwitchTheme);

    updateElements();
    slidersInit();
    autoGain->setToggleState(false, NotificationType::sendNotification);
    // gainsInit();
}

EditorComponent::~EditorComponent()
{
    processor.editorCallback = nullptr;

    for (int i = 0; i < NUMBER_OF_FILTER_ELEMENTS; ++i)
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

    juce::Slider* sliders[] = {m1_slider.get(),
                               m2_slider.get(),
                               m3_slider.get(),
                               m4_slider.get(),
                               m5_slider.get(),
                               m6_slider.get(),
                               m7_slider.get(),
                               m8_slider.get(),
                               p1_slider.get(),
                               p2_slider.get(),
                               p3_slider.get(),
                               p4_slider.get(),
                               p5_slider.get(),
                               p6_slider.get(),
                               p7_slider.get(),
                               p8_slider.get(),
                               e1_gain.get(),
                               e2_gain.get(),
                               e3_gain.get(),
                               e4_gain.get(),
                               e5_gain.get(),
                               e6_gain.get(),
                               e7_gain.get(),
                               e8_gain.get(),
                               frequency_design_slider.get(),
                               stopbandAmplitude_slider.get(),
                               passbandAmplitude_slider.get(),
                               masterGain_slider.get(),
                               transition_width_slider.get()};

    for (auto* slider : sliders)
    {
        slider->removeListener(this);
    }

    juce::Button* buttons[] = {reset_button.get(),
                               update_button.get(),
                               multiply_phases_button.get(),
                               divide_phases_button.get(),
                               swap_button.get(),
                               turn_on_button.get(),
                               turn_off_button.get(),
                               undo_button.get(),
                               redo_button.get(),
                               saveCoefficients_button.get(),
                               save_preset_button.get(),
                               load_preset_button.get()};

    for (auto* button : buttons)
    {
        button->removeListener(this);
    }

    juce::ToggleButton* toggButtons[]
        = {e1_type.get(),           e2_type.get(),       e3_type.get(),
           e4_type.get(),           e5_type.get(),       e6_type.get(),
           e7_type.get(),           e8_type.get(),       e1_active.get(),
           e2_active.get(),         e3_active.get(),     e4_active.get(),
           e5_active.get(),         e6_active.get(),     e7_active.get(),
           e8_active.get(),         linLog_switch.get(), ampDb_switch.get(),
           autoUpdate_button.get(), autoGain.get(),      bypass.get()};

    for (auto* toggButton : toggButtons)
    {
        toggButton->removeListener(this);
    }

    juce::Label* labels[]
        = {p1_freq.get(),     p2_freq.get(),     p3_freq.get(),
           p4_freq.get(),     p5_freq.get(),     p6_freq.get(),
           p7_freq.get(),     p8_freq.get(),     frequency_label.get(),
           gain1_label.get(), gain2_label.get(), gain3_label.get(),
           gain4_label.get(), gain5_label.get(), gain6_label.get(),
           gain7_label.get(), gain8_label.get()};

    for (auto* label : labels)
    {
        label->removeListener(this);
    }

    juce::ComboBox* boxes[]
        = {type_box.get(), shape_box.get(), order_box.get()};

    for (auto* box : boxes)
    {
        box->removeListener(this);
    }

    e1_gain                  = nullptr;
    gaussian_plane           = nullptr;
    e2                       = nullptr;
    e3                       = nullptr;
    e4                       = nullptr;
    e5                       = nullptr;
    e6                       = nullptr;
    e7                       = nullptr;
    e8                       = nullptr;
    e1                       = nullptr;
    passbandAmplitude_label  = nullptr;
    stopbandAmplitude_slider = nullptr;
    reset_button             = nullptr;
    magnitude_response       = nullptr;
    mg_response_label        = nullptr;
    phase_response           = nullptr;
    ph_response_label        = nullptr;
    m1_slider                = nullptr;
    p1_slider                = nullptr;
    radius_label             = nullptr;
    angles_label             = nullptr;
    type_label               = nullptr;
    active_label             = nullptr;
    e1_type                  = nullptr;
    e2_type                  = nullptr;
    e3_type                  = nullptr;
    e4_type                  = nullptr;
    e5_type                  = nullptr;
    e6_type                  = nullptr;
    e7_type                  = nullptr;
    e8_type                  = nullptr;
    e1_active                = nullptr;
    e2_active                = nullptr;
    e3_active                = nullptr;
    e4_active                = nullptr;
    e5_active                = nullptr;
    e6_active                = nullptr;
    e7_active                = nullptr;
    e8_active                = nullptr;
    gaussian_plane_label     = nullptr;
    bypass                   = nullptr;
    masterGain_slider        = nullptr;
    linLog_switch            = nullptr;
    m2_slider                = nullptr;
    m3_slider                = nullptr;
    m4_slider                = nullptr;
    m5_slider                = nullptr;
    m6_slider                = nullptr;
    m7_slider                = nullptr;
    m8_slider                = nullptr;
    p2_slider                = nullptr;
    p3_slider                = nullptr;
    p4_slider                = nullptr;
    p5_slider                = nullptr;
    p6_slider                = nullptr;
    p7_slider                = nullptr;
    p8_slider                = nullptr;
    p1_freq                  = nullptr;
    p2_freq                  = nullptr;
    p3_freq                  = nullptr;
    p4_freq                  = nullptr;
    p5_freq                  = nullptr;
    p6_freq                  = nullptr;
    p7_freq                  = nullptr;
    p8_freq                  = nullptr;
    type_box                 = nullptr;
    shape_box                = nullptr;
    update_button            = nullptr;
    multiply_phases_button   = nullptr;
    divide_phases_button     = nullptr;
    swap_button              = nullptr;
    turn_on_button           = nullptr;
    turn_off_button          = nullptr;
    order_box                = nullptr;
    design_frequency_label   = nullptr;
    ampDb_switch             = nullptr;
    frequency_design_slider  = nullptr;
    frequency_label          = nullptr;
    transition_width_label   = nullptr;
    transition_width_slider  = nullptr;
    passbandAmplitude_slider = nullptr;
    stopbandAmplitude_label  = nullptr;
    autoUpdate_button        = nullptr;
    undo_button              = nullptr;
    redo_button              = nullptr;
    saveCoefficients_button  = nullptr;
    save_preset_button       = nullptr;
    load_preset_button       = nullptr;
    gain_label               = nullptr;
    autoGain                 = nullptr;
    e2_gain                  = nullptr;
    e3_gain                  = nullptr;
    e4_gain                  = nullptr;
    e5_gain                  = nullptr;
    e6_gain                  = nullptr;
    e7_gain                  = nullptr;
    e8_gain                  = nullptr;
    gain1_label              = nullptr;
    gain2_label              = nullptr;
    gain3_label              = nullptr;
    gain4_label              = nullptr;
    gain5_label              = nullptr;
    gain6_label              = nullptr;
    gain7_label              = nullptr;
    gain8_label              = nullptr;

    warningRectangle = nullptr;
    warning_label    = nullptr;
}

void EditorComponent::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    {
        float x = 15.0f, y = 55.0f, width = 510.0f, height = 720.0f;
        juce::Colour fillColour   = juce::Colour(0x17b1b1b1);
        juce::Colour strokeColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.fillRoundedRectangle(x, y, width, height, 14.500f);
        g.setColour(strokeColour);
        g.drawRoundedRectangle(x, y, width, height, 14.500f, 1.500f);
    }

    {
        float x = 525.0f, y = 55.0f, width = 480.0f, height = 720.0f;
        juce::Colour fillColour   = juce::Colour(0x17b1b1b1);
        juce::Colour strokeColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.fillRoundedRectangle(x, y, width, height, 14.500f);
        g.setColour(strokeColour);
        g.drawRoundedRectangle(x, y, width, height, 14.500f, 1.500f);
    }

    {
        float x = 1005.0f, y = 55.0f, width = 180.0f, height = 396;
        juce::Colour fillColour   = juce::Colour(0x19656565);
        juce::Colour strokeColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.fillRoundedRectangle(x, y, width, height, 14.500f);
        g.setColour(strokeColour);
        g.drawRoundedRectangle(x, y, width, height, 14.500f, 1.500f);
    }

    {
        float x = 1005.0f, y = 451, width = 180.0f, height = 324;
        juce::Colour fillColour   = juce::Colour(0x19656565);
        juce::Colour strokeColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.fillRoundedRectangle(x, y, width, height, 14.500f);
        g.setColour(strokeColour);
        g.drawRoundedRectangle(x, y, width, height, 14.500f, 1.500f);
    }

    {
        int x = 15, y = 120, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 160, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 200, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 240, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 280, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 320, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 360, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 15, y = 400, width = 510, height = 1;
        juce::Colour fillColour = juce::Colour(0x25909497);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        int x = 1040, y = 60, width = 110, height = 24;
        juce::String text(TRANS("FILTER DESIGN"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }

    {
        int x = 1055, y = 460, width = 80, height = 20;
        juce::String text(TRANS("OUTPUT GAIN"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 11.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }

    {
        float x = 361, y = 424, width = 149, height = 316;
        juce::Colour fillColour   = juce::Colour(0x11b1b1b1);
        juce::Colour strokeColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.fillRoundedRectangle(x, y, width, height, 14.500f);
        g.setColour(strokeColour);
        g.drawRoundedRectangle(x, y, width, height, 14.500f, 0.500f);
    }

    {
        float x = 385.5, y = 435, width = 100, height = 20;
        juce::String text(TRANS("SHORTCUTS"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 12.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }

    {
        int x = 0, y = 40, width = 1200, height = 1;
        juce::Colour fillColour = juce::Colour(0x91383838);
        g.setColour(fillColour);
        g.fillRect(x, y, width, height);
    }

    {
        float x = 59, y = 16, width = 10, height = 10;
        g.setColour(juce::Colours::black);
        anticlockwise_arrow_svg->drawWithin(
            g, {x, y, width, height}, juce::RectanglePlacement::centred, 1.0);
    }

    {
        float x = 128, y = 16, width = 10, height = 10;
        g.setColour(juce::Colours::black);
        clockwise_arrow_svg->drawWithin(g, {x, y, width, height},
                                        juce::RectanglePlacement::centred, 1.0);
    }

    {
        float x = 1161, y = 11, width = 23, height = 19;
        g.setColour(juce::Colours::black);
        load_icon_svg->drawWithin(g, {x, y, width, height},
                                  juce::RectanglePlacement::centred, 1.0);
    }

    {
        float x = 1047, y = 15, width = 17, height = 12;
        g.setColour(juce::Colours::black);
        save_icon_svg->drawWithin(g, {x, y, width, height},
                                  juce::RectanglePlacement::centred, 1.0);
    }

    {
        int x = 403, y = 12, width = 56, height = 20;
        juce::String text(TRANS("AUTO GAIN"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 10.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }

    {
        int x = 1115, y = 661, width = 35, height = 20;
        juce::String text(TRANS("dB"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }

    {
        int x = 1059, y = 695, width = 80, height = 20;
        juce::String text(TRANS("BYPASS"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 13.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }

    {
        int x = 605, y = 12, width = 150, height = 20;
        juce::String text(TRANS("Coded at LIM by ANDREA CASATI"));
        juce::Colour fillColour = juce::Colour(0xff383838);
        g.setColour(fillColour);
        g.setFont(juce::Font("Gill Sans", 10.00f, juce::Font::plain)
                      .withTypefaceStyle("SemiBold"));
        g.drawText(text, x, y, width, height, juce::Justification::centred,
                   true);
    }
}

void EditorComponent::sliderValueChanged(juce::Slider* sliderThatWasMoved)
{
    const double sampleRate = processor.getSampleRate();

    if (sliderThatWasMoved == e1_gain.get())
    {
        gain1_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == stopbandAmplitude_slider.get())
    {
        stopbandAmplitudedB = sliderThatWasMoved->getValue();
        setTransitionWidthRange();

        if (autoUpdate) autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == m1_slider.get())
    {
        auto element = processor.multiChannelCascade[0][0];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(1);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(1)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p1_slider.get())
    {
        updateFrequencyFromSlider(p1_slider.get(), p1_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(1);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(1)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m2_slider.get())
    {
        auto element = processor.multiChannelCascade[0][1];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(2);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(2)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m3_slider.get())
    {
        auto element = processor.multiChannelCascade[0][2];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(3);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(3)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m4_slider.get())
    {
        auto element = processor.multiChannelCascade[0][3];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(4);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(4)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m5_slider.get())
    {
        auto element = processor.multiChannelCascade[0][4];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(5);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(5)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m6_slider.get())
    {
        auto element = processor.multiChannelCascade[0][5];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(6);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(6)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m7_slider.get())
    {
        auto element = processor.multiChannelCascade[0][6];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(7);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(7)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == m8_slider.get())
    {
        auto element = processor.multiChannelCascade[0][7];
        if (element.getType() == FilterElement::POLE
            && sliderThatWasMoved->getValue()
                   == sliderThatWasMoved->getMaximum())
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xffe86d5c));
        else
            sliderThatWasMoved->setColour(juce::Slider::textBoxTextColourId,
                                          juce::Colour(0xff333333));
        sliderThatWasMoved->repaint();

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(8);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(8)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p2_slider.get())
    {
        updateFrequencyFromSlider(p2_slider.get(), p2_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(2);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(2)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p3_slider.get())
    {
        updateFrequencyFromSlider(p3_slider.get(), p3_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(3);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(3)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p4_slider.get())
    {
        updateFrequencyFromSlider(p4_slider.get(), p4_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(4);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(4)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p5_slider.get())
    {
        updateFrequencyFromSlider(p5_slider.get(), p5_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(5);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(5)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p6_slider.get())
    {
        updateFrequencyFromSlider(p6_slider.get(), p6_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(6);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(6)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p7_slider.get())
    {
        updateFrequencyFromSlider(p7_slider.get(), p7_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(7);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(7)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == p8_slider.get())
    {
        updateFrequencyFromSlider(p8_slider.get(), p8_freq.get(), sampleRate);

        if (autoGain.get()->getToggleState() && !isSettingFilters)
        {
            auto gain = calculateGain(8);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(8)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (sliderThatWasMoved == frequency_design_slider.get())
    {
        design_frequency = updateFrequencyFromDesignSlider(
            frequency_design_slider.get(), frequency_label.get(), sampleRate);
        if (type_box->getSelectedId() > 1) setTransitionWidthRange();

        if (autoUpdate) autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == transition_width_slider.get())
    {
        normalisedTransitionWidth = sliderThatWasMoved->getValue();

        if (autoUpdate) autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == passbandAmplitude_slider.get())
    {
        passbandAmplitudedB = sliderThatWasMoved->getValue();
        setTransitionWidthRange();

        if (autoUpdate) autoUpdateCheckAndSetup();
    }
    else if (sliderThatWasMoved == e2_gain.get())
    {
        gain2_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e3_gain.get())
    {
        gain3_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e4_gain.get())
    {
        gain4_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e5_gain.get())
    {
        gain5_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e6_gain.get())
    {
        gain6_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e7_gain.get())
    {
        gain7_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
    else if (sliderThatWasMoved == e8_gain.get())
    {
        gain8_label->setText(juce::String(sliderThatWasMoved->getValue(), 1)
                                 + " dB",
                             NotificationType::dontSendNotification);
    }
}

void EditorComponent::buttonClicked(juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == reset_button.get())
    {
        processor.resetFilter();
    }
    else if (buttonThatWasClicked == e1_type.get())
    {
        auto element = processor.multiChannelCascade[0][0];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(1)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m1_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m1_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(1, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(1)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e2_type.get())
    {
        auto element = processor.multiChannelCascade[0][1];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(2)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m2_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m2_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(2, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(2)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e3_type.get())
    {
        auto element = processor.multiChannelCascade[0][2];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(3)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m3_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m3_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(3, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(3)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e4_type.get())
    {
        auto element = processor.multiChannelCascade[0][3];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(4)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m4_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m4_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(4, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(4)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e5_type.get())
    {
        auto element = processor.multiChannelCascade[0][4];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(5)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m5_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m5_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(5, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(5)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e6_type.get())
    {
        auto element = processor.multiChannelCascade[0][5];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(6)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m6_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m6_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(6, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(6)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e7_type.get())
    {
        auto element = processor.multiChannelCascade[0][6];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(7)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m7_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m7_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(7, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(7)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
        }
    }
    else if (buttonThatWasClicked == e8_type.get())
    {
        auto element = processor.multiChannelCascade[0][7];
        if (element.getType() == FilterElement::ZERO
            && element.getMagnitude() == 1.0)
            Parameters::setParameterValue(
                processor.parameters.getParameter(MAGNITUDE_NAME
                                                  + std::to_string(8)),
                POLE_MAX_MAGNITUDE);

        if (element.getType() == FilterElement::POLE)
            m8_slider->setColour(juce::Slider::textBoxTextColourId,
                                 juce::Colour(0xff333333));
        m8_slider->repaint();

        if (autoGain.get()->getToggleState())
        {
            auto gain = calculateGain(8, true);
            Parameters::setParameterValue(processor.parameters.getParameter(
                                              GAIN_NAME + std::to_string(8)),
                                          jmap(gain, GAIN_FLOOR, GAIN_CEILING,
                                               SLIDERS_FLOOR, SLIDERS_CEILING));
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
        magnitude_response->updateValues(magnitudes, referenceFrequencies,
                                         processor.getSampleRate(), ampDb);
        phase_response->updateValues(phases, referenceFrequencies,
                                     processor.getSampleRate(), true);
    }
    else if (buttonThatWasClicked == update_button.get())
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
        magnitude_response->updateValues(magnitudes, referenceFrequencies,
                                         processor.getSampleRate(), ampDb);
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
    else if (buttonThatWasClicked == bypass.get())
    {
        if (bypass->getToggleState())
        {
            backgroundColour = juce::Colour(0xffbdc0c1);
            activeSwitchesTheme.setBackgroundColour(juce::Colour(0xffbdc0c1));
            autoUpdateSwitchTheme.setBackgroundColour(juce::Colour(0xffbdc0c1));
            repaint();
        }
        else
        {
            backgroundColour = juce::Colour(0xffecf0f1);
            activeSwitchesTheme.setBackgroundColour(
                juce::Colour(SWITCH_BACKGROUND));
            autoUpdateSwitchTheme.setBackgroundColour(
                juce::Colour(SWITCH_BACKGROUND));
            repaint();
        }
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
        auto defaultPresetLocation = File::getSpecialLocation(
            File::SpecialLocationType::commonDocumentsDirectory);
        juce::FileChooser chooser("Select the save location...",
                                  defaultPresetLocation, "*.txt");
        if (chooser.browseForFileToSave(true))
        {
            juce::File file = chooser.getResult();
            juce::FileOutputStream outputStream(file);
            if (outputStream.openedOk())
            {
                outputStream.setPosition(0);
                outputStream.truncate();
                outputStream << "Sample rate: "
                             << juce::String(processor.getSampleRate()) << "\n";

                for (const auto& coeffs : processor.getCoefficients())
                {
                    outputStream << "\n";
                    for (auto c : coeffs)
                        outputStream << juce::String(c) << "\n";
                }
            }
        }
    }
    else if (buttonThatWasClicked == save_preset_button.get())
    {
        auto defaultPresetLocation = File::getSpecialLocation(
            File::SpecialLocationType::commonDocumentsDirectory);
        juce::FileChooser chooser("Select the save location...",
                                  defaultPresetLocation, "*.xml");
        if (chooser.browseForFileToSave(true))
        {
            auto file = chooser.getResult();
            if (file.exists()) file.deleteFile();
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
        auto defaultPresetLocation = File::getSpecialLocation(
            File::SpecialLocationType::commonDocumentsDirectory);
        juce::FileChooser chooser("Select the preset to load...",
                                  defaultPresetLocation, "*.xml");
        if (chooser.browseForFileToOpen())
        {
            auto file = chooser.getResult();
            MemoryBlock sourceData;
            file.loadFileAsData(sourceData);
            processor.setStateInformation(sourceData.getData(),
                                          sourceData.getSize());
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

void EditorComponent::labelTextChanged(juce::Label* labelThatHasChanged)
{
    const double sampleRate = processor.getSampleRate();
    int newFrequency        = labelThatHasChanged->getText().getIntValue();

    float newGain
        = formatGainInput(labelThatHasChanged->getText().getFloatValue());

    if (labelThatHasChanged == frequency_label.get())
        formatDesignFrequencyInput(newFrequency, labelThatHasChanged,
                                   sampleRate);

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
        design_frequency = updateDesignSliderFromFrequency(
            newFrequency, frequency_design_slider.get(), sampleRate);
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

    labelThatHasChanged->setText(juce::String(newGain, 2) + " dB",
                                 NotificationType::dontSendNotification);
}

void EditorComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == type_box.get())
    {
        design_type = comboBoxThatHasChanged->getSelectedId();
        update_button->setEnabled(false);
        if (!design_type) return;
        if (design_type == 1)
            updateGUIButterworth();
        else
        {
            updateGUIEllipticChebyshevIandII();
            if (autoUpdate) autoUpdateCheckAndSetup();
        }
    }
    else if (comboBoxThatHasChanged == shape_box.get())
    {
        design_shape = comboBoxThatHasChanged->getSelectedId();
        updateGUIGivenShape();
        if (autoUpdate) autoUpdateCheckAndSetup();
    }
    else if (comboBoxThatHasChanged == order_box.get())
    {
        design_filters_to_activate = comboBoxThatHasChanged->getSelectedId();
        if (design_filters_to_activate)
        {
            update_button->setEnabled(true);
            if (autoUpdate) autoUpdateCheckAndSetup();
        }
    }
}

void EditorComponent::slidersInit()
{
    auto elements = processor.getFilterElementsChain();

    if (elements[0].getActive())
    {
        m1_slider->setLookAndFeel(&activeSliderTheme);
        p1_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m1_slider->setLookAndFeel(&unactiveSliderTheme);
        p1_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[1].getActive())
    {
        m2_slider->setLookAndFeel(&activeSliderTheme);
        p2_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m2_slider->setLookAndFeel(&unactiveSliderTheme);
        p2_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[2].getActive())
    {
        m3_slider->setLookAndFeel(&activeSliderTheme);
        p3_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m3_slider->setLookAndFeel(&unactiveSliderTheme);
        p3_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[3].getActive())
    {
        m4_slider->setLookAndFeel(&activeSliderTheme);
        p4_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m4_slider->setLookAndFeel(&unactiveSliderTheme);
        p4_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[4].getActive())
    {
        m5_slider->setLookAndFeel(&activeSliderTheme);
        p5_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m5_slider->setLookAndFeel(&unactiveSliderTheme);
        p5_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[5].getActive())
    {
        m6_slider->setLookAndFeel(&activeSliderTheme);
        p6_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m6_slider->setLookAndFeel(&unactiveSliderTheme);
        p6_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[6].getActive())
    {
        m7_slider->setLookAndFeel(&activeSliderTheme);
        p7_slider->setLookAndFeel(&activeSliderTheme);
    }
    else
    {
        m7_slider->setLookAndFeel(&unactiveSliderTheme);
        p7_slider->setLookAndFeel(&unactiveSliderTheme);
    }

    if (elements[7].getActive())
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
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++i)
    {
        auto gain = calculateGain(i);
        Parameters::setParameterValue(
            processor.parameters.getParameter(GAIN_NAME + std::to_string(i)),
            jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR,
                 SLIDERS_CEILING));
    }
}

void EditorComponent::getFrequencyResponse()
{
    double phi;
    const auto sampleRate = processor.getSampleRate();

    auto n1 = log(FREQUENCY_FLOOR / sampleRate);
    auto n2 = log(0.5) - n1;

    std::complex<double> frequencyResponse;

    const double gain = processor.gainProcessor.getGainLinear();

    for (int i = 0; i < GRAPHS_QUALITY; ++i)
    {
        if (linLog)
            phi = static_cast<double>(i)
                  / static_cast<double>(2 * (GRAPHS_QUALITY - 1));
        else
            phi = exp(n1
                      + (n2
                         * (static_cast<double>(i)
                            / (static_cast<double>(GRAPHS_QUALITY - 1)))));
        phi *= juce::MathConstants<double>::twoPi;

        frequencyResponse = processor.multiChannelCascade[0].dtft(phi);
        magnitudes[i]     = gain * std::abs(frequencyResponse);
        phases[i]
            = (juce::MathConstants<double>::pi + std::arg(frequencyResponse))
              / juce::MathConstants<double>::twoPi;
    }
}

void EditorComponent::updateReferenceFrequencies()
{
    double phi;
    const auto sampleRate = processor.getSampleRate();

    auto n1 = log(FREQUENCY_FLOOR / sampleRate);
    auto n2 = log(0.5) - log(FREQUENCY_FLOOR / sampleRate);

    int k = 0;

    for (int i = 0; i < GRAPHS_QUALITY; ++i)
    {
        if (linLog)
            phi = static_cast<double>(i)
                  / static_cast<double>(
                      2 * (GRAPHS_QUALITY - 1));  // Linear spectrum
        else
            phi = exp(n1
                      + (n2
                         * (static_cast<double>(i)
                            / (static_cast<double>(GRAPHS_QUALITY
                                                   - 1)))));  // Log spectrum

        if (!(i % (GRAPHS_QUALITY / NUMBER_OF_REFERENCE_FREQUENCIES)))
        {
            referenceFrequencies[k] = phi;
            ++k;
        }
    }
}

void EditorComponent::updateElements()
{
    e1->updateElement(processor.multiChannelCascade[0][0], 1,
                      gaussian_plane.get(), &processor);
    e2->updateElement(processor.multiChannelCascade[0][1], 2,
                      gaussian_plane.get(), &processor);
    e3->updateElement(processor.multiChannelCascade[0][2], 3,
                      gaussian_plane.get(), &processor);
    e4->updateElement(processor.multiChannelCascade[0][3], 4,
                      gaussian_plane.get(), &processor);
    e5->updateElement(processor.multiChannelCascade[0][4], 5,
                      gaussian_plane.get(), &processor);
    e6->updateElement(processor.multiChannelCascade[0][5], 6,
                      gaussian_plane.get(), &processor);
    e7->updateElement(processor.multiChannelCascade[0][6], 7,
                      gaussian_plane.get(), &processor);
    e8->updateElement(processor.multiChannelCascade[0][7], 8,
                      gaussian_plane.get(), &processor);
}

void EditorComponent::updateFrequencyFromSlider(juce::Slider* slider,
                                                juce::Label* label,
                                                double sampleRate)
{
    double sliderValue = slider->getValue();
    int frequency      = std::ceil((sliderValue * sampleRate) / 2.0);

    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);
}

void EditorComponent::updateSliderFromFrequency(int frequency,
                                                juce::Slider* slider,
                                                double sampleRate)
{
    double sliderValue = (frequency * 2.0) / sampleRate;
    slider->setValue(sliderValue, juce::sendNotificationSync);
}

double EditorComponent::updateFrequencyFromDesignSlider(juce::Slider* slider,
                                                        juce::Label* label,
                                                        double sampleRate)
{
    double sliderValue = slider->getValue();
    int frequency      = std::ceil((sliderValue * sampleRate) / 2000.0);
    label->setText(juce::String(frequency) + " Hz", juce::dontSendNotification);

    return frequency;
}

double EditorComponent::updateDesignSliderFromFrequency(int frequency,
                                                        juce::Slider* slider,
                                                        double sampleRate)
{
    double sliderValue = (frequency * 2000.0) / sampleRate;
    slider->setValue(sliderValue, juce::sendNotificationSync);

    return std::ceil((slider->getValue() * sampleRate) / 2000.0);
}

void EditorComponent::formatFrequencyInput(int& frequency, juce::Label* label,
                                           double sampleRate)
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

void EditorComponent::formatDesignFrequencyInput(int& frequency,
                                                 juce::Label* label,
                                                 double sampleRate)
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
        design_frequency_label->setBounds(1058, 243, 72, 20);
        frequency_design_slider->setBounds(1027, 265, 83, 25);
        frequency_design_slider->setRange(0.0001, 999, 0.0001);
        updateDesignSliderFromFrequency(
            design_frequency, frequency_design_slider.get(), sampleRate);
        frequency_label->setBounds(1110, 265, 60, 25);
    }
    break;

    case 2:
    {
        design_frequency_label->setBounds(1058, 183, 72, 20);
        frequency_design_slider->setBounds(1027, 205, 83, 25);
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
    }
    break;
    }
}

void EditorComponent::setTransitionWidthRange()
{
    double sampleRate          = processor.getSampleRate();
    double normalisedFrequency = design_frequency / sampleRate;
    double minValue            = 0.0001;
    double maxValue            = jmin(2 * normalisedFrequency - minValue,
                                      2 * (0.5 - normalisedFrequency) - minValue);
    const double interval      = 0.00001;
    const int maxOrder         = 8;

    auto Gp = Decibels::decibelsToGain(passbandAmplitudedB, -300.0);
    auto Gs = Decibels::decibelsToGain(stopbandAmplitudedB, -300.0);

    auto epsp = std::sqrt(1.0 / (Gp * Gp) - 1.0);
    auto epss = std::sqrt(1.0 / (Gs * Gs) - 1.0);

    if (design_type == 2 || design_type == 3)
    {
        double X = std::acosh(epss / epsp);

        double Y
            = std::acosh(std::tan(MathConstants<double>::pi
                                  * (normalisedFrequency + minValue / 2))
                         / std::tan(MathConstants<double>::pi
                                    * (normalisedFrequency - minValue / 2)));

        while (roundToInt(std::ceil(X / Y)) > maxOrder)
        {
            minValue += interval;
            Y = acosh(std::tan(MathConstants<double>::pi
                               * (normalisedFrequency + minValue / 2))
                      / std::tan(MathConstants<double>::pi
                                 * (normalisedFrequency - minValue / 2)));
        }
    }
    else
    {
        double K, Kp, K1, K1p;

        auto k1 = epsp / epss;
        juce::dsp::SpecialFunctions::ellipticIntegralK(k1, K1,
                                                       K1p);  // Rimane fisso

        auto k = std::tan(MathConstants<double>::pi
                          * std::tan(MathConstants<double>::pi
                                     * (normalisedFrequency - minValue / 2)))
                 / std::tan(MathConstants<double>::pi
                            * std::tan(MathConstants<double>::pi
                                       * (normalisedFrequency + minValue / 2)));
        juce::dsp::SpecialFunctions::ellipticIntegralK(k, K, Kp);

        while (roundToInt(std::ceil((K1p * K) / (K1 * Kp))) > maxOrder)
        {
            minValue += interval;
            auto k
                = std::tan(MathConstants<double>::pi
                           * std::tan(MathConstants<double>::pi
                                      * (normalisedFrequency - minValue / 2)))
                  / std::tan(
                      MathConstants<double>::pi
                      * std::tan(MathConstants<double>::pi
                                 * (normalisedFrequency + minValue / 2)));
            juce::dsp::SpecialFunctions::ellipticIntegralK(k, K, Kp);
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
            ++i;
        }
    }
}

void EditorComponent::updateGUIButterworth()
{
    update_button->setEnabled(false);

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
        int int_order   = order.getIntValue();
        int attenuation = 6 * int_order;
        juce::String stringToVisualize
            = order + "  (-" + juce::String(attenuation) + " db / octave)";
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

    frequency_label->setBounds(1110, 205, 60, 25);

    passbandAmplitude_label->setVisible(true);
    passbandAmplitude_label->setBounds(1020, 236, 159, 20);

    passbandAmplitude_slider->setVisible(true);
    passbandAmplitude_slider->setBounds(1027, 260, 135, 25);

    stopbandAmplitude_label->setVisible(true);
    stopbandAmplitude_label->setBounds(1022, 291, 159, 20);

    stopbandAmplitude_slider->setVisible(true);
    stopbandAmplitude_slider->setBounds(1027, 315, 135, 25);

    transition_width_label->setVisible(true);
    transition_width_label->setBounds(1030, 346, 129, 20);

    transition_width_slider->setVisible(true);
    transition_width_slider->setBounds(1027, 370, 135, 25);

    update_button->setEnabled(true);
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
    auto element   = processor.multiChannelCascade[0][elementNr - 1];
    auto magnitude = element.getMagnitude();
    auto type      = element.getType();

    if (isChangingType)
        type = (type == FilterElement::ZERO) ? FilterElement::POLE
                                             : FilterElement::ZERO;

    auto Re = element.getMagnitude()
              * std::cos(element.getPhase() * MathConstants<double>::pi);

    switch (type)
    {
    case FilterElement::ZERO:
    {
        auto gainValue
            = std::sqrt(1.0
                        / (1.0 + 4 * Re * Re
                           + magnitude * magnitude * magnitude * magnitude));
        return static_cast<float>(
            Decibels::gainToDecibels(gainValue, GAIN_FLOOR - 1.0));
    }

    case FilterElement::POLE:
    {
        auto Im = element.getMagnitude()
                  * std::sin(element.getPhase() * MathConstants<double>::pi);
        auto MSG = 1.0
                   / (std::abs(1.0 - magnitude * magnitude)
                      * (Re * Re - 2 * std::abs(Re) + Im * Im + 1.0));
        auto gainValue = 1.0 / std::sqrt(MSG);
        return static_cast<float>(
            Decibels::gainToDecibels(gainValue, GAIN_FLOOR - 1.0));
    }
    }
}

void EditorComponent::coefficientsNormalization(double* c0, double* c1,
                                                double* c2)
{
    *c1 /= (*c0);
    if (c2 != nullptr) *c2 /= (*c0);
}

void EditorComponent::fromCoefficientsToMagnitudeAndPhase(double& mg,
                                                          double& ph, double c1,
                                                          double c2)
{
    mg = sqrt(c2);
    ph = (1 / MathConstants<double>::pi) * acos(-c1 / (2 * mg));
}

void EditorComponent::filterDesignAndSetup()
{
    const double sampleRate = processor.getSampleRate();
    const int order         = design_filters_to_activate;

    juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<double>>
        iirCoefficients;

    double cutoff = design_frequency;

    if (design_shape == 2) cutoff = sampleRate * 0.5 - design_frequency;

    switch (design_type)
    {
    case 1:
    {
        iirCoefficients = juce::dsp::FilterDesign<
            double>::designIIRLowpassHighOrderButterworthMethod(cutoff,
                                                                sampleRate,
                                                                order);
    }
    break;

    case 2:
    {
        iirCoefficients = juce::dsp::FilterDesign<double>::
            designIIRLowpassHighOrderChebyshev1Method(
                cutoff, sampleRate, normalisedTransitionWidth,
                passbandAmplitudedB, stopbandAmplitudedB);
    }
    break;

    case 3:
    {
        iirCoefficients = juce::dsp::FilterDesign<double>::
            designIIRLowpassHighOrderChebyshev2Method(
                cutoff, sampleRate, normalisedTransitionWidth,
                passbandAmplitudedB, stopbandAmplitudedB);
    }
    break;

    case 4:
    {
        iirCoefficients = juce::dsp::FilterDesign<double>::
            designIIRLowpassHighOrderEllipticMethod(
                cutoff, sampleRate, normalisedTransitionWidth,
                passbandAmplitudedB, stopbandAmplitudedB);
    }
    break;
    }

    double b0, b1, b2, a1, a2;
    double magnitude, phase;
    int elementNr = 1;

    processor.setBypass(true);
    isSettingFilters = true;
    autoGain->setToggleState(false, NotificationType::sendNotificationSync);

    for (int i = 0; i < iirCoefficients.size(); ++i)
    {
        const auto& coeffs = iirCoefficients[i];

        if (coeffs->coefficients.size() == 3)
        {
            std::complex<double> zero;
            std::complex<double> pole;

            b0 = coeffs->coefficients[0];
            b1 = coeffs->coefficients[1];
            a1 = coeffs->coefficients[2];

            if (design_shape == 2)
            {
                zero = -((-b1) / b0);
                pole = a1;
            }
            else
            {
                zero = -(b1 / b0);
                pole = -a1;
            }

            processor.setFilter(std::abs(zero), std::arg(zero),
                                FilterElement::ZERO, elementNr, b0);
            ++elementNr;

            processor.setFilter(std::abs(pole), std::arg(pole),
                                FilterElement::POLE, elementNr, b0);
            ++elementNr;
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
                b1 = -b1;
                a1 = -a1;
            }

            coefficientsNormalization(&b0, &b1, &b2);

            fromCoefficientsToMagnitudeAndPhase(magnitude, phase, b1, b2);
            std::complex<double> zero = std::polar(magnitude, phase);
            processor.setFilter(std::abs(zero), std::arg(zero),
                                FilterElement::ZERO, elementNr, b0);

            ++elementNr;

            fromCoefficientsToMagnitudeAndPhase(magnitude, phase, a1, a2);
            std::complex<double> pole = std::polar(magnitude, phase);
            processor.setFilter(std::abs(pole), std::arg(pole),
                                FilterElement::POLE, elementNr);
            ++elementNr;
        }
    }

    for (; elementNr <= NUMBER_OF_FILTER_ELEMENTS; ++elementNr)
        processor.setUnactive(elementNr);

    processor.setBypass(false);
    isSettingFilters = false;
}

void EditorComponent::autoUpdateCheckAndSetup()
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

WrappedEditor::WrappedEditor(PolesAndZerosEQAudioProcessor& p,
                             AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(p), editorComponent(p, vts)
{
    addAndMakeVisible(editorComponent);

    PropertiesFile::Options options;
    options.applicationName     = ProjectInfo::projectName;
    options.commonToAllUsers    = true;
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Application Support";

    applicationProperties.setStorageParameters(options);

    if (auto* constrainer = getConstrainer())
    {
        constrainer->setFixedAspectRatio(static_cast<double>(originalWidth)
                                         / static_cast<double>(originalHeight));
        constrainer->setSizeLimits(originalWidth * 0.25, originalHeight * 0.25,
                                   originalWidth + originalWidth * 0.20,
                                   originalHeight + originalHeight * 0.20);
    }

    auto sizeRatio = 1.0;
    if (auto* properties = applicationProperties.getCommonSettings(true))
        sizeRatio = properties->getDoubleValue("sizeRatio", 1.0);

    setResizable(true, true);
    setSize(static_cast<int>(originalWidth * sizeRatio),
            static_cast<int>(originalHeight * sizeRatio));
}

void WrappedEditor::resized()
{
    const auto scaleFactor = static_cast<float>(getWidth()) / originalWidth;
    if (auto* properties = applicationProperties.getCommonSettings(true))
        properties->setValue("sizeRatio", scaleFactor);

    editorComponent.setTransform(AffineTransform::scale(scaleFactor));
    editorComponent.setBounds(0, 0, originalWidth, originalHeight);
}
