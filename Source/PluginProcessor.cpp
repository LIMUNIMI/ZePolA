#include "PluginProcessor.h"
#include "DSP/Filter.h"
#include "DSP/Gradient.h"
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
        0.0f, juce::AudioParameterFloatAttributes {}.withLabel("dB")));

    for (int i = 0; i < n_elements; ++i)
    {
        juce::String i_str(i);
        juce::String ip1_str(i + 1);

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            MAGNITUDE_ID_PREFIX + i_str, "Magnitude " + ip1_str,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.00001f), 0.0f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            PHASE_ID_PREFIX + i_str, "Phase " + ip1_str,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.00001f), 0.0f,
            juce::AudioParameterFloatAttributes {}.withLabel(
                juce::CharPointer_UTF8("×π rad"))));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            GAIN_ID_PREFIX + i_str, "Gain " + ip1_str,
            juce::NormalisableRange<float>(-128.0f, 24.0f, 0.001f), 0.0f,
            juce::AudioParameterFloatAttributes {}.withLabel("dB")));
        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            TYPE_ID_PREFIX + i_str, "Type" + ip1_str,
            juce::StringArray {FilterElement::typeToString(0),
                               FilterElement::typeToString(1)},
            0));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            ACTIVE_ID_PREFIX + i_str, "Active " + ip1_str, false));
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
        auto i_str = juce::String(i);

        pushListener(MAGNITUDE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementMagnitude,
                         this, i, std::placeholders::_1)));
        pushListener(PHASE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementPhase, this,
                         i, std::placeholders::_1)));
        pushListener(GAIN_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementGainDb, this,
                         i, std::placeholders::_1)));
        pushListener(ACTIVE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementActiveTh,
                         this, i, std::placeholders::_1)));
        pushListener(TYPE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &PolesAndZerosEQAudioProcessor::setElementTypeF, this,
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
    gain.setGainDecibels(0.0f);
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
    VTSAudioProcessor::prepareToPlay(sampleRate, samplesPerBlock);
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
    return new PolesAndZerosEQAudioProcessorEditor(*this);
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
void PolesAndZerosEQAudioProcessor::setCurrentProgram(int) {}
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
void PolesAndZerosEQAudioProcessor::setElementGainDb(int i, float v)
{
    for (auto& fec : multiChannelCascade) fec[i].setGainDb(v);
}
void PolesAndZerosEQAudioProcessor::setElementActive(int i, bool v)
{
    for (auto& fec : multiChannelCascade) fec[i].setActive(v);
}
void PolesAndZerosEQAudioProcessor::setElementActiveTh(int i, float v)
{
    setElementActive(i, v > 0.5);
}
void PolesAndZerosEQAudioProcessor::setElementType(int i, FilterElement::Type v)
{
    for (auto& fec : multiChannelCascade) fec[i].setType(v);
}
void PolesAndZerosEQAudioProcessor::setElementTypeF(int i, float v)
{
    setElementType(i, FilterElement::floatToType(v));
}

// =============================================================================
int PolesAndZerosEQAudioProcessor::getNElements() const { return n_elements; }
std::complex<double> PolesAndZerosEQAudioProcessor::dtft(double omega) const
{
    return multiChannelCascade[0].dtft(omega)
           * static_cast<double>(gain.getGainLinear());
}
std::vector<std::array<double, 8>>
PolesAndZerosEQAudioProcessor::getCoefficients() const
{
    return multiChannelCascade[0].getCoefficients();
}
double PolesAndZerosEQAudioProcessor::getCascadePeakGain() const
{
    DifferentiableDTFT dDTFT(multiChannelCascade[0]);
    return juce::Decibels::gainToDecibels(dDTFT.peakFrequency()[1], -300.0)
           / 2.0;
}
double PolesAndZerosEQAudioProcessor::getElementAutoGain(int i) const
{
    return multiChannelCascade[0][i].getGainDb() - getCascadePeakGain();
}
void PolesAndZerosEQAudioProcessor::resetMemory()
{
    for (auto& cascade : multiChannelCascade) cascade.resetMemory();
}
void PolesAndZerosEQAudioProcessor::setAllActive(bool active)
{
    for (int i = 0; i < n_elements; ++i)
        setParameterValue(ACTIVE_ID_PREFIX + juce::String(i), active);
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
        setParameterValue(
            PHASE_ID_PREFIX + juce::String(i),
            static_cast<float>(multiChannelCascade[0][i].getPhase() * k));
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
        setParameterValue(TYPE_ID_PREFIX + juce::String(i),
                          FilterElement::typeToFloat(newType));
    }
}
void PolesAndZerosEQAudioProcessor::resetParameters()
{
    VTSAudioProcessor::resetParameters();
    // Set gain parameters to actual 0.0
    for (int i = 0; i < n_elements; ++i)
    {
        juce::String i_str(i);
        setParameterValue(GAIN_ID_PREFIX + i_str, 0.0f);
    }
    setParameterValue(GAIN_ID, 0.0f);
}
void PolesAndZerosEQAudioProcessor::setFilter(const double magnitude,
                                              const double phase,
                                              FilterElement::Type type,
                                              const int elementNr,
                                              double linearGain)
{
    if (elementNr > n_elements) return;
    auto i_str = juce::String(elementNr - 1);
    setParameterValue(ACTIVE_ID_PREFIX + i_str, false);

    setParameterValue(ACTIVE_ID_PREFIX + i_str, false);
    setParameterValue(MAGNITUDE_ID_PREFIX + i_str,
                      static_cast<float>(magnitude));
    setParameterValue(PHASE_ID_PREFIX + i_str, static_cast<float>(phase));
    setParameterValue(TYPE_ID_PREFIX + i_str, FilterElement::typeToFloat(type));
    setParameterValue(ACTIVE_ID_PREFIX + i_str, true);
    setParameterValue(GAIN_ID_PREFIX + i_str,
                      static_cast<float>(Decibels::gainToDecibels(
                          linearGain, FilterElement::gain_floor_db)));
}

// =============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PolesAndZerosEQAudioProcessor(8);
}
