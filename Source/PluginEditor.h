#pragma once
#include "GUI/DesignerPanel.h"
#include "GUI/LookAndFeel.h"
#include "GUI/ParameterPanel.h"
#include "GUI/PlotsPanel.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

// =============================================================================
/** Plugin editor for poles and zero eq  */
class PolesAndZerosEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    // ===========================================================================
    PolesAndZerosEQAudioProcessorEditor(PolesAndZerosEQAudioProcessor&);
    ~PolesAndZerosEQAudioProcessorEditor();

    //===================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // ===========================================================================
    PolesAndZerosEQAudioProcessor& processor;

    // ===========================================================================
    juce::ComponentBoundsConstrainer aspectRatioConstrainer;
    juce::ApplicationProperties applicationProperties;
    CustomLookAndFeel claf;

    // ===========================================================================
    juce::GroupComponent masterGroup;
    std::unique_ptr<ParameterPanel> parameterPanel;
    std::unique_ptr<PlotsPanel> plotsPanel;
    std::unique_ptr<DesignerPanel> designerPanel;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PolesAndZerosEQAudioProcessorEditor);
};
