/*
  ==============================================================================

    PluginProcessor.cpp

    Copyright (c) 2025 Laboratorio di Informatica Musicale
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

  ==============================================================================
*/

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
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            TYPE_ID_PREFIX + i_str, "Type " + ip1_str, false));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            INVERTED_ID_PREFIX + i_str, "Inverted " + ip1_str, false));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            SINGLE_ID_PREFIX + i_str, "Single " + ip1_str, false));
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            ACTIVE_ID_PREFIX + i_str, "Active " + ip1_str, false));
    }

    return params;
}
void ZePolAudioProcessor::appendListeners()
{
    // The unsafe flag resetter listens to all parameters
    pushListenerForAllParameters(new TriggerListener(
        std::bind(&ZePolAudioProcessor::markAsSafe, this, true)));

    pushListener(BYPASS_ID, new SimpleListener(std::bind(
                                &ZePolAudioProcessor::setBypassTh,
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
                         &ZePolAudioProcessor::setElementMagnitude,
                         this, i, std::placeholders::_1)));
        pushListener(PHASE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &ZePolAudioProcessor::setElementPhase, this,
                         i, std::placeholders::_1)));
        pushListener(GAIN_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &ZePolAudioProcessor::setElementGainDb, this,
                         i, std::placeholders::_1)));
        pushListener(ACTIVE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &ZePolAudioProcessor::setElementActiveTh,
                         this, i, std::placeholders::_1)));
        pushListener(INVERTED_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &ZePolAudioProcessor::setElementInvertedTh,
                         this, i, std::placeholders::_1)));
        pushListener(SINGLE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &ZePolAudioProcessor::setElementSingleTh,
                         this, i, std::placeholders::_1)));
        pushListener(TYPE_ID_PREFIX + i_str,
                     new SimpleListener(std::bind(
                         &ZePolAudioProcessor::setElementTypeTh, this,
                         i, std::placeholders::_1)));
    }
}

// =============================================================================
ZePolAudioProcessor::ZePolAudioProcessor(int n)
    : VTSAudioProcessor(createParameterLayout(n), getName())
    , n_elements(n)
    , pivotBuffer()
    , unsafe(juce::var(false))
    , bypassed(false)
{
    allocateChannelsIfNeeded(1);
    gain.setGainDecibels(0.0f);
    initializeListeners();
}

// =============================================================================
void ZePolAudioProcessor::allocateChannelsIfNeeded(int n)
{
    while (multiChannelCascade.size() < n)
        multiChannelCascade.push_back(FilterElementCascade(
            (multiChannelCascade.empty()) ? n_elements
                                          : multiChannelCascade[0]));
}
void ZePolAudioProcessor::resetChannels()
{
    multiChannelCascade.erase(multiChannelCascade.begin() + 1,
                              multiChannelCascade.end());
}

// =============================================================================
void ZePolAudioProcessor::prepareToPlay(double sampleRate,
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
bool ZePolAudioProcessor::isBusesLayoutSupported(
    const BusesLayout&) const
{
    return true;
}
void ZePolAudioProcessor::releaseResources()
{
    pivotBuffer.setSize(0, 0);
    resetChannels();
}
void ZePolAudioProcessor::processBlockExtraChannels(
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
void ZePolAudioProcessor::processBlock(
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
    if (!unsafe.getValue()) markAsSafe(buffer.getMagnitude(0, n_samples) < 4);
    if (unsafe.getValue()) buffer.clear();
}
void ZePolAudioProcessor::processBlockBypassed(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    processBlockExtraChannels(buffer);
    resetMemory();
}

// =============================================================================
juce::AudioProcessorEditor* ZePolAudioProcessor::createEditor()
{
    return new ZePolAudioProcessorEditor(*this);
}
bool ZePolAudioProcessor::hasEditor() const { return true; }

// =============================================================================
const juce::String ZePolAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
bool ZePolAudioProcessor::acceptsMidi() const { return false; }
bool ZePolAudioProcessor::producesMidi() const { return false; }
bool ZePolAudioProcessor::isMidiEffect() const { return false; }
double ZePolAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// =============================================================================
int ZePolAudioProcessor::getNumPrograms() { return 1; }
int ZePolAudioProcessor::getCurrentProgram() { return 0; }
void ZePolAudioProcessor::setCurrentProgram(int) {}
const juce::String ZePolAudioProcessor::getProgramName(int)
{
    return {};
}
void ZePolAudioProcessor::changeProgramName(int, const juce::String&)
{
}

// =============================================================================
void ZePolAudioProcessor::setElementMagnitude(int i, float v)
{
    for (auto& fec : multiChannelCascade) fec[i].setMagnitude(v);
}
void ZePolAudioProcessor::setElementPhase(int i, float v)
{
    for (auto& fec : multiChannelCascade) fec[i].setPhase(v);
}
void ZePolAudioProcessor::setElementGainDb(int i, float v)
{
    for (auto& fec : multiChannelCascade) fec[i].setGainDb(v);
}
void ZePolAudioProcessor::setElementActive(int i, bool v)
{
    for (auto& fec : multiChannelCascade) fec[i].setActive(v);
}
void ZePolAudioProcessor::setElementActiveTh(int i, float v)
{
    setElementActive(i, v > 0.5);
}
void ZePolAudioProcessor::setElementInverted(int i, bool v)
{
    for (auto& fec : multiChannelCascade) fec[i].setInverted(v);
    // Magnitude inversion is allowed only for zeros
    if (v) setParameterValue(TYPE_ID_PREFIX + juce::String(i), 0.0f);
}
void ZePolAudioProcessor::setElementInvertedTh(int i, float v)
{
    setElementInverted(i, v > 0.5);
}
void ZePolAudioProcessor::setElementSingle(int i, bool v)
{
    for (auto& fec : multiChannelCascade) fec[i].setSingle(v);
}
void ZePolAudioProcessor::setElementSingleTh(int i, float v)
{
    setElementSingle(i, v > 0.5);
}
void ZePolAudioProcessor::setElementType(int i, bool v)
{
    for (auto& fec : multiChannelCascade) fec[i].setType(v);
    // Poles cannot have inverted magnitude
    if (v) setParameterValue(INVERTED_ID_PREFIX + juce::String(i), 0.0f);
}
void ZePolAudioProcessor::setElementTypeTh(int i, float v)
{
    setElementType(i, v > 0.5f);
}

// =============================================================================
int ZePolAudioProcessor::getNElements() const { return n_elements; }
std::complex<double> ZePolAudioProcessor::dtft(double omega) const
{
    return multiChannelCascade[0].dtft(omega)
           * static_cast<double>(gain.getGainLinear());
}
std::vector<std::array<double, 8>>
ZePolAudioProcessor::getCoefficients() const
{
    return multiChannelCascade[0].getCoefficients();
}
double ZePolAudioProcessor::getCascadePeakGain() const
{
    DifferentiableDTFT dDTFT(multiChannelCascade[0]);
    return juce::Decibels::gainToDecibels(dDTFT.peakFrequency()[1], -300.0)
           / 2.0;
}
double ZePolAudioProcessor::getElementAutoGain(int i) const
{
    return multiChannelCascade[0][i].getGainDb() - getCascadePeakGain();
}
void ZePolAudioProcessor::resetMemory()
{
    for (auto& cascade : multiChannelCascade) cascade.resetMemory();
}
void ZePolAudioProcessor::setAllActive(bool active)
{
    for (int i = 0; i < n_elements; ++i)
        setParameterValue(ACTIVE_ID_PREFIX + juce::String(i), active);
}
void ZePolAudioProcessor::setBypass(bool b) { bypassed = b; }
void ZePolAudioProcessor::setBypassTh(float b)
{
    setBypass(b > 0.5f);
}
void ZePolAudioProcessor::addUnsafeOutputListener(
    juce::Value::Listener* uol)
{
    unsafe.addListener(uol);
    uol->valueChanged(unsafe);
}
void ZePolAudioProcessor::removeUnsafeOutputListener(
    juce::Value::Listener* uol)
{
    unsafe.removeListener(uol);
}
void ZePolAudioProcessor::markAsSafe(bool b) { unsafe = !b; }
void ZePolAudioProcessor::multiplyPhases(double k)
{
    for (int i = 0; i < n_elements; ++i)
        setParameterValue(
            PHASE_ID_PREFIX + juce::String(i),
            static_cast<float>(multiChannelCascade[0][i].getPhase() * k));
}
void ZePolAudioProcessor::doublePhases() { multiplyPhases(2.0); }
void ZePolAudioProcessor::halfPhases() { multiplyPhases(0.5); }
void ZePolAudioProcessor::swapPolesAndZeros()
{
    for (int i = 0; i < n_elements; ++i)
        setParameterValue(
            TYPE_ID_PREFIX + juce::String(i),
            static_cast<float>(!multiChannelCascade[0][i].getType()));
}
void ZePolAudioProcessor::resetParameters()
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

// =============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZePolAudioProcessor(10);
}
