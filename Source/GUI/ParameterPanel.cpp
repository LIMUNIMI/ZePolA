#include "ParameterPanel.h"
#include "LookAndFeel.h"

// =============================================================================
ParameterStrip::ParameterStrip(VTSAudioProcessor& p, int i)
    : mSliderAttachment(
        p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                         juce::Slider>(MAGNITUDE_ID_PREFIX + juce::String(i),
                                       mSlider))
    , pSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(PHASE_ID_PREFIX + juce::String(i),
                                         pSlider))
    , aButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(ACTIVE_ID_PREFIX + juce::String(i),
                                         aButton))
    , tButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(TYPE_ID_PREFIX + juce::String(i),
                                         tButton))
{
    addAndMakeVisible(mSlider);
    addAndMakeVisible(pSlider);
    addAndMakeVisible(aButton);
    addAndMakeVisible(tButton);
}

// =============================================================================
void ParameterStrip::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto rects = claf->splitProportionalStrip(getLocalBounds());
        jassert(rects.size() == 6);
        mSlider.setBounds(rects[0]);
        pSlider.setBounds(rects[1]);
        tButton.setBounds(rects[3]);
        aButton.setBounds(rects[4]);
    }
}

// =============================================================================
ParameterPanel::ParameterPanel(VTSAudioProcessor& p, size_t n)
{
    for (auto i = 0; i < n; ++i)
        strips.push_back(std::make_unique<ParameterStrip>(p, i));
    for (auto& strip : strips) addAndMakeVisible(*strip.get());
}

// =============================================================================
void ParameterPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->splitProportionalPanel(
            claf->getPanelInnerRect(getLocalBounds()));
        jassert(regions.size() == 4);

        auto n = strips.size();
        auto rects
            = claf->splitProportional(regions[1], std::vector<int>(n, 1), true);
        for (auto i = 0; i < n; ++i) strips[i]->setBounds(rects[i]);
    }
}
