#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "PluginEditor.h"

PolesAndZerosEQAudioProcessor::PolesAndZerosEQAudioProcessor()
: parameters(*this, nullptr, "PolesAndZero-EQ", Parameters::createParameterLayout())
{
    Parameters::addListenerToAllParameters(parameters, this);
}

PolesAndZerosEQAudioProcessor::~PolesAndZerosEQAudioProcessor ()
{
}

void PolesAndZerosEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    
    gainProcessor.prepare(spec);
    gainProcessor.setGainDecibels(GAIN_DEFAULT);
    
    filter.memoryReset();
}

void PolesAndZerosEQAudioProcessor::releaseResources ()
{
}

void PolesAndZerosEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    if (!active) return;

    filter.processBlock(buffer);
    
    juce::dsp::AudioBlock<float> block (buffer);
    gainProcessor.process(juce::dsp::ProcessContextReplacing<float> (block));
}

void PolesAndZerosEQAudioProcessor::processBlockBypassed(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    filter.memoryReset();
}

juce::AudioProcessorEditor* PolesAndZerosEQAudioProcessor::createEditor()
{
    return new PluginEditor(*this, parameters);
}

void PolesAndZerosEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolesAndZerosEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

void PolesAndZerosEQAudioProcessor::parameterChanged (const String& parameterID, float newValue)
{
    if (parameterID == "BYPASS")
    {
        active = !(newValue > 0.5f);
        if (!active)
            filter.memoryReset();
        return;
    }
    
    if (parameterID == "GAIN")
    {
        gainProcessor.setGainDecibels(newValue);
    }
    
    juce::String parameter = parameterID.substring(0, 1);
    const int elementNr = parameterID.substring(1).getIntValue();
    
    newValue = static_cast<double>(newValue);

    if (parameter == "M")
    {
        filter.setMagnitude(elementNr, newValue);
    }
    else if (parameter == "P")
    {
        filter.setPhase(elementNr, newValue);
    }
    else if (parameter == "A")
    {
        filter.setUnsetElementActive(elementNr, newValue > 0.5);
    }
    else if (parameter == "T")
    {
        filter.setType(elementNr, newValue > 0.5);
    }
    
    if (editorCallback)
    {
        editorCallback();
    }
}

std::complex<double> PolesAndZerosEQAudioProcessor::getFilterSpectrum (const double phi)
{
    return filter.getSpectrum(phi);
}

std::vector<std::shared_ptr<FilterElement>> PolesAndZerosEQAudioProcessor::getFilterElementsChain ()
{
    return filter.getElementsChain();
}

void PolesAndZerosEQAudioProcessor::resetFilter ()
{
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        parameters.getParameter(MAGNITUDE_NAME + std::to_string(i))->setValueNotifyingHost(MAGNITUDE_DEFAULT);
        parameters.getParameter(PHASE_NAME + std::to_string(i))->setValueNotifyingHost(PHASE_DEFAULT);
        parameters.getParameter(TYPE_NAME + std::to_string(i))->setValueNotifyingHost(TYPE_DEFAULT);
        parameters.getParameter(ACTIVE_NAME + std::to_string(i))->setValueNotifyingHost(ACTIVE_DEFAULT);
    }
    parameters.getParameter(GAIN_NAME)->setValueNotifyingHost(jmap(GAIN_DEFAULT, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
    parameters.getParameter(FILTER_BYPASS_NAME)->setValueNotifyingHost(BYPASS_DEFAULT);
    
    filter.memoryReset();
}

float PolesAndZerosEQAudioProcessor::getCurrentGain ()
{
    return gainProcessor.getGainLinear();
}

void PolesAndZerosEQAudioProcessor::multiplyPhases()
{
    auto elements = getFilterElementsChain();
    double currentPhase;
    double newPhase;
    
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        currentPhase = elements[i - 1]->getPhase();
        if (currentPhase > (PHASE_CEILING / 2))
            newPhase = 1.0;
        else
            newPhase = currentPhase * 2.0;
        parameters.getParameter(PHASE_NAME + std::to_string(i))->setValueNotifyingHost(newPhase);
    }
}

void PolesAndZerosEQAudioProcessor::dividePhases()
{
    auto elements = getFilterElementsChain();
    double currentPhase;
    
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        currentPhase = elements[i - 1]->getPhase();
        parameters.getParameter(PHASE_NAME + std::to_string(i))->setValueNotifyingHost(currentPhase * 0.5);
    }
}

void PolesAndZerosEQAudioProcessor::swapElements()
{
    auto elements = getFilterElementsChain();
    bool newType;
    
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        newType = true;
        auto currentType = elements[i - 1]->getType();
        if (currentType == FilterElement::ZERO)
            newType = false;
        
        parameters.getParameter(TYPE_NAME + std::to_string(i))->setValueNotifyingHost(newType);
    }
}

void PolesAndZerosEQAudioProcessor::turnOnOffAllElements(bool option)
{
    bool active = false;
    
    if (option)
        active = true;
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
        parameters.getParameter(ACTIVE_NAME + std::to_string(i))->setValueNotifyingHost(active);
}

void PolesAndZerosEQAudioProcessor::setFilterOrder(int filtersToActivate)
{
    for (int i = 1; i <= filtersToActivate; ++ i)
    {
        parameters.getParameter(ACTIVE_NAME + std::to_string(i))->setValueNotifyingHost(true);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter ()
{
    return new PolesAndZerosEQAudioProcessor();
}

