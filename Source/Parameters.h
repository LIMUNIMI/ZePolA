#pragma once
#include <JuceHeader.h>

// Generic constants
#define NUMBER_OF_FILTER_ELEMENTS               8
#define SLIDERS_FLOOR                           0.0f
#define SLIDERS_CEILING                         1.0f

// FilterElement constants & ids
#define MAGNITUDE_NAME                          "M"
#define MAGNITUDE_FLOOR                         0.0f
#define MAGNITUDE_CEILING                       1.0f
#define POLE_MAX_MAGNITUDE                      0.99999

#define PHASE_NAME                              "P"
#define PHASE_FLOOR                             0.0f
#define PHASE_CEILING                           1.0f

#define MAGNITUDE_DEFAULT                       0.0f
#define PHASE_DEFAULT                           0.0f
#define INTERVAL                                0.00001f

#define TYPE_NAME                               "T"
#define TYPE_DEFAULT                            true

#define ACTIVE_NAME                             "A"
#define ACTIVE_DEFAULT                          false

// Plugin constants & ids

#define GAIN_NAME                               "GAIN"
#define GAIN_FLOOR                              -24.0f
#define GAIN_CEILING                            24.0f
#define GAIN_INTERVAL                           0.01f
#define GAIN_DEFAULT                            0.0f

#define FILTER_BYPASS_NAME                      "BYPASS"
#define BYPASS_DEFAULT                          false


namespace Parameters
{

    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout ()
{
        std::vector<std::unique_ptr<RangedAudioParameter>> params;
        
        for (int i = 0; i < NUMBER_OF_FILTER_ELEMENTS; ++ i)
        {
            std::string number = std::to_string(i + 1);
            
            params.push_back(std::make_unique<AudioParameterFloat>(MAGNITUDE_NAME + number, "Element " + number + " Magnitude", NormalisableRange<float>(MAGNITUDE_FLOOR, MAGNITUDE_CEILING, INTERVAL), MAGNITUDE_DEFAULT));
            params.push_back(std::make_unique<AudioParameterFloat>(PHASE_NAME + number, "Element " + number + " Phase", NormalisableRange<float>(PHASE_FLOOR, PHASE_CEILING, INTERVAL), PHASE_DEFAULT));
            params.push_back(std::make_unique<AudioParameterBool>(ACTIVE_NAME + number, "Active " + number, ACTIVE_DEFAULT));
            params.push_back(std::make_unique<AudioParameterBool>(TYPE_NAME + number, "Type" + number, TYPE_DEFAULT));
        }
        
        juce::NormalisableRange<float> gainRange(GAIN_FLOOR, GAIN_CEILING, GAIN_INTERVAL);
        
        params.push_back(std::make_unique<AudioParameterBool>(FILTER_BYPASS_NAME, "EQ bypass", BYPASS_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(GAIN_NAME, "Gain (dB)", gainRange, GAIN_DEFAULT));
        
        
        return {params.begin(), params.end()};
    }

    static void addListenerToAllParameters (AudioProcessorValueTreeState& valueTreeState, AudioProcessorValueTreeState::Listener* listener)
    {
        std::unique_ptr<XmlElement>xml(valueTreeState.copyState().createXml());
        
        for (auto element : xml->getChildWithTagNameIterator("PARAM"))
        {
            const String& id = element->getStringAttribute("id");
            valueTreeState.addParameterListener(id, listener);
        }
    }
}

