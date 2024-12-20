#pragma once
#include "PluginProcessor.h"
#include <JuceHeader.h>

// =============================================================================
/** Plugin editor for poles and zero eq  */
class PolesAndZerosEQAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    // ===========================================================================
    PolesAndZerosEQAudioProcessorEditor(PolesAndZerosEQAudioProcessor&);

    //===================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // ===========================================================================
    /** Setup function for size-related settings */
    void sizeSetup();

    // ===========================================================================
    PolesAndZerosEQAudioProcessor& processor;

    // ===========================================================================
    juce::ApplicationProperties applicationProperties;
    juce::ComponentBoundsConstrainer aspectRatioConstrainer;
    static const int originalWidth;
    static const int originalHeight;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PolesAndZerosEQAudioProcessorEditor);
};
