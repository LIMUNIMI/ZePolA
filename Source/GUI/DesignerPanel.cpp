#include "DesignerPanel.h"
#include "../Macros.h"
#include "LookAndFeel.h"

// =============================================================================
DesignerPanel::CBoxListener::CBoxListener(std::function<void(int)> foo)
    : callback(foo)
{
}
void DesignerPanel::CBoxListener::comboBoxChanged(juce::ComboBox* cbox)
{
    callback(cbox->getSelectedId());
}

// =============================================================================
DesignerPanel::SliderListener::SliderListener(std::function<void(double)> foo)
    : callback(foo)
{
}
void DesignerPanel::SliderListener::sliderValueChanged(juce::Slider* slider)
{
    callback(slider->getValue());
}

// =============================================================================
DesignerPanel::DesignerPanel(PolesAndZerosEQAudioProcessor& p,
                             juce::ApplicationProperties& properties)
    : processor(p)
    , panelLabel("", "FILTER DESIGN")
    , typeLabel("", "FILTER TYPE")
    , shapeLabel("", "FILTER SHAPE")
    , orderLabel("", "FILTER ORDER")
    , cutoffLabel("", "CUTOFF FREQUENCY")
    , applyButton("UPDATE")
    , autoButton(std::make_shared<juce::ToggleButton>())
    , typeCBox(std::make_shared<juce::ComboBox>())
    , shapeCBox(std::make_shared<juce::ComboBox>())
    , orderSlider(std::make_shared<juce::Slider>())
    , cutoffSlider(std::make_shared<juce::Slider>())
    , filterParams(p.getSampleRate())
    , typeCBoxListener(std::bind(&DesignerPanel::setTypeFromCBoxId, this,
                                 std::placeholders::_1))
    , shapeCBoxListener(std::bind(&DesignerPanel::setShapeFromCBoxId, this,
                                  std::placeholders::_1))
    , orderSliderListener(
          std::bind(&DesignerPanel::setOrder, this, std::placeholders::_1))
    , cutoffSliderListener(
          std::bind(&DesignerPanel::setCutoff, this, std::placeholders::_1))
{
    addAndMakeVisible(panelLabel);
    addAndMakeVisible(typeLabel);
    addAndMakeVisible(shapeLabel);
    addAndMakeVisible(orderLabel);
    addAndMakeVisible(cutoffLabel);
    addAndMakeVisible(*typeCBox.get());
    addAndMakeVisible(*shapeCBox.get());
    addAndMakeVisible(*orderSlider.get());
    addAndMakeVisible(*cutoffSlider.get());
    addAndMakeVisible(*autoButton.get());
    addAndMakeVisible(applyButton);

    panelLabel.setJustificationType(juce::Justification::centred);
    typeLabel.setJustificationType(juce::Justification::centred);
    shapeLabel.setJustificationType(juce::Justification::centred);
    orderLabel.setJustificationType(juce::Justification::centred);
    cutoffLabel.setJustificationType(juce::Justification::centred);

    for (auto i = 0; i < FilterParameters::FilterType::N_FILTER_TYPES; ++i)
        typeCBox->addItem(FilterParameters::typeToString(
                              static_cast<FilterParameters::FilterType>(i)),
                          i + 1);
    for (auto i = 0; i < FilterParameters::FilterShape::N_FILTER_SHAPES; ++i)
        shapeCBox->addItem(FilterParameters::shapeToString(
                               static_cast<FilterParameters::FilterShape>(i)),
                           i + 1);
    orderSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    orderSlider->setNormalisableRange(
        {2.0, static_cast<double>(processor.getNElements()), 2.0});
    cutoffSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider->setNormalisableRange(
        {0.0, processor.getSampleRate() * 0.5, 0.1, 0.2});
    Button_setOnOffLabel(*autoButton.get(), "MAN", "AUTO");

    typeCBox->addListener(&typeCBoxListener);
    shapeCBox->addListener(&shapeCBoxListener);
    orderSlider->addListener(&orderSliderListener);
    cutoffSlider->addListener(&cutoffSliderListener);

    autoButtonAttachment.reset(new ApplicationPropertiesButtonAttachment(
        properties, "autoFilterDesign", autoButton));
    typeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "typeFilterDesign", typeCBox));
    shapeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "shapeFilterDesign", shapeCBox));
    orderSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "orderFilterDesign", orderSlider));
    cutoffSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "cutoffFilterDesign", cutoffSlider));

    if (!typeCBox->getSelectedId())
        typeCBox->setSelectedId(1 + FilterParameters::FilterType::Butterworth);
    if (!shapeCBox->getSelectedId())
        shapeCBox->setSelectedId(1 + FilterParameters::FilterShape::LowPass);

    applyButton.onClick = std::bind(&DesignerPanel::designFilter, this);
}
DesignerPanel::~DesignerPanel()
{
    typeCBox->removeListener(&typeCBoxListener);
    shapeCBox->removeListener(&shapeCBoxListener);
    orderSlider->removeListener(&orderSliderListener);
    cutoffSlider->removeListener(&cutoffSliderListener);
}

// =============================================================================
void DesignerPanel::setTypeFromCBoxId(int i)
{
    filterParams.type = static_cast<FilterParameters::FilterType>(i - 1);
    DBG(FilterParameters::typeToString(filterParams.type));
}
void DesignerPanel::setShapeFromCBoxId(int i)
{
    filterParams.shape = static_cast<FilterParameters::FilterShape>(i - 1);
    DBG(FilterParameters::shapeToString(filterParams.shape));
}
void DesignerPanel::setOrder(double f)
{
    filterParams.order = juce::roundToInt(f);
    DBG("ORDER: " << filterParams.order);
}
void DesignerPanel::setCutoff(double f)
{
    filterParams.cutoff = f;
    DBG("CUTOFF: " << filterParams.cutoff);
}

// =============================================================================
void DesignerPanel::designFilter()
{
    filterParams.cutoff = cutoffSlider->getValue();
    filterParams.order  = juce::roundToInt(orderSlider->getValue());
    filterParams.computeZPK();

    auto degree = filterParams.zpk.degree();
    auto k_db   = juce::Decibels::gainToDecibels(filterParams.zpk.gain, -1000.0)
                / filterParams.zpk.nElements();
    int e = 0;
    DBG("------------------------------------------------------------------");
    DBG("Filter Design");
    for (auto i = 0; i < degree; ++i)
    {
        if (i < filterParams.zpk.zeros.size())
            applyFilterElement(e++, filterParams.zpk.zeros[i],
                               FilterElement::Type::ZERO, k_db);
        if (i < filterParams.zpk.poles.size())
            applyFilterElement(e++, filterParams.zpk.poles[i],
                               FilterElement::Type::POLE, k_db);
    }
    auto n = processor.getNElements();
    for (; e < n; ++e)
        processor.setParameterValue(ACTIVE_ID_PREFIX + juce::String(e), false);
    DBG("------------------------------------------------------------------");
}
void DesignerPanel::applyFilterElement(int i, std::complex<double> z,
                                       FilterElement::Type t, double gain)
{
    juce::String i_str(i);
    double m = abs(z), a = std::arg(z) / juce::MathConstants<double>::pi;
    processor.setParameterValue(TYPE_ID_PREFIX + i_str,
                                FilterElement::typeToFloat(t));
    processor.setParameterValue(GAIN_ID_PREFIX + i_str,
                                static_cast<float>(gain));
    processor.setParameterValue(MAGNITUDE_ID_PREFIX + i_str,
                                static_cast<float>(m));
    processor.setParameterValue(PHASE_ID_PREFIX + i_str, static_cast<float>(a));
    processor.setParameterValue(ACTIVE_ID_PREFIX + i_str, true);
    ONLY_ON_DEBUG({
        juce::String prefix("?");
        switch (t)
        {
        case FilterElement::ZERO: prefix = "Z"; break;
        case FilterElement::POLE: prefix = "P"; break;
        }
        DBG("  " << prefix << "(" << m << "; " << a << ") " << gain << "dB");
    })
}
void DesignerPanel::sampleRateChangedCallback(double sr)
{
    auto nr = cutoffSlider->getNormalisableRange();
    cutoffSlider->setNormalisableRange(
        {nr.start, sr * 0.5, nr.interval, nr.skew});
    filterParams.sr = sr;
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
        typeLabel.setBounds(b.removeFromTop(h));
        typeCBox->setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        shapeLabel.setBounds(b.removeFromTop(h));
        shapeCBox->setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        orderLabel.setBounds(b.removeFromTop(h));
        orderSlider->setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        cutoffLabel.setBounds(b.removeFromTop(h));
        cutoffSlider->setBounds(b.removeFromTop(h));
        b.removeFromTop(h / 3);
        auto last_row = b.removeFromTop(h);
        autoButton->setBounds(
            last_row.withTrimmedRight(last_row.getWidth() * 53 / 100));
        applyButton.setBounds(
            last_row.withTrimmedLeft(last_row.getWidth() * 53 / 100));

        orderSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                     orderSlider->getTextBoxWidth(),
                                     orderSlider->getTextBoxHeight());
        cutoffSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                      cutoffSlider->getTextBoxWidth(),
                                      cutoffSlider->getTextBoxHeight());
    }
}
