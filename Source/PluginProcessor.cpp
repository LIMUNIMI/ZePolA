#include "PluginProcessor.h"
#include "Filter.h"
#include "Macros.h"
#include "Parameters.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

// =============================================================================
static std::vector<std::unique_ptr<juce::RangedAudioParameter>>
createParameterLayout(int n_elements)
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    // Refactored with listener
    params.push_back(
        std::make_unique<AudioParameterBool>(BYPASS_ID, "Bypass", false));

    // Not refactored
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
    params.push_back(std::make_unique<AudioParameterFloat>(
        MASTER_GAIN_NAME, "Gain (dB)", masterGainRange, MASTER_GAIN_DEFAULT));

    return params;
}
void PolesAndZerosEQAudioProcessor::appendListeners()
{
    pushListener(BYPASS_ID, new SimpleListener(std::bind(
                                &PolesAndZerosEQAudioProcessor::setBypassTh,
                                this, std::placeholders::_1)));
}

// =============================================================================
PolesAndZerosEQAudioProcessor::PolesAndZerosEQAudioProcessor(int n)
    : VTSAudioProcessor(createParameterLayout(n), getName())
    , n_elements(n)
    , pivotBuffer()
    , safetyFlag(false)
    , bypassed(false)
{
    allocateChannelsIfNeeded(1);
    Parameters::addListenerToAllParameters(valueTreeState, this);
    initializeListeners();
}

// =============================================================================
void PolesAndZerosEQAudioProcessor::allocateChannelsIfNeeded(int n)
{
    while (multiChannelCascade.size() < n)
        multiChannelCascade.push_back(FilterElementCascade(n_elements));
}
void PolesAndZerosEQAudioProcessor::resetChannels()
{
    multiChannelCascade.erase(multiChannelCascade.begin() + 1,
                              multiChannelCascade.end());
}

// =============================================================================
void PolesAndZerosEQAudioProcessor::prepareToPlay(double sampleRate,
                                                  int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels      = getTotalNumInputChannels();

    gainProcessor.prepare(spec);

    resetMemory();
}
bool PolesAndZerosEQAudioProcessor::isBusesLayoutSupported(
    const BusesLayout&) const
{
    return true;
}
void PolesAndZerosEQAudioProcessor::releaseResources()
{
    pivotBuffer.setSize(0, 0);
    resetChannels();
}
void PolesAndZerosEQAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    if (bypassed) return processBlockBypassed(buffer, midiMessages);

    // Ensure enough processors for input channels and reset memory of excess
    // ones
    int n_in_channels  = getTotalNumInputChannels();
    int n_out_channels = getTotalNumOutputChannels();
    int n_samples      = buffer.getNumSamples();
    allocateChannelsIfNeeded(n_in_channels);
    size_t n_processors = multiChannelCascade.size();
    for (int i = n_in_channels; i < n_processors; ++i)
        multiChannelCascade[i].resetMemory();

    pivotBuffer.makeCopyOf(buffer, true);
    {
        // Process in double precision
        auto channels = pivotBuffer.getArrayOfWritePointers();
        for (int c = 0; c < n_in_channels; ++c)
            multiChannelCascade[c].processBlock(channels[c], channels[c],
                                                n_samples);
    }
    buffer.makeCopyOf(pivotBuffer, true);

    if (n_in_channels < n_out_channels)
    {
        // Handle channel mismatch
        if (2 == n_out_channels)
        {
            // in the case mono-to-stereo, copy channel output
            buffer.copyFrom(1, 0, buffer, 0, 0, n_samples);
        }
        else
        {
            // in the case any-to-any, clear extra channels
            for (int c = n_in_channels; c < n_out_channels; ++c)
                buffer.clear(c, 0, n_samples);
        }
    }
    {
        // Apply gain
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> prc_ctx(block);
        gainProcessor.process(prc_ctx);
    }

    // Output safety check (not resetting)
    safetyFlag |= buffer.getMagnitude(0, n_samples) > 4;
    if (safetyFlag) buffer.clear();
}
void PolesAndZerosEQAudioProcessor::processBlockBypassed(
    juce::AudioBuffer<float>&, juce::MidiBuffer&)
{
    resetMemory();
}

// =============================================================================
juce::AudioProcessorEditor* PolesAndZerosEQAudioProcessor::createEditor()
{
    return new WrappedEditor(*this, valueTreeState);
}
bool PolesAndZerosEQAudioProcessor::hasEditor() const { return true; }

// =============================================================================
const juce::String PolesAndZerosEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
bool PolesAndZerosEQAudioProcessor::acceptsMidi() const { return false; }
bool PolesAndZerosEQAudioProcessor::producesMidi() const { return false; }
bool PolesAndZerosEQAudioProcessor::isMidiEffect() const { return false; }
double PolesAndZerosEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// =============================================================================
int PolesAndZerosEQAudioProcessor::getNumPrograms() { return 1; }
int PolesAndZerosEQAudioProcessor::getCurrentProgram() { return 0; }
void PolesAndZerosEQAudioProcessor::setCurrentProgram(int) { }
const juce::String PolesAndZerosEQAudioProcessor::getProgramName(int)
{
    return {};
}
void PolesAndZerosEQAudioProcessor::changeProgramName(int, const juce::String&)
{
}

// =============================================================================
void PolesAndZerosEQAudioProcessor::resetMemory()
{
    for (auto& cascade : multiChannelCascade) cascade.resetMemory();
}
void PolesAndZerosEQAudioProcessor::setAllActive(bool active)
{
    for (int i = 1; i <= n_elements; ++i)
        setParameterValue(ACTIVE_NAME + std::to_string(i), active);
}
void PolesAndZerosEQAudioProcessor::setInactive(const int elementNr)
{
    setParameterValue(ACTIVE_NAME + std::to_string(elementNr), false);
}
void PolesAndZerosEQAudioProcessor::setBypass(bool b) { bypassed = b; }
void PolesAndZerosEQAudioProcessor::setBypassTh(float b)
{
    setBypass(b > 0.5f);
}
void PolesAndZerosEQAudioProcessor::multiplyPhases(double k)
{
    for (int i = 0; i < n_elements; ++i)
        setParameterValue(PHASE_NAME + std::to_string(i + 1),
                          multiChannelCascade[0][i].getPhase() * k);
}
void PolesAndZerosEQAudioProcessor::doublePhases() { multiplyPhases(2.0); }
void PolesAndZerosEQAudioProcessor::halfPhases() { multiplyPhases(0.5); }
void PolesAndZerosEQAudioProcessor::swapPolesAndZeros()
{
    for (int i = 0; i < n_elements; ++i)
    {
        auto currentType = multiChannelCascade[0][i].getType();
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
        setParameterValue(TYPE_NAME + std::to_string(i + 1), newType);
    }
}
void PolesAndZerosEQAudioProcessor::setFilter(const double magnitude,
                                              const double phase,
                                              FilterElement::Type type,
                                              const int elementNr,
                                              double linearGain)
{
    if (elementNr > n_elements) return;
    setParameterValue(ACTIVE_NAME + std::to_string(elementNr), false);

    float gain = Decibels::gainToDecibels(linearGain, GAIN_FLOOR - 1.0);

    setParameterValue(MAGNITUDE_NAME + std::to_string(elementNr), magnitude);
    setParameterValue(PHASE_NAME + std::to_string(elementNr), phase);
    setParameterValue(TYPE_NAME + std::to_string(elementNr), !type);
    setParameterValue(ACTIVE_NAME + std::to_string(elementNr), true);
    setParameterValue(
        GAIN_NAME + std::to_string(elementNr),
        jmap(gain, GAIN_FLOOR, GAIN_CEILING, SLIDERS_FLOOR, SLIDERS_CEILING));
}

// =============================================================================
void PolesAndZerosEQAudioProcessor::parameterChanged(const String& parameterID,
                                                     float newValue)
{
    juce::String parameter;
    int elementNr;

    if (parameterID == "MSTR_GAIN")
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

// =============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolesAndZerosEQAudioProcessor(8);
}
