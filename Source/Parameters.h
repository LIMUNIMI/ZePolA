/*
  ==============================================================================

    Parameters.h

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// =============================================================================
// Parameter ids
#define BYPASS_ID "BYPASS"
#define NOISE_ID "NOISE"
#define GAIN_ID "GAIN"
#define MAGNITUDE_ID_PREFIX "MAGNITUDE_"
#define PHASE_ID_PREFIX "PHASE_"
#define GAIN_ID_PREFIX "GAIN_"
#define ACTIVE_ID_PREFIX "ACTIVE_"
#define INVERTED_ID_PREFIX "INVERTED_"
#define SINGLE_ID_PREFIX "SINGLE_"
#define TYPE_ID_PREFIX "TYPE_"
#define AUTO_GAIN_PROPERTY_ID "autoGain"
#define AUTO_FILTER_PROPERTY_ID "autoFilterDesign"

// =============================================================================
namespace Parameters
{
/** Set parameter value, enclosing the operation in a gesture */
void setParameterValue(juce::RangedAudioParameter* parameter, float value);
/** Set parameter value to default, enclosing the operation in a gesture */
void resetParameterValue(juce::RangedAudioParameter*);
}  // namespace Parameters

// =============================================================================
/** Sample rate listener abstract base class */
class SampleRateListener
{
public:
    // =========================================================================
    virtual void sampleRateChangedCallback(double) = 0;
};

// =============================================================================
/**
 * A class for audio processors with a value tree state
 */
class VTSAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    VTSAudioProcessor(std::vector<std::unique_ptr<juce::RangedAudioParameter>>,
                      const juce::Identifier& valueTreeType);
    ~VTSAudioProcessor();

    //==============================================================================
    virtual void getStateInformation(juce::MemoryBlock& destData) override;
    virtual void setStateInformation(const void* data,
                                     int sizeInBytes) override;

    //==============================================================================
    /** Make an attachment for the value tree state */
    template <typename AttachmentType, typename ComponentType>
    AttachmentType* makeAttachment(juce::StringRef parameterID,
                                   ComponentType& slider);

    //==============================================================================
    /** Add a sample rate listener to this processor */
    void addSampleRateListener(SampleRateListener*);
    /** Remove a sample rate listener from this processor */
    void removeSampleRateListener(SampleRateListener*);
    /** Trigger a specific callback */
    void sendSampleRateToListener(SampleRateListener*, double);
    /** Trigger a specific callback */
    void sendSampleRateToListener(SampleRateListener*);
    /** Trigger all sample rate callbacks */
    void sendSampleRateToAllListeners(double);
    /** Trigger all sample rate callbacks */
    void sendSampleRateToAllListeners();
    virtual void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    //==============================================================================
    /** Get parameter by ID */
    juce::RangedAudioParameter* getParameterById(juce::StringRef);
    /** Format parameter value, so that it is in range */
    float formatParameterValue(juce::StringRef parameterID, float value);
    /** Set parameter value, enclosing the operation in a gesture */
    void setParameterValue(juce::StringRef parameterID, float value);
    /** Get parameter unnormalized value by parameter ID */
    float getParameterUnnormValue(juce::StringRef parameterID) const;
    /** Set parameter value to default, enclosing the operation in a gesture */
    virtual void resetParameterValue(juce::StringRef);
    /** Reset value of all parameters */
    virtual void resetParameters();
    /** Gets the vector of all parameter identifiers */
    std::vector<juce::StringRef> parameterIDs();
    /** Add parameter listener to value tree state */
    void addParameterListener(juce::StringRef,
                              juce::AudioProcessorValueTreeState::Listener*);
    /** Remove parameter listener from value tree state */
    void removeParameterListener(juce::StringRef,
                                 juce::AudioProcessorValueTreeState::Listener*);
    /** Undo last action with undo manager */
    void undoManagerUndo();
    /** Redo last action with undo manager */
    void undoManagerRedo();

protected:
    //==============================================================================
    /**
     * Push a listener and its id into the list. Call this method from
     * appendListeners()
     */
    void pushListener(juce::String,
                      juce::AudioProcessorValueTreeState::Listener*);
    /** Appends the specified listener as listener of all parameters  */
    void
    pushListenerForAllParameters(juce::AudioProcessorValueTreeState::Listener*);
    /**
     * Append listeners and ids to the lists. Subclasses should override this
     * method
     */
    virtual void appendListeners();
    /**
     * Responsible for listener and id initialization and registration at
     * construction time. Call this method in the final class constructor.
     */
    void initializeListeners();

private:
    //==============================================================================
    /**
     * Helper function for notifying all listeners of a value change. Used by
     * setStateInformation()
     */
    void sendValueChangedMessageToAllListeners();

    //==============================================================================
    juce::AudioProcessorValueTreeState valueTreeState;
    std::vector<juce::AudioProcessorValueTreeState::Listener*> listeners;
    std::vector<juce::String> listeners_ids;
    std::vector<SampleRateListener*> sr_listeners;
    juce::UndoManager undo_manager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VTSAudioProcessor)
};

// =============================================================================
/**
 * A simple value tree state parameter change listener
 */
class SimpleListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    SimpleListener(std::function<void(float)>);
    void parameterChanged(const juce::String&, float) override;

private:
    //==============================================================================
    std::function<void(float)> setterFunction;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleListener)
};

// =============================================================================
/**
 * A simple value tree state parameter change listener for triggering a function
 */
class TriggerListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    TriggerListener(std::function<void()>);
    void parameterChanged(const juce::String&, float) override;

private:
    //==============================================================================
    std::function<void()> triggerFunction;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriggerListener)
};
