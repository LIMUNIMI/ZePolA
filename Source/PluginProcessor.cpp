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
    filter.memoryReset();
}

void PolesAndZerosEQAudioProcessor::releaseResources ()
{
    filter.memoryReset();
}

void PolesAndZerosEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    filter.processBlock(buffer);
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
    filter.parameterChanged(parameterID, newValue);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter ()
{
    return new PolesAndZerosEQAudioProcessor();
}
