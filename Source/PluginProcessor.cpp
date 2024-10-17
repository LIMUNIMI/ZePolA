#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "PluginEditor.h"
#include "Filter.h"

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

template <typename TargetType, typename SourceType>
void PolesAndZerosEQAudioProcessor::castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples)
{
    auto dst = destination.getArrayOfWritePointers();
    auto src = source.getArrayOfReadPointers();

    for (int ch = 0; ch < numChannels; ++ch)
        for (int smp = 0; smp < numSamples; ++smp)
            dst[ch][smp] = static_cast<TargetType>(src[ch][smp]);
}

void PolesAndZerosEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!active) return;
    
    juce::ScopedNoDenormals noDenormals;
    
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();
    
    AudioBuffer<double> doubleBuffer(numChannels, numSamples);
    castBuffer(doubleBuffer, buffer, numChannels, numSamples);
    
    auto bufferData = doubleBuffer.getArrayOfWritePointers();
    
    for (int ch = 0; ch < numChannels; ++ ch)
        filter.processBlock(bufferData[ch], numSamples);
    
    juce::dsp::AudioBlock<double> block (doubleBuffer);
    gainProcessor.process(juce::dsp::ProcessContextReplacing<double> (block));
    
    castBuffer(buffer, doubleBuffer, numChannels, numSamples);
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
        gainProcessor.setGainLinear(Decibels::decibelsToGain(newValue, -48.0f));
    }
    
    juce::String parameter = parameterID.substring(0, 1);
    const int elementNr = parameterID.substring(1).getIntValue();
    
    newValue = static_cast<double>(newValue);

    if (parameter == "M")
    {
        filter.setElementMagnitude(elementNr, newValue);
    }
    else if (parameter == "P")
    {
        filter.setElementPhase(elementNr, newValue);
    }
    else if (parameter == "A")
    {
        filter.setUnsetElementActive(elementNr, newValue > 0.5);
    }
    else if (parameter == "T")
    {
        filter.setElementType(elementNr, newValue > 0.5);
    }
    
    if (editorCallback)
    {
        editorCallback();
    }
}

void PolesAndZerosEQAudioProcessor::setParameterValue(juce::RangedAudioParameter *parameter, float value)
{
    parameter->beginChangeGesture();
    parameter->setValueNotifyingHost(value);
    parameter->endChangeGesture();
}

void PolesAndZerosEQAudioProcessor::setUnactive (const int elementNr)
{
    setParameterValue(parameters.getParameter(ACTIVE_NAME + std::to_string(elementNr)), ACTIVE_DEFAULT);
}

std::complex<double> PolesAndZerosEQAudioProcessor::getPhiSpectrum (const double phi)
{
    return filter.getPhiSpectrum(phi);
}

double PolesAndZerosEQAudioProcessor::getElementGain (const int elementNr)
{
    return filter.getElementGain(elementNr);
}

std::vector<FilterElement> PolesAndZerosEQAudioProcessor::getFilterElementsChain ()
{
    return filter.getElementsChain();
}

FilterElement PolesAndZerosEQAudioProcessor::getElementState (const int elementNr)
{
    auto elements = getFilterElementsChain();
    
    for (int i = 0; i < NUMBER_OF_FILTER_ELEMENTS; ++ i)
        if (i == elementNr - 1)
            return elements[i];
}

void PolesAndZerosEQAudioProcessor::resetFilter ()
{
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        setParameterValue(parameters.getParameter(MAGNITUDE_NAME + std::to_string(i)), MAGNITUDE_DEFAULT);
        setParameterValue(parameters.getParameter(PHASE_NAME + std::to_string(i)), PHASE_DEFAULT);
        setParameterValue(parameters.getParameter(TYPE_NAME + std::to_string(i)), TYPE_DEFAULT);
        setParameterValue(parameters.getParameter(ACTIVE_NAME + std::to_string(i)), ACTIVE_DEFAULT);
    }
    setParameterValue(parameters.getParameter(GAIN_NAME), jmap(GAIN_DEFAULT, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
    setParameterValue(parameters.getParameter(FILTER_BYPASS_NAME), BYPASS_DEFAULT);
    
    filter.memoryReset();
}

float PolesAndZerosEQAudioProcessor::getCurrentGain ()
{
    return gainProcessor.getGainLinear();
}

void PolesAndZerosEQAudioProcessor::setBypass(bool bypass)
{
    setParameterValue(parameters.getParameter(FILTER_BYPASS_NAME), bypass);
}

void PolesAndZerosEQAudioProcessor::doublePhases()
{
    auto elements = getFilterElementsChain();
    double currentPhase;
    double newPhase;
    
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        currentPhase = elements[i - 1].getPhase();
        if (currentPhase > (PHASE_CEILING / 2))
            newPhase = 1.0;
        else
            newPhase = currentPhase * 2.0;
        setParameterValue(parameters.getParameter(PHASE_NAME + std::to_string(i)), newPhase);
    }
}

void PolesAndZerosEQAudioProcessor::halfPhases()
{
    auto elements = getFilterElementsChain();
    double currentPhase;
    
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        currentPhase = elements[i - 1].getPhase();
        setParameterValue(parameters.getParameter(PHASE_NAME + std::to_string(i)), currentPhase * 0.5);
    }
}

void PolesAndZerosEQAudioProcessor::swapPolesAndZeros()
{
    auto elements = getFilterElementsChain();
    bool newType;
    
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
    {
        newType = true;
        auto currentType = elements[i - 1].getType();
        if (currentType == FilterElement::ZERO)
        {
            newType = false;
            if (elements[i - 1].getMagnitude() == 1.0)
                setParameterValue(parameters.getParameter(MAGNITUDE_NAME + std::to_string(i)), POLE_MAX_MAGNITUDE);
        }
        setParameterValue(parameters.getParameter(TYPE_NAME + std::to_string(i)), newType);
    }
}

void PolesAndZerosEQAudioProcessor::turnOnOffAllElements(bool option)
{
    bool active = false;
    
    if (option)
        active = true;
    for (int i = 1; i <= NUMBER_OF_FILTER_ELEMENTS; ++ i)
        setParameterValue(parameters.getParameter(ACTIVE_NAME + std::to_string(i)), active);
}

void PolesAndZerosEQAudioProcessor::setFilter(const double magnitude, const double phase, FilterElement::Type type, const int elementNr)
{
    if (elementNr > NUMBER_OF_FILTER_ELEMENTS)
        return;
    
    setParameterValue(parameters.getParameter(MAGNITUDE_NAME + std::to_string(elementNr)), magnitude);
    setParameterValue(parameters.getParameter(PHASE_NAME + std::to_string(elementNr)), phase);
    setParameterValue(parameters.getParameter(TYPE_NAME + std::to_string(elementNr)), !type);
    setParameterValue(parameters.getParameter(ACTIVE_NAME + std::to_string(elementNr)), true);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter ()
{
    return new PolesAndZerosEQAudioProcessor();
}


