#pragma once
#include <JuceHeader.h>

// Generic constants
#define MAX_NUM_CHANNELS                        2

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
#define POLE_MAGNITUDE_CEILING                  0.999f

#define POLE_PHASE_NAME                         "PP"
#define POLE_PHASE_FLOOR                        0.0f
#define POLE_PHASE_CEILING                      1.0f

#define COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT     0.0f


namespace Parameters
{
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout ()
    {
        std::vector<std::unique_ptr<RangedAudioParameter>> params;
        
        params.push_back(std::make_unique<AudioParameterFloat>(ZERO_MAGNITUDE_NAME, "Zeros Magnitude", ZERO_MAGNITUDE_FLOOR, ZERO_MAGNITUDE_CEILING, COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(ZERO_PHASE_NAME, "Zeros Phase", ZERO_PHASE_FLOOR, ZERO_PHASE_CEILING, COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        
        params.push_back(std::make_unique<AudioParameterFloat>(POLE_MAGNITUDE_NAME, "Poles Magnitude", POLE_MAGNITUDE_FLOOR, POLE_MAGNITUDE_CEILING, COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(POLE_PHASE_NAME, "Poles Phase", POLE_PHASE_FLOOR, POLE_PHASE_CEILING, COMPLEX_PHASE_AND_MAGNITUDE_DEFAULT));
        
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
