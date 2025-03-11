#pragma once
#include "GUI/DesignerPanel.h"
#include "GUI/LookAndFeel.h"
#include "GUI/MasterPanel.h"
#include "GUI/ParameterPanel.h"
#include "GUI/PlotsPanel.h"
#include "GUI/TopMenuPanel.h"
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
    std::unique_ptr<TopMenuPanel> topMenuPanel;
    std::unique_ptr<ParameterPanel> parameterPanel;
    std::unique_ptr<PlotsPanel> plotsPanel;
    std::unique_ptr<DesignerPanel> designerPanel;
    std::unique_ptr<MasterPanel> masterPanel;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PolesAndZerosEQAudioProcessorEditor);
};
