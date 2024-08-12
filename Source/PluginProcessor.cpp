#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"

BiquadFilterEQAudioProcessor::BiquadFilterEQAudioProcessor()
: parameters(*this, nullptr, "BiquadFilter", Parameters::createParameterLayout())
{
    Parameters::addListenerToAllParameters(parameters, this);
}

BiquadFilterEQAudioProcessor::~BiquadFilterEQAudioProcessor ()
{
}

void BiquadFilterEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    filter.prepareToPlay();
}

void BiquadFilterEQAudioProcessor::releaseResources ()
{
    filter.releaseResources();
}

template <typename TargetType, typename SourceType>
void castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples)
{
    auto dst = destination.getArrayOfWritePointers();
    auto src = source.getArrayOfReadPointers();

    for (int ch = 0; ch < numChannels; ++ch)
        for (int smp = 0; smp < numSamples; ++smp)
            dst[ch][smp] = static_cast<TargetType>(src[ch][smp]);
}

void BiquadFilterEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    filter.processBlock(buffer);
}

bool BiquadFilterEQAudioProcessor::hasEditor () const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BiquadFilterEQAudioProcessor::createEditor()
{
    return nullptr;
}

void BiquadFilterEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BiquadFilterEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

void BiquadFilterEQAudioProcessor::parameterChanged (const String& paramID, float newValue)
{
    
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter ()
{
    return new BiquadFilterEQAudioProcessor();
}
