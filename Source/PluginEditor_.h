#pragma once
#include "GUIElements/Palette.h"
#include "PluginProcessor.h"
#include <JuceHeader.h>

// =============================================================================
/** Rectangular panel for the GUI  */
class Panel : public juce::Component
{
public:
    // ===========================================================================
    Panel();

    //===================================================================
    void paint(juce::Graphics&) override;

private:
    // ===========================================================================
    PolesAndZerosEQColourScheme palette;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Panel);
};

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
    juce::Colour getColour(PolesAndZerosEQColourScheme::Label);
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
    juce::ApplicationProperties applicationProperties;
    juce::ComponentBoundsConstrainer aspectRatioConstrainer;
    PolesAndZerosEQColourScheme palette;
    static const int originalWidth;
    static const int originalHeight;
    float sizeRatio;
    Panel slidersPanel, plotsPanel, designPanel, masterPanel;

    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        PolesAndZerosEQAudioProcessorEditor);
};
