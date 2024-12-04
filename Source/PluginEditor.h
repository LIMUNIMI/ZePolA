#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphs.h"
#include "GraphicTheme.h"
#include "Parameters.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class EditorComponent  :            public juce::Component,
                                    public juce::Slider::Listener,
                                    public juce::Button::Listener,
                                    public juce::Label::Listener,
                                    public juce::ComboBox::Listener,
                                    public juce::Timer
{
public:
    
    EditorComponent (PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts);
    ~EditorComponent() override;

    void slidersInit();
    void gainsInit();
    
    void getFrequencyResponse ();
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

    float formatGainInput(float gain);
    float calculateGain (const int elementNr, bool isChangingType = false);

    void coefficientsNormalization(double* c0, double* c1, double* c2 = nullptr);
    void fromCoefficientsToMagnitudeAndPhase(double& mg, double& ph, double c1, double c2);
    void filterDesignAndSetup();
    void autoUpdateCheckAndSetup();

    void paint (juce::Graphics& g) override;
    void sliderValueChanged (juce::Slider* sliderThatWasMoved) override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;
    void labelTextChanged (juce::Label* labelThatHasChanged) override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;
    
    void timerCallback() override;

    // Binary resources:
    static const char* clockwise_arrow_png;
    static const int clockwise_arrow_pngSize;
    static const char* load_icon_png;
    static const int load_icon_pngSize;
    static const char* save_icon_png;
    static const int save_icon_pngSize;


private:
    // Generic variables
    PolesAndZerosEQAudioProcessor& processor;
    AudioProcessorValueTreeState& valueTreeState;

    // Frequency response variables
    double magnitudes[GRAPHS_QUALITY];
    double phases[GRAPHS_QUALITY];
    double referenceFrequencies[NUMBER_OF_REFERENCE_FREQUENCIES];
    bool linLog = false;
    bool ampDb = false;
    
    // Filter design variables
    int design_type = -1;
    int design_shape;
    int design_filters_to_activate;
    int design_frequency;
    double normalisedTransitionWidth;
    double passbandAmplitudedB = -0.1;
    double stopbandAmplitudedB = -21;
    bool autoUpdate = false;
    std::vector<juce::String> selectable_filter_types;
    std::vector<juce::String> selectable_orders_butterworth;
    
    // Warning and protection variables
    bool isSettingFilters = false;
    bool isUserWarned = false;

    std::vector<std::unique_ptr<SliderAttachment>> magnitudesAttachments;
    std::vector<std::unique_ptr<SliderAttachment>> phasesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> typesAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> activeAttachments;
    std::vector<std::unique_ptr<SliderAttachment>> gainsAttachments;
    std::unique_ptr<SliderAttachment> masterGainAttachment;
    std::unique_ptr<ButtonAttachment> bypassAttachment;
    std::unique_ptr<ButtonAttachment> linLogAttachment;

    ActiveSliderTheme activeSliderTheme;
    UnactiveSliderTheme unactiveSliderTheme;
    MasterGainSliderTheme masterGainSliderTheme;

    TypeSwitchTheme typeSwitchesTheme;
    LinLogSwitchTheme linLogTheme;
    AmpDbSwitchTheme ampDbTheme;

    ActiveSwitchTheme activeSwitchesTheme;
    GenericButtonTheme updateButtonTheme;

    GenericButtonTheme autoGainButtonTheme;
    GenericButtonTheme turnAllOnTheme;
    GenericButtonTheme turnAllOffTheme;
    GenericButtonTheme multiplyPhasesTheme;
    GenericButtonTheme dividePhasesTheme;
    GenericButtonTheme swapButtonTheme;

    MenuButtonTheme undoButtonTheme;
    MenuButtonTheme redoButtonTheme;
    MenuButtonTheme resetButtonTheme;

    MenuButtonTheme saveCoefficientsButtonTheme;
    MenuButtonTheme savePresetButtonTheme;
    MenuButtonTheme loadPresetButtonTheme;

    ComboBoxTheme comboBoxTheme;

    AutoUpdateSwitchTheme autoUpdateSwitchTheme;
    
    GenericButtonTheme resetSafetyFlagButtonTheme;
    
    // Magnitude sliders
    std::shared_ptr<CustomSlider> m1_slider;
    std::shared_ptr<CustomSlider> m2_slider;
    std::shared_ptr<CustomSlider> m3_slider;
    std::shared_ptr<CustomSlider> m4_slider;
    std::shared_ptr<CustomSlider> m5_slider;
    std::shared_ptr<CustomSlider> m6_slider;
    std::shared_ptr<CustomSlider> m7_slider;
    std::shared_ptr<CustomSlider> m8_slider;
    
    // Phase sliders
    std::unique_ptr<CustomSlider> p1_slider;
    std::unique_ptr<CustomSlider> p2_slider;
    std::unique_ptr<CustomSlider> p3_slider;
    std::unique_ptr<CustomSlider> p4_slider;
    std::unique_ptr<CustomSlider> p5_slider;
    std::unique_ptr<CustomSlider> p6_slider;
    std::unique_ptr<CustomSlider> p7_slider;
    std::unique_ptr<CustomSlider> p8_slider;
    
    // Frequency labels
    std::unique_ptr<CustomLabel> p1_freq;
    std::unique_ptr<CustomLabel> p2_freq;
    std::unique_ptr<CustomLabel> p3_freq;
    std::unique_ptr<CustomLabel> p4_freq;
    std::unique_ptr<CustomLabel> p5_freq;
    std::unique_ptr<CustomLabel> p6_freq;
    std::unique_ptr<CustomLabel> p7_freq;
    std::unique_ptr<CustomLabel> p8_freq;
    
    // Type switches
    std::unique_ptr<CustomToggleButton> e1_type;
    std::unique_ptr<CustomToggleButton> e2_type;
    std::unique_ptr<CustomToggleButton> e3_type;
    std::unique_ptr<CustomToggleButton> e4_type;
    std::unique_ptr<CustomToggleButton> e5_type;
    std::unique_ptr<CustomToggleButton> e6_type;
    std::unique_ptr<CustomToggleButton> e7_type;
    std::unique_ptr<CustomToggleButton> e8_type;
    
    // Active switches
    std::unique_ptr<CustomToggleButton> e1_active;
    std::unique_ptr<CustomToggleButton> e2_active;
    std::unique_ptr<CustomToggleButton> e3_active;
    std::unique_ptr<CustomToggleButton> e4_active;
    std::unique_ptr<CustomToggleButton> e5_active;
    std::unique_ptr<CustomToggleButton> e6_active;
    std::unique_ptr<CustomToggleButton> e7_active;
    std::unique_ptr<CustomToggleButton> e8_active;
    
    // Gain sliders & labels
    std::unique_ptr<CustomSlider> e1_gain;
    std::unique_ptr<CustomSlider> e2_gain;
    std::unique_ptr<CustomSlider> e3_gain;
    std::unique_ptr<CustomSlider> e4_gain;
    std::unique_ptr<CustomSlider> e5_gain;
    std::unique_ptr<CustomSlider> e6_gain;
    std::unique_ptr<CustomSlider> e7_gain;
    std::unique_ptr<CustomSlider> e8_gain;
    
    std::unique_ptr<DraggableGainLabel> gain1_label;
    std::unique_ptr<DraggableGainLabel> gain2_label;
    std::unique_ptr<DraggableGainLabel> gain3_label;
    std::unique_ptr<DraggableGainLabel> gain4_label;
    std::unique_ptr<DraggableGainLabel> gain5_label;
    std::unique_ptr<DraggableGainLabel> gain6_label;
    std::unique_ptr<DraggableGainLabel> gain7_label;
    std::unique_ptr<DraggableGainLabel> gain8_label;
    
    // Labels
    std::unique_ptr<juce::Label> radius_label;
    std::unique_ptr<juce::Label> angles_label;
    std::unique_ptr<juce::Label> type_label;
    std::unique_ptr<juce::Label> active_label;
    std::unique_ptr<juce::Label> gain_label;
    
    // Gaussian plane & draggable elements
    std::unique_ptr<GaussianPlane> gaussian_plane;
    std::unique_ptr<juce::Label> gaussian_plane_label;
    std::unique_ptr<DraggableElement> e1;
    std::unique_ptr<DraggableElement> e2;
    std::unique_ptr<DraggableElement> e3;
    std::unique_ptr<DraggableElement> e4;
    std::unique_ptr<DraggableElement> e5;
    std::unique_ptr<DraggableElement> e6;
    std::unique_ptr<DraggableElement> e7;
    std::unique_ptr<DraggableElement> e8;
    
    // Setup shortchuts & controls
    std::unique_ptr<CustomToggleButton> autoGain;
    std::unique_ptr<CustomButton> turn_on_button;
    std::unique_ptr<CustomButton> turn_off_button;
    std::unique_ptr<CustomButton> multiply_phases_button;
    std::unique_ptr<CustomButton> divide_phases_button;
    std::unique_ptr<CustomButton> swap_button;
    
    // Frequency response
    std::unique_ptr<MagnitudeResponse> magnitude_response;
    std::unique_ptr<juce::Label> mg_response_label;
    
    std::unique_ptr<CustomToggleButton> ampDb_switch;
    std::unique_ptr<CustomToggleButton> linLog_switch;
    
    std::unique_ptr<PhaseResponse> phase_response;
    std::unique_ptr<juce::Label> ph_response_label;
    
    // Filter design
    std::unique_ptr<CustomComboBox> shape_box;
    std::unique_ptr<CustomComboBox> type_box;
    std::unique_ptr<CustomComboBox> order_box;
    
    std::unique_ptr<juce::Label> design_frequency_label;
    std::unique_ptr<CustomSlider> frequency_design_slider;
    std::unique_ptr<CustomLabel> frequency_label;
    
    std::unique_ptr<juce::Label> passbandAmplitude_label;
    std::unique_ptr<CustomSlider> passbandAmplitude_slider;
    
    std::unique_ptr<juce::Label> stopbandAmplitude_label;
    std::unique_ptr<CustomSlider> stopbandAmplitude_slider;
    
    std::unique_ptr<juce::Label> transition_width_label;
    std::unique_ptr<CustomSlider> transition_width_slider;
    
    std::unique_ptr<CustomToggleButton> autoUpdate_button;
    std::unique_ptr<CustomButton> update_button;

    // Controls
    std::unique_ptr<CustomSlider> masterGain_slider;
    std::unique_ptr<CustomToggleButton> bypass;
    
    std::unique_ptr<juce::Label> warning_label;
    std::unique_ptr<WarningRectangle> warningRectangle;
    
    // Menu
    std::unique_ptr<CustomButton> undo_button;
    std::unique_ptr<CustomButton> redo_button;
    std::unique_ptr<CustomButton> reset_button;
    std::unique_ptr<CustomButton> saveCoefficients_button;
    std::unique_ptr<CustomButton> save_preset_button;
    std::unique_ptr<CustomButton> load_preset_button;
    
    // Images
    std::unique_ptr<juce::Drawable> anticlockwise_arrow_svg;
    std::unique_ptr<juce::Drawable> clockwise_arrow_svg;
    std::unique_ptr<juce::Drawable> load_icon_svg;
    juce::Image cachedImage_save_icon_png_4;
    
    juce::Colour backgroundColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorComponent)
};

class WrappedEditor : public juce::AudioProcessorEditor
{
    public:
    WrappedEditor(PolesAndZerosEQAudioProcessor& p, AudioProcessorValueTreeState& vts);
    void resized() override;
    
    private:
    
    static constexpr int originalWidth = 1200;
    static constexpr int originalHeight = 790;
    
    EditorComponent editorComponent;
    ApplicationProperties applicationProperties;
};
