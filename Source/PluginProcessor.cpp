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

    params.push_back(
        std::make_unique<AudioParameterBool>(BYPASS_ID, "Bypass", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        GAIN_ID, "Gain", juce::NormalisableRange<float>(-48.0f, 48.0f, 0.01f),
        0.0f, "dB"));

    for (int i = 0; i < n_elements; ++i)
    {
        auto i_str   = std::to_string(i);
        auto ip1_str = std::to_string(i + 1);

        params.push_back(std::make_unique<AudioParameterFloat>(
            MAGNITUDE_ID_PREFIX + i_str, "Magnitude " + ip1_str,
            NormalisableRange<float>(0.0f, 1.0f, 0.00001f), 0.0f));
        params.push_back(std::make_unique<AudioParameterFloat>(
            PHASE_ID_PREFIX + i_str, "Phase " + ip1_str,
            NormalisableRange<float>(0.0f, 1.0f, 0.00001f), 0.0f));
    }

    // ^^^ Refactored with listener
    // vvv Not refactored

    juce::NormalisableRange<float> gainRange(GAIN_FLOOR, GAIN_CEILING,
                                             GAIN_INTERVAL);

    for (int i = 0; i < n_elements; ++i)
    {
        std::string number = std::to_string(i + 1);

        params.push_back(std::make_unique<AudioParameterBool>(
            ACTIVE_NAME + number, "Active " + number, ACTIVE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterBool>(
            TYPE_NAME + number, "Type" + number, TYPE_DEFAULT));
        params.push_back(std::make_unique<AudioParameterFloat>(
            GAIN_NAME + number, "Gain " + number, gainRange, GAIN_DEFAULT));
    }

    return params;
}
void PolesAndZerosEQAudioProcessor::appendListeners()
{
    // The unsafe flag resetter listens to all parameters
    pushListenerForAllParameters(
        new SimpleListener(std::bind(&PolesAndZerosEQAudioProcessor::markAsSafe,
                                     this, std::placeholders::_1)));

    pushListener(BYPASS_ID, new SimpleListener(std::bind(
                                &PolesAndZerosEQAudioProcessor::setBypassTh,
                                this, std::placeholders::_1)));
    pushListener(GAIN_ID, new SimpleListener(std::bind(
                              &juce::dsp::Gain<float>::setGainDecibels, &gain,
                              std::placeholders::_1)));
    // Listeners for element parameters
    for (int i = 0; i < n_elements; ++i)
    {
        auto i_str = std::to_string(i);

        pushListener(MAGNITUDE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementMagnitude,
                         this, i, std::placeholders::_1)));
        pushListener(PHASE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementPhase, this,
                         i, std::placeholders::_1)));
    }
}

// =============================================================================
PolesAndZerosEQAudioProcessor::PolesAndZerosEQAudioProcessor(int n)
    : VTSAudioProcessor(createParameterLayout(n), getName())
    , n_elements(n)
    , pivotBuffer()
    , unsafe(false)
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
        multiChannelCascade.push_back(FilterElementCascade(
            (multiChannelCascade.empty()) ? n_elements
                                          : multiChannelCascade[0]));
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

    gain.prepare(spec);

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
void PolesAndZerosEQAudioProcessor::processBlockExtraChannels(
    juce::AudioBuffer<float>& buffer)
{
    int n_in_channels  = getTotalNumInputChannels();
    int n_out_channels = getTotalNumOutputChannels();
    if (n_in_channels < n_out_channels)
    {
        int n_samples = buffer.getNumSamples();
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
}
void PolesAndZerosEQAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    if (bypassed) return processBlockBypassed(buffer, midiMessages);

    // Ensure enough processors for input channels and reset memory of excess
    // ones
    int n_in_channels = getTotalNumInputChannels();
    int n_samples     = buffer.getNumSamples();
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

    // Handle channel mismatch
    processBlockExtraChannels(buffer);
    {
        // Apply gain
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> prc_ctx(block);
        gain.process(prc_ctx);
    }

    // Output safety check (not resetting)
    unsafe |= buffer.getMagnitude(0, n_samples) > 4;
    if (unsafe) buffer.clear();
}
void PolesAndZerosEQAudioProcessor::processBlockBypassed(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    processBlockExtraChannels(buffer);
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
void PolesAndZerosEQAudioProcessor::setElementMagnitude(int i, float v)
{
    for (auto& fec : multiChannelCascade) fec[i].setMagnitude(v);
}
void PolesAndZerosEQAudioProcessor::setElementPhase(int i, float v)
{
    for (auto& fec : multiChannelCascade) fec[i].setPhase(v);
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
void PolesAndZerosEQAudioProcessor::markAsSafe(float)
{
    unsafe = false;
    if (editorCallback) editorCallback();
}
void PolesAndZerosEQAudioProcessor::multiplyPhases(double k)
{
    for (int i = 0; i < n_elements; ++i)
        setParameterValue(PHASE_ID_PREFIX + std::to_string(i),
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
    auto i_str = std::to_string(elementNr - 1);

    setParameterValue(MAGNITUDE_ID_PREFIX + i_str, magnitude);
    setParameterValue(PHASE_ID_PREFIX + i_str, phase);
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
    if (parameterID == GAIN_ID || parameterID == BYPASS_ID) return;

    auto parameter = parameterID.substring(0, 1);
    auto elementNr = parameterID.substring(1).getIntValue();
    newValue       = static_cast<double>(newValue);

    if (parameter == "A")
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
}

// =============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolesAndZerosEQAudioProcessor(8);
}
