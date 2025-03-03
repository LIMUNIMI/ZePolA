#pragma once
#include "../Parameters.h"
#include "ApplicationPropertiesListeners.h"
#include "CustomButtons.h"
#include <JuceHeader.h>

// =============================================================================
/** Filter design panel  */
class DesignerPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    DesignerPanel(VTSAudioProcessor&, juce::ApplicationProperties&);
    ~DesignerPanel();

    // =========================================================================
    void resized() override;

private:
    // =========================================================================
    std::unique_ptr<ApplicationPropertiesButtonAttachment> autoButtonAttachment;
    VTSAudioProcessor& processor;
    juce::Label panelLabel;
    juce::ComboBox typeCBox, shapeCBox;
    juce::Slider orderSlider, cutoffSlider;
    std::shared_ptr<juce::ToggleButton> autoButton;
    juce::TextButton applyButton;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
