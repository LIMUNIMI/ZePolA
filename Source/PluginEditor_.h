#pragma once
#include "GUI/LookAndFeel.h"
#include "GUI/Settings.h"
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

    //===================================================================
    /** Get colour from palette */
    juce::Colour getColour(GUISettings::ColourLabel);
    /** Get value from settings */
    float getValue(GUISettings::SettingLabel);
    /** Get rectangle corner size */
    float getRectCornerSize();
    /** Get rectangle corner size */
    float getRectThickness();

private:
    // ===========================================================================
    /** Setup function for size-related settings */
    void sizeSetup();

    // ===========================================================================
    PolesAndZerosEQAudioProcessor& processor;

    // ===========================================================================
    juce::GroupComponent slidersGroup, plotsGroup, designGroup, masterGroup;

    // ===========================================================================
    juce::ComponentBoundsConstrainer aspectRatioConstrainer;
    juce::ApplicationProperties applicationProperties;
    CustomLookAndFeel claf;
    GUISettings settings;
    float sizeRatio;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PolesAndZerosEQAudioProcessorEditor);
};
