#include "MasterPanel.h"
#include "LookAndFeel.h"

// =============================================================================
MasterPanel::MasterPanel(VTSAudioProcessor& p)
    : gainLabel("", "OUTPUT GAIN")
    , bypassLabel("", "BYPASS")
    , gainSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(GAIN_ID, gainSlider))
    , bypassButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(BYPASS_ID, bypassButton))
{
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(bypassLabel);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(bypassButton);

    gainLabel.setJustificationType(juce::Justification::centred);
    bypassLabel.setJustificationType(juce::Justification::centred);

    gainSlider.setSliderStyle(juce::Slider::LinearVertical);
}
void MasterPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->configureMasterPanel(getLocalBounds());
        gainLabel.setBounds(regions[0]);
        gainSlider.setBounds(regions[1]);
        bypassLabel.setBounds(regions[2]);
        bypassButton.setBounds(regions[3]);

        claf->resizeToggleButton(bypassButton);
        gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                                   gainSlider.getWidth(),
                                   bypassButton.getHeight());
    }
}
