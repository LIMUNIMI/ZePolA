/*
  ==============================================================================

    MasterPanel.cpp

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
