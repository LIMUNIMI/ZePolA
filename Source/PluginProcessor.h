#pragma once
#include "DSP/Filter.h"
#include "Parameters.h"
#include <JuceHeader.h>

// =============================================================================
/** Plugin processor for poles and zero eq  */
class PolesAndZerosEQAudioProcessor : public VTSAudioProcessor
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
    /**
     * Set an element's magnitude
     *
     * @param i Element index (zero-based)
     * @param v Magnitude value
     */
    void setElementMagnitude(int i, float v);
    /**
     * Set an element's phase
     *
     * @param i Element index (zero-based)
     * @param v Phase value
     */
    void setElementPhase(int i, float v);
    /**
     * Set an element's gain in decibel
     *
     * @param i Element index (zero-based)
     * @param v Gain value (decibel)
     */
    void setElementGainDb(int i, float v);
    /**
     * Set an element's active state
     *
     * @param i Element index (zero-based)
     * @param v New active state
     */
    void setElementActive(int i, bool v);
    /**
     * Set an element's active state, with a value threshold on 0.5
     *
     * @param i Element index (zero-based)
     * @param v New active state, as a float
     */
    void setElementActiveTh(int i, float v);
    /**
     * Set an element's type
     *
     * @param i Element index (zero-based)
     * @param v New element type
     */
    void setElementType(int i, FilterElement::Type v);
    /**
     * Set an element's type, with a float value
     *
     * @param i Element index (zero-based)
     * @param v New element type, as a float value
     */
    void setElementTypeF(int i, float v);

    // =========================================================================
    /** Clear filter memory */
    void resetMemory();
    /** Activate or deactivate all filter elements */
    void setAllActive(bool);
    /** Set the bypassed state of the processor */
    void setBypass(bool bypass);
    /** Set the bypassed state of the processor with a value threshold on 0.5 */
    void setBypassTh(float bypass);
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
    void appendListeners() override;
    /** Mark the processor as in a safe state */
    void markAsSafe(float);
    /** Process extra channels in audio block */
    void processBlockExtraChannels(juce::AudioBuffer<float>&);
    /**
     * Allocate processors in order to have enough to process the specified
     * number of channels
     */
    void allocateChannelsIfNeeded(int);
    /** Delete all allocated channel processors except one */
    void resetChannels();
    /** Multiply the value of the phases */
    void multiplyPhases(double);

    // =========================================================================
    std::vector<FilterElementCascade> multiChannelCascade;
    juce::dsp::Gain<float> gain;

    // =========================================================================
    bool bypassed, unsafe;
    const int n_elements;
    juce::AudioBuffer<double> pivotBuffer;

    std::function<void()> editorCallback;
    juce::UndoManager undoManager;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PolesAndZerosEQAudioProcessor);
};
