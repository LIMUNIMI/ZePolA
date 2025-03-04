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

    // =========================================================================
    virtual void sampleRateChangedCallback(double);
    void resized() override;

private:
    // =========================================================================
    void designFilter();
    void applyFilterElement(int i, std::complex<double>, FilterElement::Type,
                            double gain);

    // =========================================================================
    std::unique_ptr<ApplicationPropertiesButtonAttachment> autoButtonAttachment;
    std::unique_ptr<ApplicationPropertiesComboBoxAttachment> typeCBoxAttachment,
        shapeCBoxAttachment;
    std::unique_ptr<ApplicationPropertiesSliderAttachment>
        orderSliderAttachment, cutoffSliderAttachment;
    CBoxListener typeCBoxListener, shapeCBoxListener;
    SliderListener orderSliderListener, cutoffSliderListener;
    PolesAndZerosEQAudioProcessor& processor;
    juce::Label panelLabel, typeLabel, shapeLabel, orderLabel, cutoffLabel;
    std::shared_ptr<juce::ComboBox> typeCBox, shapeCBox;
    std::shared_ptr<juce::Slider> orderSlider, cutoffSlider;
    std::shared_ptr<juce::ToggleButton> autoButton;
    juce::TextButton applyButton;
    FilterParameters filterParams;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
