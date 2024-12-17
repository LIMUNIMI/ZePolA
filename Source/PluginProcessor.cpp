#include "PluginProcessor.h"
#include "Filter.h"
#include "Macros.h"
#include "Parameters.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

PolesAndZerosEQAudioProcessor::PolesAndZerosEQAudioProcessor()
    : parameters(*this, &undoManager, "PolesAndZero-EQ",
                 Parameters::createParameterLayout())
{
    Parameters::addListenerToAllParameters(parameters, this);

    for (int i = 0; i < STEREO; ++i)
        multiChannelCascade.push_back(
            FilterElementCascade(NUMBER_OF_FILTER_ELEMENTS));
}

PolesAndZerosEQAudioProcessor::~PolesAndZerosEQAudioProcessor() { }

void PolesAndZerosEQAudioProcessor::prepareToPlay(double sampleRate,
                                                  int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels      = getTotalNumInputChannels();

    gainProcessor.prepare(spec);
    gainProcessor.setGainDecibels(MASTER_GAIN_DEFAULT);

    for (auto& cascade : multiChannelCascade) cascade.resetMemory();
}

void PolesAndZerosEQAudioProcessor::releaseResources() { }

void PolesAndZerosEQAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!active) return;

    juce::ScopedNoDenormals noDenormals;

    const auto numChannels = buffer.getNumChannels();
    const auto numSamples  = buffer.getNumSamples();

    if (numChannels > multiChannelCascade.size())
    {
        auto currentNumCh = multiChannelCascade.size();
        for (int i = currentNumCh; i < numChannels; ++i)
            multiChannelCascade.push_back(
                FilterElementCascade(multiChannelCascade[0]));
    }
    else if (multiChannelCascade.size() > numChannels)
        for (int i = numChannels; i < multiChannelCascade.size(); ++i)
            multiChannelCascade[i].resetMemory();

    AudioBuffer<double> doubleBuffer(numChannels, numSamples);
    doubleBuffer.makeCopyOf(buffer, true);

    auto bufferData = doubleBuffer.getArrayOfWritePointers();

    for (int ch = 0; ch < numChannels; ++ch)
        multiChannelCascade[ch].processBlock(bufferData[ch], bufferData[ch],
                                             numSamples);

    juce::dsp::AudioBlock<double> block(doubleBuffer);
    gainProcessor.process(juce::dsp::ProcessContextReplacing<double>(block));

    buffer.makeCopyOf(doubleBuffer, true);

    safetyFlag |= buffer.getMagnitude(0, numSamples) > 4;

    if (safetyFlag) buffer.clear();
}

void PolesAndZerosEQAudioProcessor::processBlockBypassed(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (auto& cascade : multiChannelCascade) cascade.resetMemory();
}

juce::AudioProcessorEditor* PolesAndZerosEQAudioProcessor::createEditor()
{
    return new WrappedEditor(*this, parameters);
}

void PolesAndZerosEQAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PolesAndZerosEQAudioProcessor::setStateInformation(const void* data,
                                                        int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
}

bool PolesAndZerosEQAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
    return true;
}

void PolesAndZerosEQAudioProcessor::parameterChanged(const String& parameterID,
                                                     float newValue)
{
    juce::String parameter;
    int elementNr;

    if (parameterID == "BYPASS")
    {
        active = !(newValue > 0.5f);
        if (!active)
            for (auto& cascade : multiChannelCascade) cascade.resetMemory();
        return;
    }
    else if (parameterID == "MSTR_GAIN")
    {
        gainProcessor.setGainLinear(
            Decibels::decibelsToGain(newValue, MASTER_GAIN_FLOOR - 1.0f));
    }
    else
    {
        parameter = parameterID.substring(0, 1);
        elementNr = parameterID.substring(1).getIntValue();

        newValue = static_cast<double>(newValue);
    }

    if (parameter == "M")
    {
        for (auto& cascade : multiChannelCascade)
            cascade[elementNr - 1].setMagnitude(newValue);
    }
    else if (parameter == "P")
    {
        for (auto& cascade : multiChannelCascade)
            cascade[elementNr - 1].setPhase(newValue);
    }
    else if (parameter == "A")
    {
        for (auto& cascade : multiChannelCascade)
            cascade[elementNr - 1].setActive(newValue > 0.5);
    }
    else if (parameter == "T")
    {
        for (auto& cascade : multiChannelCascade)
            cascade[elementNr - 1].setType((newValue > 0.5)
                                               ? FilterElement::Type::ZERO
                                               : FilterElement::Type::POLE);
    }
    else if (parameter == "G")
    {
        for (auto& cascade : multiChannelCascade)
            cascade[elementNr - 1].setGainDb(newValue);
    }

    if (editorCallback)
    {
        editorCallback();
    }

    safetyFlag = false;
}

void PolesAndZerosEQAudioProcessor::setInactive(const int elementNr)
{
    Parameters::setParameterValue(
        parameters.getParameter(ACTIVE_NAME + std::to_string(elementNr)),
        false);
}

double PolesAndZerosEQAudioProcessor::getElementGain(const int elementNr)
{
    return multiChannelCascade[0][elementNr - 1].getGain();
}

void PolesAndZerosEQAudioProcessor::resetParams()
{
    size_t n = multiChannelCascade[0].size();
    RangedAudioParameter* p;
    for (int i = 1; i <= n; ++i)
        for (auto k :
             {MAGNITUDE_NAME, PHASE_NAME, TYPE_NAME, ACTIVE_NAME, GAIN_NAME})
            Parameters::resetParameterValue(
                parameters.getParameter(k + std::to_string(i)));
    for (auto k : {MASTER_GAIN_NAME, FILTER_BYPASS_NAME})
        Parameters::resetParameterValue(parameters.getParameter(k));
    for (auto& cascade : multiChannelCascade) cascade.resetMemory();
}

float PolesAndZerosEQAudioProcessor::getCurrentGain()
{
    return gainProcessor.getGainLinear();
}

void PolesAndZerosEQAudioProcessor::setBypass(bool bypass)
{
    Parameters::setParameterValue(parameters.getParameter(FILTER_BYPASS_NAME),
                                  bypass);
}

void PolesAndZerosEQAudioProcessor::multiplyPhases(double k)
{
    size_t n = multiChannelCascade[0].size();
    for (int i = 0; i < n; ++i)
    {
        Parameters::setParameterValue(
            parameters.getParameter(PHASE_NAME + std::to_string(i + 1)),
            multiChannelCascade[0][i].getPhase() * k);
    }
}

void PolesAndZerosEQAudioProcessor::doublePhases() { multiplyPhases(2.0); }

void PolesAndZerosEQAudioProcessor::halfPhases() { multiplyPhases(0.5); }

void PolesAndZerosEQAudioProcessor::swapPolesAndZeros()
{
    size_t n = multiChannelCascade[0].size();
    for (int i = 1; i <= n; ++i)
    {
        auto currentType = multiChannelCascade[0][i - 1].getType();
        FilterElement::Type newType;
        switch (currentType)
        {
        default:
            UNHANDLED_SWITCH_CASE(
                "Unhandled case for filter element type. Defaulting to 'ZERO'");
        case (FilterElement::Type::ZERO):
            newType = FilterElement::Type::POLE;
            break;
        case (FilterElement::Type::POLE):
            newType = FilterElement::Type::ZERO;
            break;
        }
        Parameters::setParameterValue(
            parameters.getParameter(TYPE_NAME + std::to_string(i)), newType);
    }
}

void PolesAndZerosEQAudioProcessor::setAllActive(bool active)
{
    size_t n = multiChannelCascade[0].size();
    for (int i = 1; i <= n; ++i)
        Parameters::setParameterValue(
            parameters.getParameter(ACTIVE_NAME + std::to_string(i)), active);
}

void PolesAndZerosEQAudioProcessor::setFilter(const double magnitude,
                                              const double phase,
                                              FilterElement::Type type,
                                              const int elementNr,
                                              double linearGain)
{
    if (elementNr > NUMBER_OF_FILTER_ELEMENTS) return;
    Parameters::setParameterValue(
        parameters.getParameter(ACTIVE_NAME + std::to_string(elementNr)),
        false);

    float gain = Decibels::gainToDecibels(linearGain, GAIN_FLOOR - 1.0);

    Parameters::setParameterValue(
        parameters.getParameter(MAGNITUDE_NAME + std::to_string(elementNr)),
        magnitude);
    Parameters::setParameterValue(
        parameters.getParameter(PHASE_NAME + std::to_string(elementNr)), phase);
    Parameters::setParameterValue(
        parameters.getParameter(TYPE_NAME + std::to_string(elementNr)), !type);
    Parameters::setParameterValue(
        parameters.getParameter(ACTIVE_NAME + std::to_string(elementNr)), true);
    Parameters::setParameterValue(
        parameters.getParameter(GAIN_NAME + std::to_string(elementNr)),
        jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolesAndZerosEQAudioProcessor();
}
