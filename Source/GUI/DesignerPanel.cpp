#include "DesignerPanel.h"
#include "../DSP/FilterDesign.h"
#include "LookAndFeel.h"

// =============================================================================
DesignerPanel::DesignerPanel(VTSAudioProcessor& p)
    : processor(p), panelLabel("", "FILTER DESIGN"), applyButton("APPLY")
{
    addAndMakeVisible(panelLabel);
    addAndMakeVisible(typeCBox);
    addAndMakeVisible(shapeCBox);
    addAndMakeVisible(orderSlider);
    addAndMakeVisible(cutoffSlider);
    addAndMakeVisible(autoButton);
    addAndMakeVisible(applyButton);

    panelLabel.setJustificationType(juce::Justification::centred);
    orderSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    Button_setOnOffLabel(autoButton, "MAN", "AUTO");

    // !REMOVE THIS! Test FilterDesign
    FilterParameters params(processor.getSampleRate());
    params.cutoff = 500.0;
    params.order  = 4;
    params.computeZPK();

    auto degree = params.zpk.degree();
    auto k_db   = juce::Decibels::gainToDecibels(params.zpk.gain, -1000.0)
                / (params.zpk.zeros.size() + params.zpk.poles.size());
    for (auto i = 0; i < degree; ++i)
    {
        if (i < params.zpk.zeros.size())
            DBG("  Z(" << abs(params.zpk.zeros[i]) << "; "
                       << std::arg(params.zpk.zeros[i])
                              / juce::MathConstants<double>::pi
                       << ") " << k_db << "dB");
        if (i < params.zpk.poles.size())
            DBG("  P(" << abs(params.zpk.poles[i]) << "; "
                       << std::arg(params.zpk.poles[i])
                              / juce::MathConstants<double>::pi
                       << ") " << k_db << "dB");
    }
    // !REMOVE THIS!
}

// =============================================================================
void DesignerPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto b = claf->getPanelInnerRect(getLocalBounds());
        auto h = b.getHeight() / 18;

        panelLabel.setBounds(b.removeFromTop(h));
        b.removeFromTop(h);
        typeCBox.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        shapeCBox.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        orderSlider.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        cutoffSlider.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        auto last_row = b.removeFromTop(h);
        autoButton.setBounds(
            last_row.withTrimmedRight(last_row.getWidth() * 53 / 100));
        applyButton.setBounds(
            last_row.withTrimmedLeft(last_row.getWidth() * 53 / 100));

        orderSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                    orderSlider.getTextBoxWidth(),
                                    orderSlider.getTextBoxHeight());
        cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                     cutoffSlider.getTextBoxWidth(),
                                     cutoffSlider.getTextBoxHeight());
    }
}
