#pragma once
#include "../PluginProcessor.h"
#include "ApplicationPropertiesListeners.h"
#include "CustomButtons.h"
#include <JuceHeader.h>

// =============================================================================
/** Filter design panel  */
class DesignerPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    DesignerPanel(PolesAndZerosEQAudioProcessor&, juce::ApplicationProperties&);
    ~DesignerPanel();

    // =========================================================================
    void resized() override;

private:
    // =========================================================================
    std::unique_ptr<ApplicationPropertiesButtonAttachment> autoButtonAttachment;
    PolesAndZerosEQAudioProcessor& processor;
    juce::Label panelLabel, typeLabel, shapeLabel, orderLabel, cutoffLabel;
    juce::ComboBox typeCBox, shapeCBox;
    juce::Slider orderSlider, cutoffSlider;
    std::shared_ptr<juce::ToggleButton> autoButton;
    juce::TextButton applyButton;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
