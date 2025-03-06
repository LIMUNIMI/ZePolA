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
        auto rect = claf->getPanelInnerRect(getLocalBounds());
        bypassButton.setBounds(rect.removeFromBottom(claf->resizeSize(28)));
        bypassLabel.setBounds(rect.removeFromBottom(claf->resizeSize(12)));
        claf->resizeToggleButton(bypassButton);
        rect.removeFromBottom(claf->resizeSize(12));

        gainLabel.setBounds(rect.removeFromTop(claf->resizeSize(12)));
        gainSlider.setBounds(rect);
        gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false,
                                   gainSlider.getWidth(),
                                   bypassButton.getHeight());
    }
}
