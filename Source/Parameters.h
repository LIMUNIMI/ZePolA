#pragma once
#include <JuceHeader.h>

// =============================================================================
// Generic constants & macros
#define NUMBER_OF_FILTER_ELEMENTS 8
#define STEREO 2
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
#define MAGNITUDE_NAME "M"
#define MAGNITUDE_FLOOR 0.0f
#define MAGNITUDE_CEILING 1.0f
#define POLE_MAX_MAGNITUDE 0.99999

#define PHASE_NAME "P"
#define PHASE_FLOOR 0.0f
#define PHASE_CEILING 1.0f

#define MAGNITUDE_DEFAULT 0.0f
#define PHASE_DEFAULT 0.0f
#define INTERVAL 0.00001f

#define TYPE_NAME "T"
#define TYPE_DEFAULT true

#define ACTIVE_NAME "A"
#define ACTIVE_DEFAULT false

#define GAIN_NAME "G"
#define GAIN_FLOOR -128.0f
#define GAIN_CEILING 24.0f
#define GAIN_INTERVAL 0.1f
#define GAIN_DEFAULT 0.0f

#define MASTER_GAIN_NAME "MSTR_GAIN"
#define MASTER_GAIN_FLOOR -48.0f
#define MASTER_GAIN_CEILING 48.0f
#define MASTER_GAIN_INTERVAL 0.01f
#define MASTER_GAIN_DEFAULT 0.0f

#define FILTER_BYPASS_NAME "BYPASS"
#define BYPASS_DEFAULT false

// =============================================================================
namespace Parameters
{
/** Initialize the parameter layout for the plugin */
juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

// =============================================================================
/** Add the specified listener as listener of all parameters  */
void addListenerToAllParameters(
    juce::AudioProcessorValueTreeState& valueTreeState,
    juce::AudioProcessorValueTreeState::Listener* listener);
/** Set parameter value, enclosing it in a gesture */
void setParameterValue(juce::RangedAudioParameter* parameter, float value);
}  // namespace Parameters
