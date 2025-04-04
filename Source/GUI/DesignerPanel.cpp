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
DesignerPanel::ButtonListener::ButtonListener(std::function<void(bool)> foo)
    : callback(foo)
{
}
void DesignerPanel::ButtonListener::buttonClicked(juce::Button* b)
{
    callback(b->getToggleState());
}
void DesignerPanel::ButtonListener::buttonStateChanged(juce::Button*) {}

// =============================================================================
DesignerPanel::DesignerPanel(PolesAndZerosEQAudioProcessor& p,
                             juce::ApplicationProperties& properties)
    : processor(p)
    , panelLabel("", "FILTER DESIGN")
    , orderLabel("", "FILTER ORDER")
    , cutoffLabel("", "CUTOFF FREQUENCY")
    , rpLabel("", "PASSBAND RIPPLE")
    , rsLabel("", "STOPBAND RIPPLE")
    , applyButton("UPDATE")
    , autoButton(std::make_shared<juce::ToggleButton>())
    , typeCBox(std::make_shared<juce::ComboBox>())
    , shapeCBox(std::make_shared<juce::ComboBox>())
    , orderSlider(std::make_shared<juce::Slider>())
    , cutoffSlider(std::make_shared<juce::Slider>())
    , rpSlider(std::make_shared<juce::Slider>())
    , rsSlider(std::make_shared<juce::Slider>())
    , filterParams(p.getSampleRate())
    , typeCBoxListener(std::bind(&DesignerPanel::setTypeFromCBoxId, this,
                                 std::placeholders::_1))
    , shapeCBoxListener(std::bind(&DesignerPanel::setShapeFromCBoxId, this,
                                  std::placeholders::_1))
    , orderSliderListener(
          std::bind(&DesignerPanel::setOrder, this, std::placeholders::_1))
    , cutoffSliderListener(
          std::bind(&DesignerPanel::setCutoff, this, std::placeholders::_1))
    , rpSliderListener(std::bind(&DesignerPanel::setPassbandRipple, this,
                                 std::placeholders::_1))
    , rsSliderListener(std::bind(&DesignerPanel::setStopbandRipple, this,
                                 std::placeholders::_1))
    , autoButtonListener(
          std::bind(&DesignerPanel::setAuto, this, std::placeholders::_1))
    , autoUpdate(false)
    , applicationProperties(properties)
{
    addAndMakeVisible(panelLabel);
    addAndMakeVisible(orderLabel);
    addAndMakeVisible(cutoffLabel);
    addAndMakeVisible(rpLabel);
    addAndMakeVisible(rsLabel);
    addAndMakeVisible(*typeCBox.get());
    addAndMakeVisible(*shapeCBox.get());
    addAndMakeVisible(*orderSlider.get());
    addAndMakeVisible(*cutoffSlider.get());
    addAndMakeVisible(*rpSlider.get());
    addAndMakeVisible(*rsSlider.get());
    addAndMakeVisible(*autoButton.get());
    addAndMakeVisible(applyButton);

    panelLabel.setJustificationType(juce::Justification::centredTop);
    orderLabel.setJustificationType(juce::Justification::centred);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    rpLabel.setJustificationType(juce::Justification::centred);
    rsLabel.setJustificationType(juce::Justification::centred);

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
        {0.0, processor.getSampleRate() * 0.5, 0.1, 0.25});
    rpSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    rpSlider->setNormalisableRange({0.1, 5.0, 0.001});
    rsSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    rsSlider->setNormalisableRange({6.0, 60.0, 0.001});

    Button_setOnOffLabel(*autoButton.get(), "MAN", "AUTO");

    typeCBox->setSelectedId(1 + filterParams.type);
    shapeCBox->setSelectedId(1 + filterParams.shape);
    orderSlider->setValue(static_cast<double>(filterParams.order));
    cutoffSlider->setValue(filterParams.cutoff);
    rpSlider->setValue(filterParams.passbandRippleDb);
    rsSlider->setValue(filterParams.stopbandRippleDb);

    typeCBox->addListener(&typeCBoxListener);
    shapeCBox->addListener(&shapeCBoxListener);
    orderSlider->addListener(&orderSliderListener);
    cutoffSlider->addListener(&cutoffSliderListener);
    rpSlider->addListener(&rpSliderListener);
    rsSlider->addListener(&rsSliderListener);
    autoButton->addListener(&autoButtonListener);

    typeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "typeFilterDesign", typeCBox));
    shapeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "shapeFilterDesign", shapeCBox));
    orderSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "orderFilterDesign", orderSlider));
    cutoffSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "cutoffFilterDesign", cutoffSlider));
    rpSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "rpFilterDesign", rpSlider));
    rsSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "rsFilterDesign", rsSlider));

    applyButton.onClick = std::bind(&DesignerPanel::designFilter, this);
    autoButtonAttachment.reset(new ApplicationPropertiesButtonAttachment(
        properties, "autoFilterDesign", autoButton));

    updatePassbandRippleVisibility();
    updateStopbandRippleVisibility();
}
DesignerPanel::~DesignerPanel()
{
    typeCBox->removeListener(&typeCBoxListener);
    shapeCBox->removeListener(&shapeCBoxListener);
    orderSlider->removeListener(&orderSliderListener);
    cutoffSlider->removeListener(&cutoffSliderListener);
    rpSlider->removeListener(&rpSliderListener);
    rsSlider->removeListener(&rsSliderListener);
    autoButton->removeListener(&autoButtonListener);
}

// =============================================================================
void DesignerPanel::setTypeFromCBoxId(int i)
{
    filterParams.type = static_cast<FilterParameters::FilterType>(i - 1);
    DBG("TYPE: " << FilterParameters::typeToString(filterParams.type));
    autoDesignFilter();
    updatePassbandRippleVisibility();
    updateStopbandRippleVisibility();
}
void DesignerPanel::setShapeFromCBoxId(int i)
{
    filterParams.shape = static_cast<FilterParameters::FilterShape>(i - 1);
    DBG("SHAPE: " << FilterParameters::shapeToString(filterParams.shape));
    autoDesignFilter();
}
void DesignerPanel::setOrder(double f)
{
    filterParams.order = juce::roundToInt(f);
    DBG("ORDER: " << filterParams.order);
    autoDesignFilter();
}
void DesignerPanel::setCutoff(double f)
{
    filterParams.cutoff = f;
    DBG("CUTOFF: " << filterParams.cutoff);
    autoDesignFilter();
}
void DesignerPanel::setPassbandRipple(double rp)
{
    filterParams.passbandRippleDb = rp;
    DBG("PASSBAND RIPPLE: " << filterParams.passbandRippleDb);
    autoDesignFilter();
}
void DesignerPanel::setStopbandRipple(double rs)
{
    filterParams.stopbandRippleDb = rs;
    DBG("STOPBAND RIPPLE: " << filterParams.stopbandRippleDb);
    autoDesignFilter();
}
void DesignerPanel::setAuto(bool b)
{
    autoUpdate = b;
    DBG(((autoUpdate) ? "AUTO" : "MANUAL"));
    autoDesignFilter();
}

// =============================================================================
void DesignerPanel::updatePassbandRippleVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::ChebyshevI:
    case FilterParameters::FilterType::Elliptic: shouldBeVisible = true;
    }
    DBG("PassbandRipple slider should" << ((shouldBeVisible) ? "" : "n't")
                                       << " be visible");
    if (shouldBeVisible != rpSlider->isVisible()
        || shouldBeVisible != rpLabel.isVisible())
    {
        DBG(" Setting PassbandRipple slider visibility");
        rpLabel.setVisible(shouldBeVisible);
        rpSlider->setVisible(shouldBeVisible);
        resized();
    }
}
void DesignerPanel::updateStopbandRippleVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::ChebyshevII:
    case FilterParameters::FilterType::Elliptic: shouldBeVisible = true;
    }
    DBG("StopbandRipple slider should" << ((shouldBeVisible) ? "" : "n't")
                                       << " be visible");
    if (shouldBeVisible != rsSlider->isVisible()
        || shouldBeVisible != rsLabel.isVisible())
    {
        DBG(" Setting StopbandRipple slider visibility");
        rsLabel.setVisible(shouldBeVisible);
        rsSlider->setVisible(shouldBeVisible);
        resized();
    }
}

// =============================================================================
void DesignerPanel::autoDesignFilter()
{
    if (autoUpdate) designFilter();
}
void DesignerPanel::designFilter()
{
    filterParams.computeZPK();

    auto degree = filterParams.zpk.degree();
    auto k_db   = juce::Decibels::gainToDecibels(filterParams.zpk.gain, -600.0);
    k_db /= filterParams.zpk.nElements();
    int e = 0;
    ONLY_ON_DEBUG(if (!autoUpdate) {
        DBG("---------------------------------"
            "---------------------------------");
        DBG("Filter Design");
    })
    for (auto i = 0; i < degree; ++i)
    {
        if (i < filterParams.zpk.zeros.size())
            applyFilterElement(e++, filterParams.zpk.zeros[i], false, k_db);
        if (i < filterParams.zpk.poles.size())
            applyFilterElement(e++, filterParams.zpk.poles[i], true, k_db);
    }
    auto n = processor.getNElements();
    for (auto i = e; i < n; ++i)
        processor.setParameterValue(ACTIVE_ID_PREFIX + juce::String(i), false);
    {
        // AUTO GAIN
        juce::PropertiesFile* pf
            = applicationProperties.getCommonSettings(true);
        if (pf && pf->getBoolValue(AUTO_GAIN_PROPERTY_ID, false))
            k_db -= processor.getCascadePeakGain()
                    / filterParams.zpk.nElements();
        ONLY_ON_DEBUG(
            if (!autoUpdate) { DBG("FILTER DESIGNER AUTO GAIN: " << k_db); })
        for (auto i = 0; i < e; ++i)
            processor.setParameterValue(GAIN_ID_PREFIX + juce::String(i),
                                        static_cast<float>(k_db));
    }
    ONLY_ON_DEBUG(if (!autoUpdate) DBG("---------------------------------"
                                       "---------------------------------");)
}
void DesignerPanel::applyFilterElement(int i, std::complex<double> z, bool t,
                                       double gain)
{
    juce::String i_str(i);
    double m = abs(z), a = std::arg(z) / juce::MathConstants<double>::pi;
    processor.setParameterValue(TYPE_ID_PREFIX + i_str, static_cast<float>(t));
    processor.setParameterValue(MAGNITUDE_ID_PREFIX + i_str,
                                static_cast<float>(m));
    processor.setParameterValue(PHASE_ID_PREFIX + i_str, static_cast<float>(a));
    processor.setParameterValue(GAIN_ID_PREFIX + i_str,
                                static_cast<float>(gain));
    processor.setParameterValue(INVERTED_ID_PREFIX + i_str, false);
    processor.setParameterValue(SINGLE_ID_PREFIX + i_str, false);
    processor.setParameterValue(ACTIVE_ID_PREFIX + i_str, true);
    ONLY_ON_DEBUG(if (!autoUpdate) {
        DBG("  " << ((t) ? "P" : "Z") << "(" << m << "; " << a << ") " << gain
                 << "dB");
    })
}
void DesignerPanel::sampleRateChangedCallback(double sr)
{
    auto nr = cutoffSlider->getNormalisableRange();
    cutoffSlider->setNormalisableRange(
        {nr.start, sr * 0.5, nr.interval, nr.skew});
    filterParams.sr = sr;
    autoDesignFilter();
}

// =============================================================================
void DesignerPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        int ph, sh;
        auto regions = claf->configureDesignerPanel(getLocalBounds(), &ph, &sh);
        jassert(regions.size() == 3);

        autoButton->setBounds(regions[1]);
        applyButton.setBounds(regions[2]);

        // Header label
        panelLabel.setBounds(regions[0].removeFromTop(ph));

        // Type combo box
        regions[0].removeFromTop(sh);
        typeCBox->setBounds(regions[0].removeFromTop(ph));

        // Shape combobox
        regions[0].removeFromTop(sh);
        shapeCBox->setBounds(regions[0].removeFromTop(ph));

        // Order slider
        regions[0].removeFromTop(sh);
        orderLabel.setBounds(regions[0].removeFromTop(ph));
        orderSlider->setBounds(regions[0].removeFromTop(ph));
        orderSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                     orderSlider->getTextBoxWidth(),
                                     orderSlider->getTextBoxHeight());

        // Cutoff frequency slider
        regions[0].removeFromTop(sh);
        cutoffLabel.setBounds(regions[0].removeFromTop(ph));
        cutoffSlider->setBounds(regions[0].removeFromTop(ph));
        cutoffSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                      cutoffSlider->getTextBoxWidth(),
                                      cutoffSlider->getTextBoxHeight());

        // Passband ripple slider
        if (rpLabel.isVisible() || rpSlider->isVisible())
        {
            regions[0].removeFromTop(sh);
            rpLabel.setBounds(regions[0].removeFromTop(ph));
            rpSlider->setBounds(regions[0].removeFromTop(ph));
            rpSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                      rpSlider->getTextBoxWidth(),
                                      rpSlider->getTextBoxHeight());
        }
        // Stopband ripple slider
        if (rsLabel.isVisible() || rsSlider->isVisible())
        {
            regions[0].removeFromTop(sh);
            rsLabel.setBounds(regions[0].removeFromTop(ph));
            rsSlider->setBounds(regions[0].removeFromTop(ph));
            rsSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                      rsSlider->getTextBoxWidth(),
                                      rsSlider->getTextBoxHeight());
        }
    }
}
