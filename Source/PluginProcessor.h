#pragma once
#include <JuceHeader.h>
#include "Filter.h"

class PolesAndZerosEQAudioProcessor  : public juce::AudioProcessor, public AudioProcessorValueTreeState::Listener
{
public:
    PolesAndZerosEQAudioProcessor();
    ~PolesAndZerosEQAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
    template <typename TargetType, typename SourceType>
    void castBuffer(AudioBuffer<TargetType>& destination, const AudioBuffer<SourceType>& source, const int numChannels, const int numSamples);
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true ;};

    const juce::String getName() const override {return JucePlugin_Name; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override {return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    
    void setParameterValue (juce::RangedAudioParameter* parameter, float value);
    
    void setUnactive (const int elementNr);
    
    std::complex<double> getPhiSpectrum(const double phi);
    double getElementGain(const int elementNr);
    void setElementGain (const int elementNr, double gain);
    
    std::vector<FilterElement> getFilterElementsChain();
    FilterElement getElementState(const int elementNr);
    
    void setEditorCallback(std::function<void()> callback)
    {
        editorCallback = callback;
    }
    
    void resetFilter ();
    
    float getCurrentGain();
    void setdBGain(double newdBGain);
    void setBypass(bool bypass);
    
    void doublePhases();
    void halfPhases();
    void swapPolesAndZeros();
    
    void turnOnOffAllElements(bool option);
    
    void setFilter(const double magnitude, const double phase, FilterElement::Type type, const int elementNr, const double gain = 1.0);
    
    void setGainFromMagnitudeAndPhase(const int elementNr);
    void setGainFromLabel(const int elementNr, double newGain);
    
    AudioProcessorValueTreeState parameters;

private:
    void parameterChanged(const String& parameterID, float newValue) override;
    
    std::vector<PolesAndZerosCascade> multiChannelCascade;
    
    juce::dsp::Gain<double> gainProcessor;
    
    bool active = true;
    
    std::function<void()> editorCallback;
    
    UndoManager undoManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolesAndZerosEQAudioProcessor);
};
