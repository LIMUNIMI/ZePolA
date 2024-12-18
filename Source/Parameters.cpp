#include "Parameters.h"

// =============================================================================
std::vector<std::unique_ptr<juce::RangedAudioParameter>>
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

    return params;
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

// ============================================================================
VTSAudioProcessor::VTSAudioProcessor(
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params,
    const juce::Identifier& valueTreeType, juce::UndoManager* undoManagerToUse)
    : valueTreeState(*this, undoManagerToUse, valueTreeType,
                     {params.begin(), params.end()})
{
}
VTSAudioProcessor::~VTSAudioProcessor()
{
    jassert(listeners_ids.size() == listeners.size());

    size_t n = std::min(listeners.size(), listeners_ids.size());
    for (size_t i = 0; i < n; ++i)
        if (listeners[i] != nullptr)
        {
            for (size_t j = i + 1; j < n; ++j)
                if (listeners[i] == listeners[j]) listeners[j] = nullptr;
            delete listeners[i];
        }
}

// ============================================================================
void VTSAudioProcessor::appendListeners() { }
void VTSAudioProcessor::pushListener(
    juce::String id, juce::AudioProcessorValueTreeState::Listener* listener)
{
    listeners_ids.push_back(id);
    listeners.push_back(listener);
}
void VTSAudioProcessor::initializeListeners()
{
    jassert(listeners_ids.size() == 0);
    jassert(listeners.size() == 0);

    appendListeners();

    jassert(listeners_ids.size() == listeners.size());
    size_t n = std::min(listeners.size(), listeners_ids.size());
    for (size_t i = 0; i < n; ++i)
        valueTreeState.addParameterListener(listeners_ids[i], listeners[i]);
}

// ============================================================================
void VTSAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml(
        valueTreeState.copyState().createXml());
    juce::AudioProcessor::copyXmlToBinary(*xml, destData);
}
void VTSAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(
        juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr)
        if (xml->hasTagName(valueTreeState.state.getType()))
            valueTreeState.replaceState(juce::ValueTree::fromXml(*xml));
    sendValueChangedMessageToAllListeners();
}

// ============================================================================
void VTSAudioProcessor::sendValueChangedMessageToAllListeners()
{
    DBG("--- sendValueChangedMessageToAllListeners ------------------------");
    std::unique_ptr<juce::XmlElement> xml(
        valueTreeState.copyState().createXml());
    for (auto* element : xml->getChildWithTagNameIterator("PARAM"))
    {
        const juce::String& id = element->getStringAttribute("id");
        auto* param            = valueTreeState.getParameter(id);
        auto val               = param->getValue();
        DBG(id << "->sendValueChangedMessageToListeners (" << val << ")");
        param->sendValueChangedMessageToListeners(val);
    }
    DBG("------------------------------------------------------------------");
}

// ============================================================================
SimpleListener::SimpleListener(std::function<void(float)> f) : setterFunction(f)
{
}
void SimpleListener::parameterChanged(const juce::String& /* id */,
                                      float newValue)
{
    setterFunction(newValue);
}
