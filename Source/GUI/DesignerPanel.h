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
    DesignerPanel(PolesAndZerosEQAudioProcessor&, juce::ApplicationProperties&);
    ~DesignerPanel();

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
