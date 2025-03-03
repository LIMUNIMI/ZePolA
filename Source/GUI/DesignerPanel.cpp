#include "DesignerPanel.h"
#include "../DSP/FilterDesign.h"
#include "LookAndFeel.h"

// =============================================================================
DesignerPanel::DesignerPanel(VTSAudioProcessor& p,
                             juce::ApplicationProperties& properties)
    : processor(p)
    , panelLabel("", "FILTER DESIGN")
    , typeLabel("", "FILTER TYPE")
    , shapeLabel("", "FILTER SHAPE")
    , applyButton("APPLY")
    , autoButton(std::make_shared<juce::ToggleButton>())
{
    addAndMakeVisible(panelLabel);
    addAndMakeVisible(typeLabel);
    addAndMakeVisible(shapeLabel);
    addAndMakeVisible(typeCBox);
    addAndMakeVisible(shapeCBox);
    addAndMakeVisible(orderSlider);
    addAndMakeVisible(cutoffSlider);
    addAndMakeVisible(*autoButton.get());
    addAndMakeVisible(applyButton);

    panelLabel.setJustificationType(juce::Justification::centred);
    typeLabel.setJustificationType(juce::Justification::centred);
    shapeLabel.setJustificationType(juce::Justification::centred);
    for (auto i = 0; i < FilterParameters::FilterType::N_FILTER_TYPES; ++i)
        typeCBox.addItem(FilterParameters::typeToString(
                             static_cast<FilterParameters::FilterType>(i)),
                         i + 1);
    for (auto i = 0; i < FilterParameters::FilterShape::N_FILTER_SHAPES; ++i)
        shapeCBox.addItem(FilterParameters::shapeToString(
                              static_cast<FilterParameters::FilterShape>(i)),
                          i + 1);
    orderSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    Button_setOnOffLabel(*autoButton.get(), "MAN", "AUTO");

    typeCBox.setSelectedId(1 + FilterParameters::FilterType::Butterworth);
    shapeCBox.setSelectedId(1 + FilterParameters::FilterShape::LowPass);

    autoButtonAttachment.reset(new ApplicationPropertiesButtonAttachment(
        properties, "autoFilterDesign", autoButton));

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
DesignerPanel::~DesignerPanel() {}

// =============================================================================
void DesignerPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto b = claf->getPanelInnerRect(getLocalBounds());
        auto h = b.getHeight() / 18;

        panelLabel.setBounds(b.removeFromTop(h));
        b.removeFromTop(h);
        typeLabel.setBounds(b.removeFromTop(h));
        typeCBox.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        shapeLabel.setBounds(b.removeFromTop(h));
        shapeCBox.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        orderSlider.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        cutoffSlider.setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        auto last_row = b.removeFromTop(h);
        autoButton->setBounds(
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
