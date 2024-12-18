#pragma once
#include <JuceHeader.h>

// =============================================================================
// Parameter id
#define BYPASS_ID "BYPASS"
#define GAIN_ID "GAIN"
#define MAGNITUDE_ID_PREFIX "MAGNITUDE_"
#define PHASE_ID_PREFIX "PHASE_"

// =============================================================================
// Generic constants & macros
#define SLIDERS_FLOOR 0.0f
#define SLIDERS_CEILING 1.0f

// =============================================================================
// GUI constants and macros
#define NUMBER_OF_REFERENCE_FREQUENCIES 8
#define FREQUENCY_FLOOR 20.0

#define DESIGN_FREQUENCY_FLOOR 1.0

#define SELECTABLE_FILTER_TYPES                                                \
    {                                                                          \
        "BUTTERWORTH", "CHEBYSHEV I", "CHEBYSHEV II", "ELLIPTIC"               \
    }
#define SELECTABLE_ORDERS_BUTTERWORTH                                          \
    {                                                                          \
        "2", "4", "6", "8"                                                     \
    }

// =============================================================================
// Filter Element macros
#define FILTER_ELEMENT_GAIN_FLOOR -128.0

// =============================================================================
// Plugin parameters macros
#define TYPE_NAME "T"
#define TYPE_DEFAULT true

#define ACTIVE_NAME "A"
#define ACTIVE_DEFAULT false

#define GAIN_NAME "G"
#define GAIN_FLOOR -128.0f
#define GAIN_CEILING 24.0f
#define GAIN_INTERVAL 0.1f
#define GAIN_DEFAULT 0.0f

// =============================================================================
namespace Parameters
{
/** Add the specified listener as listener of all parameters  */
void addListenerToAllParameters(
    juce::AudioProcessorValueTreeState& valueTreeState,
    juce::AudioProcessorValueTreeState::Listener* listener);
/** Set parameter value, enclosing the operation in a gesture */
void setParameterValue(juce::RangedAudioParameter* parameter, float value);
/** Set parameter value to default, enclosing the operation in a gesture */
void resetParameterValue(juce::RangedAudioParameter*);
}  // namespace Parameters

// =============================================================================
/**
 * A class for audio processors with a value tree state
 */
class VTSAudioProcessor : public juce::AudioProcessor,
                          public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    VTSAudioProcessor(std::vector<std::unique_ptr<juce::RangedAudioParameter>>,
                      const juce::Identifier& valueTreeType,
                      juce::UndoManager* undoManagerToUse = nullptr);
    ~VTSAudioProcessor();

    //==============================================================================
    virtual void getStateInformation(juce::MemoryBlock& destData);
    virtual void setStateInformation(const void* data, int sizeInBytes);

    //==============================================================================
    /** Set parameter value, enclosing the operation in a gesture */
    void setParameterValue(juce::StringRef parameterID, float value);
    /** Set parameter value to default, enclosing the operation in a gesture */
    void resetParameterValue(juce::StringRef);
    /** Reset value of all parameters */
    void resetParameters();

protected:
    //==============================================================================
    // TODO: Should be private in the end
    juce::AudioProcessorValueTreeState valueTreeState;

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
    /** Gets the vector of all parameter identifiers */
    std::vector<juce::StringRef> parameterIDs();
    /**
     * Helper function for notifying all listeners of a value change. Used by
     * setStateInformation()
     */
    void sendValueChangedMessageToAllListeners();

    //==============================================================================
    std::vector<juce::AudioProcessorValueTreeState::Listener*> listeners;
    std::vector<juce::String> listeners_ids;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VTSAudioProcessor)
};

/**
 * A simple value tree state parameter change listener
 */
class SimpleListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    SimpleListener(std::function<void(float)>);
    void parameterChanged(const juce::String&, float) override;

private:
    std::function<void(float)> setterFunction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleListener)
};
