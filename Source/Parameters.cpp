#include "Parameters.h"
#include "GUI/DraggableLabel.h"
#include "Macros.h"

// =============================================================================
void Parameters::setParameterValue(juce::RangedAudioParameter* parameter,
                                   float value)
{
    if (parameter)
    {
        parameter->beginChangeGesture();
        parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
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
    jassert(sr_listeners.size() == 0);
    jassert(listeners_ids.size() == listeners.size());

    size_t n = std::min(listeners.size(), listeners_ids.size());
    for (size_t i = 0; i < n; ++i)
        valueTreeState.removeParameterListener(listeners_ids[i], listeners[i]);
    for (size_t i = 0; i < n; ++i)
        if (listeners[i] != nullptr)
        {
            for (size_t j = i + 1; j < n; ++j)
                if (listeners[i] == listeners[j]) listeners[j] = nullptr;
            delete listeners[i];
        }
}

// ============================================================================
void VTSAudioProcessor::appendListeners() {}
void VTSAudioProcessor::pushListener(
    juce::String id, juce::AudioProcessorValueTreeState::Listener* listener)
{
    listeners_ids.push_back(id);
    listeners.push_back(listener);
}
void VTSAudioProcessor::pushListenerForAllParameters(
    juce::AudioProcessorValueTreeState::Listener* listener)
{
    for (auto id : parameterIDs()) pushListener(id, listener);
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

//==============================================================================
template <typename AttachmentType, typename ComponentType>
AttachmentType* VTSAudioProcessor::makeAttachment(juce::StringRef parameterID,
                                                  ComponentType& component)
{
    return new AttachmentType(valueTreeState, parameterID, component);
}

template juce::AudioProcessorValueTreeState::SliderAttachment*
VTSAudioProcessor::makeAttachment(juce::StringRef parameterID,
                                  juce::Slider& component);
template juce::AudioProcessorValueTreeState::ButtonAttachment*
VTSAudioProcessor::makeAttachment(juce::StringRef parameterID,
                                  juce::Button& component);
template DraggableLabelAttachment*
VTSAudioProcessor::makeAttachment(juce::StringRef parameterID,
                                  DraggableLabel& component);

//==============================================================================
void VTSAudioProcessor::addSampleRateListener(SampleRateListener* srl)
{
    // Check that listener is not already in the list
    ONLY_ON_DEBUG(std::vector<SampleRateListener*>::iterator pos
                  = std::find(sr_listeners.begin(), sr_listeners.end(), srl);
                  jassert(pos == sr_listeners.end());)
    sr_listeners.push_back(srl);
    sendSampleRateToListener(srl);
}
void VTSAudioProcessor::removeSampleRateListener(SampleRateListener* srl)
{
    std::vector<SampleRateListener*>::iterator pos
        = std::find(sr_listeners.begin(), sr_listeners.end(), srl);
    if (pos != sr_listeners.end()) sr_listeners.erase(pos);
    ONLY_ON_DEBUG(else jassertfalse;)
}
void VTSAudioProcessor::sendSampleRateToListener(SampleRateListener* srl,
                                                 double sampleRate)
{
    srl->sampleRateChangedCallback(sampleRate);
}
void VTSAudioProcessor::sendSampleRateToListener(SampleRateListener* srl)
{
    sendSampleRateToListener(srl, getSampleRate());
}
void VTSAudioProcessor::sendSampleRateToAllListeners(double sampleRate)
{
    for (auto srl : sr_listeners) sendSampleRateToListener(srl, sampleRate);
}
void VTSAudioProcessor::sendSampleRateToAllListeners()
{
    sendSampleRateToAllListeners(getSampleRate());
}
void VTSAudioProcessor::prepareToPlay(double sampleRate, int)
{
    sendSampleRateToAllListeners(sampleRate);
}

//==============================================================================
juce::RangedAudioParameter*
VTSAudioProcessor::getParameterById(juce::StringRef parameterID)
{
    return valueTreeState.getParameter(parameterID);
}
float VTSAudioProcessor::formatParameterValue(juce::StringRef parameterID,
                                              float value)
{
    juce::NormalisableRange<float> nr
        = valueTreeState.getParameterRange(parameterID);
    return nr.convertFrom0to1(nr.convertTo0to1(value));
}
float VTSAudioProcessor::getParameterUnnormValue(juce::StringRef parameterID)
{
    return valueTreeState.getParameterRange(parameterID)
        .convertFrom0to1(valueTreeState.getParameter(parameterID)->getValue());
}
void VTSAudioProcessor::setParameterValue(juce::StringRef parameterID,
                                          float value)
{
    Parameters::setParameterValue(getParameterById(parameterID), value);
}
void VTSAudioProcessor::resetParameterValue(juce::StringRef parameterID)
{
    Parameters::resetParameterValue(getParameterById(parameterID));
}
void VTSAudioProcessor::resetParameters()
{
    for (auto id : parameterIDs()) resetParameterValue(id);
}

// ============================================================================
std::vector<juce::StringRef> VTSAudioProcessor::parameterIDs()
{
    std::vector<juce::StringRef> v;
    std::unique_ptr<juce::XmlElement> xml(
        valueTreeState.copyState().createXml());
    for (auto* element : xml->getChildWithTagNameIterator("PARAM"))
        v.push_back(element->getStringAttribute("id"));
    return v;
}
void VTSAudioProcessor::addParameterListener(
    juce::StringRef i, juce::AudioProcessorValueTreeState::Listener* l)
{
    valueTreeState.addParameterListener(i, l);
}
void VTSAudioProcessor::removeParameterListener(
    juce::StringRef i, juce::AudioProcessorValueTreeState::Listener* l)
{
    valueTreeState.removeParameterListener(i, l);
}
void VTSAudioProcessor::sendValueChangedMessageToAllListeners()
{
    DBG("--- sendValueChangedMessageToAllListeners ------------------------");
    for (auto id : parameterIDs())
    {
        auto* param = valueTreeState.getParameter(id);
        auto val    = param->getValue();
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
