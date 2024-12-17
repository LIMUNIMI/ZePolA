#pragma once
#include "Filter.h"
#include <JuceHeader.h>

// =============================================================================
/** Plugin processor for poles and zero eq  */
class PolesAndZerosEQAudioProcessor
    : public juce::AudioProcessor,
      public juce::AudioProcessorValueTreeState::Listener
{
public:
    // =========================================================================
    friend class EditorComponent;
    // TODO: remove this friend declaration: a DraggableElement should own a
    // pointer its corresponding parameter
    friend class DraggableElement;

    // =========================================================================
    /**
     * @brief Build a Poles And Zeros EQ processor
     *
     * @param n_elements Number of filter elements
     */
    PolesAndZerosEQAudioProcessor(int n_elements);

    // =========================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    bool isBusesLayoutSupported(
        const juce::AudioProcessor::BusesLayout&) const override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&,
                              juce::MidiBuffer&) override;

    // =========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    // =========================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    // =========================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int);
    void changeProgramName(int index, const juce::String& newName) override;

    // =========================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // =========================================================================
    /** Reset parameters to default and clear filter memory */
    void resetParams();
    /** Clear filter memory */
    void resetMemory();
    /** Activate or deactivate all filter elements */
    void setAllActive(bool);
    /** Deactivate specific filter element */
    void setInactive(int elementNr);
    /** Set the bypassed state of the processor */
    void setBypass(bool bypass);
    /** Double the value of the phases */
    void doublePhases();
    /** Divide by two the value of the phases */
    void halfPhases();
    /** Turn zeros into poles and vice versa */
    void swapPolesAndZeros();
    /** Set the parameters of a filter element */
    [[deprecated("Avoid using such an invasive method")]] void
    setFilter(const double magnitude, const double phase,
              FilterElement::Type type, const int elementNr,
              const double linearGain = 1.0);

private:
    // =========================================================================
    /**
     * Allocate processors in order to have enough to process the specified
     * number of channels
     */
    void allocateChannelsIfNeeded(int);
    /** Multiply the value of the phases */
    void multiplyPhases(double);

    // =========================================================================
    void parameterChanged(const juce::String& parameterID,
                          float newValue) override;

    // =========================================================================
    std::vector<FilterElementCascade> multiChannelCascade;
    juce::dsp::Gain<float> gainProcessor;

    // =========================================================================
    bool active, safetyFlag;
    const int n_elements;
    juce::AudioBuffer<double> pivotBuffer;

    std::function<void()> editorCallback;
    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState parameters;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolesAndZerosEQAudioProcessor);
};
