#pragma once
#include <JuceHeader.h>

// Generic constants
#define MAX_NUM_CHANNELS                        2
#define PI                                      3.141592653589793238462643383279502884197169399375105820974944

// Zero constants
#define ZERO_MAGNITUDE_NAME                     "ZM"
#define ZERO_MAGNITUDE_FLOOR                    0.0f
#define ZERO_MAGNITUDE_CEILING                  1.0f

#define ZERO_PHASE_NAME                         "ZP"
#define ZERO_PHASE_FLOOR                        0.0f
#define ZERO_PHASE_CEILING                      1.0f

// Pole constants
#define POLE_MAGNITUDE_NAME                     "PM"
#define POLE_MAGNITUDE_FLOOR                    0.0f
#define POLE_MAGNITUDE_CEILING                  0.99999f

#define POLE_PHASE_NAME                         "PP"
#define POLE_PHASE_FLOOR                        0.0f
#define POLE_PHASE_CEILING                      1.0f

#define COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT     0.0f
#define INTERVAL                                0.00001f


namespace Parameters
{
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout ()
    {
        std::vector<std::unique_ptr<RangedAudioParameter>> params;
        
        params.push_back(std::make_unique<AudioParameterFloat>(ZERO_MAGNITUDE_NAME, "Zeros Magnitude", NormalisableRange<float>(ZERO_MAGNITUDE_FLOOR, ZERO_MAGNITUDE_CEILING, INTERVAL), COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(ZERO_PHASE_NAME, "Zeros Phase", NormalisableRange<float>(ZERO_PHASE_FLOOR, ZERO_PHASE_CEILING, INTERVAL), COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        
        params.push_back(std::make_unique<AudioParameterFloat>(POLE_MAGNITUDE_NAME, "Poles Magnitude", NormalisableRange<float>(POLE_MAGNITUDE_FLOOR, POLE_MAGNITUDE_CEILING, INTERVAL), COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(POLE_PHASE_NAME, "Poles Phase", NormalisableRange<float>(POLE_PHASE_FLOOR, POLE_PHASE_CEILING, INTERVAL), COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        
        return {params.begin(), params.end()};
    }

    static void addListenerToAllParameters (AudioProcessorValueTreeState& valueTreeState, AudioProcessorValueTreeState::Listener* listener)
    {
        std:: unique_ptr<XmlElement> xml(valueTreeState.copyState().createXml());
        
        for (auto element : xml->getChildWithTagNameIterator("PARAM"))
        {
            const String& id = element->getStringAttribute("id");
            valueTreeState.addParameterListener(id, listener);
        }
    }
}
