#pragma once
#include "../DSP/FilterDesign.h"
#include "../PluginProcessor.h"
#include "ApplicationPropertiesListeners.h"
#include "CustomButtons.h"
#include <JuceHeader.h>

// =============================================================================
/** Filter design panel  */
class DesignerPanel : public juce::GroupComponent, public SampleRateListener
{
public:
    // =========================================================================
    class CBoxListener : public juce::ComboBox::Listener
    {
    public:
        //======================================================================
        CBoxListener(std::function<void(int)>);

        //======================================================================
        virtual void comboBoxChanged(juce::ComboBox*) override;

    private:
        //======================================================================
        std::function<void(int)> callback;

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CBoxListener)
    };

    // =========================================================================
    class SliderListener : public juce::Slider::Listener
    {
    public:
        //======================================================================
        SliderListener(std::function<void(double)>);

        //======================================================================
        virtual void sliderValueChanged(juce::Slider*) override;

    private:
        //======================================================================
        std::function<void(double)> callback;

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderListener)
    };

    // =========================================================================
    class ButtonListener : public juce::Button::Listener
    {
    public:
        //======================================================================
        ButtonListener(std::function<void(bool)>);

        //======================================================================
        virtual void buttonClicked(juce::Button*) override;
        virtual void buttonStateChanged(juce::Button*) override;

    private:
        //======================================================================
        std::function<void(bool)> callback;

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonListener)
    };

    // =========================================================================
    DesignerPanel(PolesAndZerosEQAudioProcessor&, juce::ApplicationProperties&);
    ~DesignerPanel();

    // =========================================================================
    /** Set filter type from ComboBox Id (+1 offset) */
    void setTypeFromCBoxId(int);
    /** Set filter shape from ComboBox Id (+1 offset) */
    void setShapeFromCBoxId(int);
    /** Set filter order */
    void setOrder(double);
    /** Set filter cutoff frequency */
    void setCutoff(double);
    /** Set the ripple amount (dB) in the passband */
    void setPassbandRipple(double);
    /** Set the ripple amount (dB) in the stopband */
    void setStopbandRipple(double);
    /** Set the designer in automatic (or manual) mode */
    void setAuto(bool);

    // =========================================================================
    virtual void sampleRateChangedCallback(double);
    void resized() override;

private:
    // =========================================================================
    /** Update visibility of passband ripple slider */
    void updatePassbandRippleVisibility();
    /** Update visibility of stopband ripple slider */
    void updateStopbandRippleVisibility();

    // =========================================================================
    void autoDesignFilter();
    void designFilter();
    void applyFilterElement(int i, std::complex<double>, bool type,
                            double gain);

    // =========================================================================
    std::unique_ptr<ApplicationPropertiesButtonAttachment> autoButtonAttachment;
    std::unique_ptr<ApplicationPropertiesComboBoxAttachment> typeCBoxAttachment,
        shapeCBoxAttachment;
    std::unique_ptr<ApplicationPropertiesSliderAttachment>
        orderSliderAttachment, cutoffSliderAttachment, rpSliderAttachment,
        rsSliderAttachment;
    CBoxListener typeCBoxListener, shapeCBoxListener;
    SliderListener orderSliderListener, cutoffSliderListener, rpSliderListener,
        rsSliderListener;
    ButtonListener autoButtonListener;
    PolesAndZerosEQAudioProcessor& processor;
    juce::Label panelLabel, orderLabel, cutoffLabel, rpLabel, rsLabel;
    std::shared_ptr<juce::ComboBox> typeCBox, shapeCBox;
    std::shared_ptr<juce::Slider> orderSlider, cutoffSlider, rpSlider, rsSlider;
    std::shared_ptr<juce::ToggleButton> autoButton;
    juce::TextButton applyButton;
    FilterParameters filterParams;
    bool autoUpdate;
    juce::ApplicationProperties& applicationProperties;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
