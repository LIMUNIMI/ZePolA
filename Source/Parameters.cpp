#include "Parameters.h"

// =============================================================================
AudioProcessorValueTreeState::ParameterLayout
Parameters::createParameterLayout(int n_elements)
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    juce::NormalisableRange<float> masterGainRange(
        MASTER_GAIN_FLOOR, MASTER_GAIN_CEILING, MASTER_GAIN_INTERVAL);
    juce::NormalisableRange<float> gainRange(GAIN_FLOOR, GAIN_CEILING,
                                             GAIN_INTERVAL);

    for (int i = 0; i < n_elements; ++i)
    {
        std::string number = std::to_string(i + 1);

        params.push_back(std::make_unique<AudioParameterFloat>(
            MAGNITUDE_NAME + number, "Element " + number + " Magnitude",
            NormalisableRange<float>(MAGNITUDE_FLOOR, MAGNITUDE_CEILING,
                                     INTERVAL),
            MAGNITUDE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(
            PHASE_NAME + number, "Element " + number + " Phase",
            NormalisableRange<float>(PHASE_FLOOR, PHASE_CEILING, INTERVAL),
            PHASE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterBool>(
            ACTIVE_NAME + number, "Active " + number, ACTIVE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterBool>(
            TYPE_NAME + number, "Type" + number, TYPE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(
            GAIN_NAME + number, "Gain " + number, gainRange, GAIN_DEFAULT));
    }

    params.push_back(std::make_unique<AudioParameterBool>(
        FILTER_BYPASS_NAME, "EQ bypass", BYPASS_DEFAULT));
    params.push_back(std::make_unique<AudioParameterFloat>(
        MASTER_GAIN_NAME, "Gain (dB)", masterGainRange, MASTER_GAIN_DEFAULT));

    return {params.begin(), params.end()};
}

// =============================================================================
void Parameters::addListenerToAllParameters(
    AudioProcessorValueTreeState& valueTreeState,
    AudioProcessorValueTreeState::Listener* listener)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.copyState().createXml());

    for (auto element : xml->getChildWithTagNameIterator("PARAM"))
    {
        const juce::String& id = element->getStringAttribute("id");
        valueTreeState.addParameterListener(id, listener);
    }
}
void Parameters::setParameterValue(juce::RangedAudioParameter* parameter,
                                   float value)
{
    if (parameter)
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(value);
        parameter->endChangeGesture();
    }
}
void Parameters::resetParameterValue(juce::RangedAudioParameter* p)
{
    Parameters::setParameterValue(p, p->getDefaultValue());
}