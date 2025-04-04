#pragma once
#include "DSP/Filter.h"
#include "Parameters.h"
#include <JuceHeader.h>

// =============================================================================
/** Plugin processor for poles and zero eq  */
class ZePolAudioProcessor : public VTSAudioProcessor
{
public:
    // =========================================================================
    /**
     * @brief Build a Poles And Zeros EQ processor
     *
     * @param n_elements Number of filter elements
     */
    ZePolAudioProcessor(int n_elements);

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
     * Set an element's magnitude as inverted
     *
     * @param i Element index (zero-based)
     * @param v New inverted state
     */
    void setElementInverted(int i, bool v);
    /**
     * Set an element's magnitude as inverted, with a value threshold on 0.5
     *
     * @param i Element index (zero-based)
     * @param v New inverted state, as a float
     */
    void setElementInvertedTh(int i, float v);
    /**
     * Set a filter as a 1-element filter
     *
     * @param i Element index (zero-based)
     * @param v New single state
     */
    void setElementSingle(int i, bool v);
    /**
     * Set a filter as a 1-element filter, with a value threshold on 0.5
     *
     * @param i Element index (zero-based)
     * @param v New single state, as a float
     */
    void setElementSingleTh(int i, float v);
    /**
     * Set an element's type
     *
     * @param i Element index (zero-based)
     * @param v New element type
     */
    void setElementType(int i, bool v);
    /**
     * Set an element's type, with a float value
     *
     * @param i Element index (zero-based)
     * @param v New element type, as a float value
     */
    void setElementTypeTh(int i, float v);

    // =========================================================================
    /** Get the number of filter elements */
    int getNElements() const;
    /** Get the DTFT of the filter */
    std::complex<double> dtft(double omega) const;
    /** Get the time-domain coefficients of the filter */
    std::vector<std::array<double, 8>> getCoefficients() const;
    /** Get the peak gain value for the cascade (in dB) */
    double getCascadePeakGain() const;
    /** Get the auto gain value for the specified element */
    double getElementAutoGain(int) const;
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
    void resetParameters() override;

    // =========================================================================
    /** Add an unsafe output listener to this processor */
    void addUnsafeOutputListener(juce::Value::Listener*);
    /** Remove an unsafe output listener from this processor */
    void removeUnsafeOutputListener(juce::Value::Listener*);
    /** Mark the processor as in a safe state (or not) */
    void markAsSafe(bool safe = true);

private:
    // =========================================================================
    void appendListeners() override;
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
    bool bypassed;
    juce::Value unsafe;
    const int n_elements;
    juce::AudioBuffer<double> pivotBuffer;

    std::function<void()> editorCallback;
    juce::UndoManager undoManager;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZePolAudioProcessor);
};
