/*
  ==============================================================================

    DesignerPanel.cpp

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
DesignerPanel::DesignerPanel(ZePolAudioProcessor& p,
                             juce::ApplicationProperties& properties)
    : typeCBoxListener(std::bind(&DesignerPanel::setTypeFromCBoxId, this,
                                 std::placeholders::_1))
    , analogShapeCBoxListener(
          std::bind(&DesignerPanel::setAnalogShapeFromCBoxId, this,
                    std::placeholders::_1))
    , biquadShapeCBoxListener(
          std::bind(&DesignerPanel::setBiquadShapeFromCBoxId, this,
                    std::placeholders::_1))
    , orderSliderListener(
          std::bind(&DesignerPanel::setOrder, this, std::placeholders::_1))
    , cutoffSliderListener(
          std::bind(&DesignerPanel::setCutoff, this, std::placeholders::_1))
    , cutoff2SliderListener(
          std::bind(&DesignerPanel::setCutoff2, this, std::placeholders::_1))
    , rpSliderListener(std::bind(&DesignerPanel::setPassbandRipple, this,
                                 std::placeholders::_1))
    , rsSliderListener(std::bind(&DesignerPanel::setStopbandRipple, this,
                                 std::placeholders::_1))
    , qualitySliderListener(
          std::bind(&DesignerPanel::setQuality, this, std::placeholders::_1))
    , gainDBSliderListener(
          std::bind(&DesignerPanel::setGainDB, this, std::placeholders::_1))
    , autoButtonListener(
          std::bind(&DesignerPanel::setAuto, this, std::placeholders::_1))
    , processor(p)
    , panelLabel("", "FILTER DESIGN")
    , orderLabel("", "FILTER ORDER")
    , cutoffLabel("", "CUTOFF FREQUENCY")
    , rpLabel("", "PASSBAND RIPPLE")
    , rsLabel("", "STOPBAND RIPPLE")
    , qualityLabel("", "QUALITY")
    , gainDBLabel("", "GAIN")
    , typeCBox(std::make_shared<juce::ComboBox>())
    , analogShapeCBox(std::make_shared<juce::ComboBox>())
    , biquadShapeCBox(std::make_shared<juce::ComboBox>())
    , orderSlider(std::make_shared<juce::Slider>())
    , cutoffSlider(std::make_shared<juce::Slider>())
    , cutoff2Slider(std::make_shared<juce::Slider>())
    , rpSlider(std::make_shared<juce::Slider>())
    , rsSlider(std::make_shared<juce::Slider>())
    , qualitySlider(std::make_shared<juce::Slider>())
    , gainDBSlider(std::make_shared<juce::Slider>())
    , autoButton(std::make_shared<juce::ToggleButton>())
    , applyButton("UPDATE")
    , filterParams(p.getSampleRate())
    , autoUpdate(false)
    , crossUpdateShape(false)
    , applicationProperties(properties)
{
    addAndMakeVisible(panelLabel);
    addAndMakeVisible(orderLabel);
    addAndMakeVisible(cutoffLabel);
    addAndMakeVisible(rpLabel);
    addAndMakeVisible(rsLabel);
    addAndMakeVisible(qualityLabel);
    addAndMakeVisible(gainDBLabel);
    addAndMakeVisible(*typeCBox.get());
    addAndMakeVisible(*analogShapeCBox.get());
    addAndMakeVisible(*biquadShapeCBox.get());
    addAndMakeVisible(*orderSlider.get());
    addAndMakeVisible(*cutoffSlider.get());
    addAndMakeVisible(*cutoff2Slider.get());
    addAndMakeVisible(*rpSlider.get());
    addAndMakeVisible(*rsSlider.get());
    addAndMakeVisible(*qualitySlider.get());
    addAndMakeVisible(*gainDBSlider.get());
    addAndMakeVisible(*autoButton.get());
    addAndMakeVisible(applyButton);

    panelLabel.setJustificationType(juce::Justification::centredTop);
    orderLabel.setJustificationType(juce::Justification::centred);
    cutoffLabel.setJustificationType(juce::Justification::centred);
    rpLabel.setJustificationType(juce::Justification::centred);
    rsLabel.setJustificationType(juce::Justification::centred);
    qualityLabel.setJustificationType(juce::Justification::centred);
    gainDBLabel.setJustificationType(juce::Justification::centred);

    for (auto i = 0; i < FilterParameters::FilterType::N_FILTER_TYPES; ++i)
        typeCBox->addItem(FilterParameters::typeToString(
                              static_cast<FilterParameters::FilterType>(i)),
                          i + 1);
    for (auto i = 0;
         i < FilterParameters::AnalogFilterShape::N_ANALOG_FILTER_SHAPES; ++i)
        analogShapeCBox->addItem(
            FilterParameters::shapeToString(
                static_cast<FilterParameters::AnalogFilterShape>(i)),
            i + 1);
    for (auto i = 0;
         i < FilterParameters::BiquadFilterShape::N_BIQUAD_FILTER_SHAPES; ++i)
        biquadShapeCBox->addItem(
            FilterParameters::shapeToString(
                static_cast<FilterParameters::BiquadFilterShape>(i)),
            i + 1);

    orderSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    orderSlider->setNormalisableRange(
        {2.0, static_cast<double>(processor.getNElements()), 2.0});
    cutoffSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    cutoff2Slider->setSliderStyle(juce::Slider::LinearHorizontal);
    sampleRateChangedCallback(p.getSampleRate());
    rpSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    rpSlider->setNormalisableRange({1e-6, 12, 0.001});
    rsSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    rsSlider->setNormalisableRange({0.1, 60.0, 0.001});
    qualitySlider->setSliderStyle(juce::Slider::LinearHorizontal);
    {
        juce::NormalisableRange<double> qnr(0.2, 12.0, 0.001);
        qnr.setSkewForCentre(3.0);
        qualitySlider->setNormalisableRange(qnr);
    }
    gainDBSlider->setSliderStyle(juce::Slider::LinearHorizontal);
    gainDBSlider->setNormalisableRange({-60.0, 60.0, 0.001});

    Button_setOnOffLabel(*autoButton.get(), "MAN", "AUTO");

    setInternalValuesToComponents(juce::NotificationType::sendNotificationSync);

    typeCBox->addListener(&typeCBoxListener);
    analogShapeCBox->addListener(&analogShapeCBoxListener);
    biquadShapeCBox->addListener(&biquadShapeCBoxListener);
    orderSlider->addListener(&orderSliderListener);
    cutoffSlider->addListener(&cutoffSliderListener);
    cutoff2Slider->addListener(&cutoff2SliderListener);
    rpSlider->addListener(&rpSliderListener);
    rsSlider->addListener(&rsSliderListener);
    qualitySlider->addListener(&qualitySliderListener);
    gainDBSlider->addListener(&gainDBSliderListener);
    autoButton->addListener(&autoButtonListener);

    typeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "typeFilterDesign", typeCBox));
    biquadShapeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "biquadShapeFilterDesign", biquadShapeCBox));
    analogShapeCBoxAttachment.reset(new ApplicationPropertiesComboBoxAttachment(
        properties, "analogShapeFilterDesign", analogShapeCBox));
    orderSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "orderFilterDesign", orderSlider));
    cutoffSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "cutoffFilterDesign", cutoffSlider));
    cutoff2SliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "cutoff2FilterDesign", cutoff2Slider));
    rpSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "rpFilterDesign", rpSlider));
    rsSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "rsFilterDesign", rsSlider));
    qualitySliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "qualityFilterDesign", qualitySlider));
    gainDBSliderAttachment.reset(new ApplicationPropertiesSliderAttachment(
        properties, "gainDBFilterDesign", gainDBSlider));

    updateBiquadFilterShapeVisibility();
    updateAnalogFilterShapeVisibility();
    setCrossUpdateShape(true);

    applyButton.onClick = std::bind(&DesignerPanel::designFilter, this);

    // Do not attach auto-button to property file: it causes inconsistencies
    // dependent on order of operations
    // autoButtonAttachment.reset(new ApplicationPropertiesButtonAttachment(
    //     properties, AUTO_FILTER_PROPERTY_ID, autoButton));
}
DesignerPanel::~DesignerPanel()
{
    typeCBox->removeListener(&typeCBoxListener);
    analogShapeCBox->removeListener(&analogShapeCBoxListener);
    biquadShapeCBox->removeListener(&biquadShapeCBoxListener);
    orderSlider->removeListener(&orderSliderListener);
    cutoffSlider->removeListener(&cutoffSliderListener);
    cutoff2Slider->removeListener(&cutoff2SliderListener);
    rpSlider->removeListener(&rpSliderListener);
    rsSlider->removeListener(&rsSliderListener);
    qualitySlider->removeListener(&qualitySliderListener);
    gainDBSlider->removeListener(&gainDBSliderListener);
    autoButton->removeListener(&autoButtonListener);
}

// =============================================================================
void DesignerPanel::setInternalValuesToComponents(juce::NotificationType nt)
{
    typeCBox->setSelectedId(1 + filterParams.type, nt);
    analogShapeCBox->setSelectedId(1 + filterParams.analogFShape, nt);
    biquadShapeCBox->setSelectedId(1 + filterParams.biquadFShape, nt);
    orderSlider->setValue(static_cast<double>(filterParams.order), nt);
    cutoffSlider->setValue(filterParams.cutoff, nt);
    cutoff2Slider->setValue(filterParams.cutoff2, nt);
    rpSlider->setValue(filterParams.passbandRippleDb, nt);
    rsSlider->setValue(filterParams.stopbandRippleDb, nt);
    qualitySlider->setValue(filterParams.quality, nt);
    gainDBSlider->setValue(filterParams.gain_db, nt);
    autoButton->setToggleState(autoUpdate, nt);
}
template <typename MemberType>
bool DesignerPanel::_checkChangedAndSetValue(MemberType* p, const MemberType v)
{
    bool b = *p != v;
    if (b) *p = v;
    return b;
}

// =============================================================================
static double _RIPPLE_DELTA = 0.25;
void DesignerPanel::setSafeValueForStopbandRipple(double unsafeStopbandRippleDb)
{
    double safeStopbandRippleDb = unsafeStopbandRippleDb;
    double lowerBound           = filterParams.passbandRippleDb + _RIPPLE_DELTA;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::Elliptic:
        if (unsafeStopbandRippleDb < lowerBound)
        {
            rsSlider->setValue(safeStopbandRippleDb = lowerBound);
            DBG("  Stopband ripple lower bound: " << lowerBound);
        }
        break;
    default: break;  // Nothing to do
    }

    if (!_checkChangedAndSetValue(&filterParams.stopbandRippleDb,
                                  safeStopbandRippleDb))
        return;

    DBG("  Stopband ripple:             " << filterParams.stopbandRippleDb);
    autoDesignFilter();
}

void DesignerPanel::setTypeFromCBoxId(int i)
{
    if (!_checkChangedAndSetValue(
            &filterParams.type,
            static_cast<FilterParameters::FilterType>(i - 1)))
        return;

    DBG("TYPE: " << FilterParameters::typeToString(filterParams.type));
    setSafeValueForStopbandRipple(filterParams.stopbandRippleDb);
    updateBiquadFilterShapeVisibility();
    updateAnalogFilterShapeVisibility();

    autoDesignFilter();
}
void DesignerPanel::setAnalogShapeFromCBoxId(int i)
{
    if (!_checkChangedAndSetValue(
            &filterParams.analogFShape,
            static_cast<FilterParameters::AnalogFilterShape>(i - 1)))
        return;

    DBG("ANALOG_SHAPE: " << FilterParameters::shapeToString(
            filterParams.analogFShape));

    // Cross-update to biquad filter shapes if compatible
    if (crossUpdateShape)
    {
        switch (filterParams.type)
        {
        case FilterParameters::FilterType::Biquad: break;
        default:
        {
            int j;
            switch (filterParams.analogFShape)
            {
            case FilterParameters::AnalogFilterShape::AnalogLowPass:
                j = FilterParameters::BiquadFilterShape::BiquadLowPass + 1;
                break;
            case FilterParameters::AnalogFilterShape::AnalogHighPass:
                j = FilterParameters::BiquadFilterShape::BiquadHighPass + 1;
                break;
            default: j = 0; break;  // Incompatible filter shape
            }
            if (j) biquadShapeCBox->setSelectedId(j);
        }
        break;
        }
    }

    autoDesignFilter();
}
void DesignerPanel::setBiquadShapeFromCBoxId(int i)
{
    if (!_checkChangedAndSetValue(
            &filterParams.biquadFShape,
            static_cast<FilterParameters::BiquadFilterShape>(i - 1)))
        return;

    // Cross-update to analog filter shapes if compatible
    if (crossUpdateShape)
    {
        switch (filterParams.type)
        {
        case FilterParameters::FilterType::Biquad:
        {
            int j;
            switch (filterParams.biquadFShape)
            {
            case FilterParameters::BiquadFilterShape::BiquadLowPass:
            case FilterParameters::BiquadFilterShape::BiquadHighShelf1:
            case FilterParameters::BiquadFilterShape::BiquadHighShelf2:
                j = FilterParameters::AnalogFilterShape::AnalogLowPass + 1;
                break;
            case FilterParameters::BiquadFilterShape::BiquadHighPass:
            case FilterParameters::BiquadFilterShape::BiquadLowShelf1:
            case FilterParameters::BiquadFilterShape::BiquadLowShelf2:
                j = FilterParameters::AnalogFilterShape::AnalogHighPass + 1;
                break;
            default: j = 0; break;  // Incompatible filter shape
            }
            if (j) analogShapeCBox->setSelectedId(j);
        }
        break;
        default: break;
        }
    }
    updateQualityVisibility();
    updateGainDBVisibility();

    autoDesignFilter();
}
void DesignerPanel::setOrder(double f)
{
    if (!_checkChangedAndSetValue(&filterParams.order, juce::roundToInt(f)))
        return;

    DBG("ORDER: " << filterParams.order);
    autoDesignFilter();
}
void DesignerPanel::setCutoff(double f)
{
    if (!_checkChangedAndSetValue(&filterParams.cutoff, f)) return;

    DBG("CUTOFF: " << filterParams.cutoff);
    autoDesignFilter();
}
void DesignerPanel::setCutoff2(double f)
{
    if (!_checkChangedAndSetValue(&filterParams.cutoff2, f)) return;

    DBG("CUTOFF_2: " << filterParams.cutoff2);
    autoDesignFilter();
}
void DesignerPanel::setPassbandRipple(double rp)
{
    if (!_checkChangedAndSetValue(&filterParams.passbandRippleDb, rp)) return;

    DBG("PASSBAND RIPPLE: " << filterParams.passbandRippleDb);
    setSafeValueForStopbandRipple(filterParams.stopbandRippleDb);
    autoDesignFilter();
}
void DesignerPanel::setStopbandRipple(double rs)
{
    setSafeValueForStopbandRipple(rs);
}
void DesignerPanel::setQuality(double q)
{
    if (!_checkChangedAndSetValue(&filterParams.quality, q)) return;

    DBG("QUALITY: " << filterParams.quality);
    autoDesignFilter();
}
void DesignerPanel::setGainDB(double db)
{
    if (!_checkChangedAndSetValue(&filterParams.gain_db, db)) return;

    DBG("GAIN dB: " << filterParams.gain_db);
    autoDesignFilter();
}
void DesignerPanel::setAuto(bool b)
{
    if (!_checkChangedAndSetValue(&autoUpdate, b)) return;

    DBG(((autoUpdate) ? "AUTO" : "MANUAL"));
    autoDesignFilter();
}
void DesignerPanel::setCrossUpdateShape(bool b)
{
    if (!_checkChangedAndSetValue(&crossUpdateShape, b)) return;

    DBG("CROSS UPDATE: " << ((crossUpdateShape) ? "on" : "off"));
    autoDesignFilter();
}

// =============================================================================
void DesignerPanel::updateBiquadFilterShapeVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::Biquad: shouldBeVisible = true; break;
    default: break;  // Nothing to do
    }
    DBG("BiquadFilterShape menu should" << ((shouldBeVisible) ? "" : "n't")
                                        << " be visible");
    if (shouldBeVisible)
        setBiquadShapeFromCBoxId(biquadShapeCBox->getSelectedId());
    if (shouldBeVisible != biquadShapeCBox->isVisible())
    {
        DBG(" Setting BiquadFilterShape menu visibility");
        biquadShapeCBox->setVisible(shouldBeVisible);
        resized();
    }
    updateQualityVisibility();
    updateGainDBVisibility();
    updateCutoff2Visibility();
}
void DesignerPanel::updateAnalogFilterShapeVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::Butterworth:
    case FilterParameters::FilterType::ChebyshevI:
    case FilterParameters::FilterType::ChebyshevII:
    case FilterParameters::FilterType::Elliptic: shouldBeVisible = true; break;
    default: break;  // Nothing to do
    }
    DBG("AnalogFilterShape menu should" << ((shouldBeVisible) ? "" : "n't")
                                        << " be visible");
    if (shouldBeVisible)
        setAnalogShapeFromCBoxId(analogShapeCBox->getSelectedId());
    if (shouldBeVisible != analogShapeCBox->isVisible())
    {
        DBG(" Setting AnalogFilterShape menu visibility");
        analogShapeCBox->setVisible(shouldBeVisible);
        resized();
    }
    updatePassbandRippleVisibility();
    updateStopbandRippleVisibility();
    updateFilterOrderVisibility();
}
void DesignerPanel::updateFilterOrderVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::Butterworth:
    case FilterParameters::FilterType::ChebyshevI:
    case FilterParameters::FilterType::ChebyshevII:
    case FilterParameters::FilterType::Elliptic: shouldBeVisible = true; break;
    default: break;  // Nothing to do
    }
    DBG("FilterOrder slider should" << ((shouldBeVisible) ? "" : "n't")
                                    << " be visible");
    if (shouldBeVisible != orderSlider->isVisible()
        || shouldBeVisible != orderLabel.isVisible())
    {
        DBG(" Setting FilterOrder slider visibility");
        orderSlider->setVisible(shouldBeVisible);
        orderLabel.setVisible(shouldBeVisible);
        resized();
    }
}
void DesignerPanel::updateCutoff2Visibility()
{
    bool shouldBeVisible = false;
    DBG("Cutoff2 slider should" << ((shouldBeVisible) ? "" : "n't")
                                << " be visible");
    if (shouldBeVisible != cutoff2Slider->isVisible())
    {
        DBG(" Setting Cutoff2 slider visibility");
        cutoff2Slider->setVisible(shouldBeVisible);
        resized();
    }
}
void DesignerPanel::updatePassbandRippleVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::ChebyshevI:
    case FilterParameters::FilterType::Elliptic: shouldBeVisible = true; break;
    default: break;  // Nothing to do
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
    case FilterParameters::FilterType::Elliptic: shouldBeVisible = true; break;
    default: break;  // Nothing to do
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
void DesignerPanel::updateQualityVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::Biquad:
        switch (filterParams.biquadFShape)
        {
        case FilterParameters::BiquadFilterShape::BiquadLowPass:
        case FilterParameters::BiquadFilterShape::BiquadHighPass:
        case FilterParameters::BiquadFilterShape::BiquadNotch:
        case FilterParameters::BiquadFilterShape::BiquadAllPass:
        case FilterParameters::BiquadFilterShape::BiquadPeaking:
        case FilterParameters::BiquadFilterShape::BiquadLowShelf2:
        case FilterParameters::BiquadFilterShape::BiquadHighShelf2:
        case FilterParameters::BiquadFilterShape::BiquadBandPass1:
        case FilterParameters::BiquadFilterShape::BiquadBandPass2:
            shouldBeVisible = true;
            break;
        default: break;  // Nothing to do
        };
        break;
    default: break;  // Nothing to do
    }
    DBG("Quality slider should" << ((shouldBeVisible) ? "" : "n't")
                                << " be visible");
    if (shouldBeVisible != qualitySlider->isVisible()
        || shouldBeVisible != qualityLabel.isVisible())
    {
        DBG(" Setting Quality slider visibility");
        qualityLabel.setVisible(shouldBeVisible);
        qualitySlider->setVisible(shouldBeVisible);
        resized();
    }
}
void DesignerPanel::updateGainDBVisibility()
{
    bool shouldBeVisible = false;
    switch (filterParams.type)
    {
    case FilterParameters::FilterType::Biquad:
        switch (filterParams.biquadFShape)
        {
        case FilterParameters::BiquadFilterShape::BiquadPeaking:
        case FilterParameters::BiquadFilterShape::BiquadLowShelf1:
        case FilterParameters::BiquadFilterShape::BiquadHighShelf1:
        case FilterParameters::BiquadFilterShape::BiquadLowShelf2:
        case FilterParameters::BiquadFilterShape::BiquadHighShelf2:
            shouldBeVisible = true;
            break;
        default: break;  // Nothing to do
        };
        break;
    default: break;  // Nothing to do
    }
    DBG("GainDB slider should" << ((shouldBeVisible) ? "" : "n't")
                               << " be visible");
    if (shouldBeVisible != gainDBSlider->isVisible()
        || shouldBeVisible != gainDBLabel.isVisible())
    {
        DBG(" Setting GainDB slider visibility");
        gainDBLabel.setVisible(shouldBeVisible);
        gainDBSlider->setVisible(shouldBeVisible);
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

    auto k_db = juce::Decibels::gainToDecibels(filterParams.zpk.gain, -600.0);
    auto db_denom     = filterParams.zpk.degree() * 2;
    auto k_db_portion = k_db / db_denom;
    auto n_p          = filterParams.zpk.poles.size();
    auto n_z          = filterParams.zpk.zeros.size();
    auto n            = processor.getNElements();
    int pd = 0, zd = 0;
    std::vector<int> gain_db_multipliers(n, 1);
    std::vector<bool> designed_elements(n, false);

    // Apply design to filter element
    ONLY_ON_DEBUG(if (!autoUpdate) {
        DBG("---------------------------------"
            "---------------------------------");
        DBG("Filter Design");
    })
    for (int pi = 0, zi = 0, i = 0; i < n && (pi < n_p || zi < n_z);)
    {
        if (processor.getElementLocked(i))
        {
            i++;
            continue;
        }
        ONLY_ON_DEBUG(auto prev_i = i;)
        auto next_pd
            = pd + ((pi < n_p && filterParams.zpk.single_poles[pi]) ? 1 : 2);
        if (zi < n_z && (pi >= n_p || zd <= next_pd))
        {
            designed_elements[i] = true;
            if (!filterParams.zpk.single_zeros[zi]) gain_db_multipliers[i] = 2;
            applyFilterElement(i, filterParams.zpk.zeros[zi], false,
                               k_db_portion * gain_db_multipliers[i],
                               filterParams.zpk.single_zeros[zi]);
            zd += gain_db_multipliers[i];
            zi++;
            i++;
        }
        if (i < n && pi < n_p && (zi >= n_z || zd > next_pd))
        {
            designed_elements[i] = true;
            if (!filterParams.zpk.single_poles[pi]) gain_db_multipliers[i] = 2;
            applyFilterElement(i, filterParams.zpk.poles[pi], true,
                               k_db_portion * gain_db_multipliers[i],
                               filterParams.zpk.single_poles[pi]);
            pd += gain_db_multipliers[i];
            pi++;
            i++;
        }
        jassert(prev_i < i);
    }
    jassert(db_denom >= zd + pd);
    ONLY_ON_DEBUG(if (db_denom > zd + pd) {
        DBG("Filter elements overflow during design. Amount: " << db_denom - zd
                                                                      - pd);
    })

    // Deactivate unaffected non-locked elements
    for (auto i = 0; i < n; ++i)
        if (!(designed_elements[i] || processor.getElementLocked(i)))
            processor.setParameterValue(ACTIVE_ID_PREFIX + juce::String(i),
                                        false);

    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
    {
        // Divide for the actual weight of instantiated elements
        k_db_portion = (k_db - processor.getCascadePeakGain()) / (zd + pd);

        ONLY_ON_DEBUG(if (!autoUpdate) {
            DBG("FILTER DESIGNER AUTO GAIN: " << k_db_portion);
        })

        // Apply weighted gain
        for (auto i = 0; i < n; ++i)
            if (designed_elements[i])
            {
                jassert(!processor.getElementLocked(i));
                processor.setParameterValue(
                    GAIN_ID_PREFIX + juce::String(i),
                    static_cast<float>(k_db_portion * gain_db_multipliers[i]));
            }
    }

    ONLY_ON_DEBUG(if (!autoUpdate) DBG("---------------------------------"
                                       "---------------------------------");)
}
void DesignerPanel::applyFilterElement(int i, std::complex<double> z, bool t,
                                       double gain, bool is_single)
{
    juce::String i_str(i);
    double m = abs(z), a = std::arg(z) / juce::MathConstants<double>::pi;
    bool inv = m > 1.0 + 1e-6;
    if (inv) m = 1.0 / m;
    processor.setParameterValue(TYPE_ID_PREFIX + i_str, static_cast<float>(t));
    processor.setParameterValue(MAGNITUDE_ID_PREFIX + i_str,
                                static_cast<float>(m));
    processor.setParameterValue(PHASE_ID_PREFIX + i_str, static_cast<float>(a));
    processor.setParameterValue(GAIN_ID_PREFIX + i_str,
                                static_cast<float>(gain));
    processor.setParameterValue(INVERTED_ID_PREFIX + i_str, inv);
    processor.setParameterValue(SINGLE_ID_PREFIX + i_str, is_single);
    processor.setParameterValue(ACTIVE_ID_PREFIX + i_str, true);
    ONLY_ON_DEBUG(if (!autoUpdate) {
        DBG("  " << ((t) ? "P" : "Z") << "(" << m << "; " << a << ") " << gain
                 << "dB");
    })
}
void DesignerPanel::sampleRateChangedCallback(double sr)
{
    auto nr  = cutoffSlider->getNormalisableRange();
    nr.start = 0.0;
    nr.end   = sr * 0.5;

    nr.interval = std::clamp(sr * 0.001, 0.0, 0.1);
    nr.setSkewForCentre(std::clamp(sr * 0.25, 0.0, 1000.0));
    cutoffSlider->setNormalisableRange(nr);
    cutoff2Slider->setNormalisableRange(nr);
    filterParams.sr = sr;
    autoDesignFilter();
}

// =============================================================================
void DesignerPanel::appendLabelAndComponentIfVisible(juce::Rectangle<int>& r,
                                                     int sh, int ph,
                                                     juce::Label* lbl,
                                                     juce::Component* cmp)
{
    bool lbl_flag = lbl && lbl->isVisible();
    bool cmp_flag = cmp && cmp->isVisible();
    if (lbl_flag || cmp_flag)
    {
        r.removeFromTop(sh);
        if (lbl_flag) lbl->setBounds(r.removeFromTop(ph));
        if (cmp_flag)
        {
            cmp->setBounds(r.removeFromTop(ph));
            if (auto sli = dynamic_cast<juce::Slider*>(cmp))
                sli->setTextBoxStyle(juce::Slider::TextBoxRight, false,
                                     sli->getTextBoxWidth(),
                                     sli->getTextBoxHeight());
        }
    }
}
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
        appendLabelAndComponentIfVisible(regions[0], sh, ph, nullptr,
                                         typeCBox.get());

        appendLabelAndComponentIfVisible(regions[0], sh, ph, nullptr,
                                         biquadShapeCBox.get());
        appendLabelAndComponentIfVisible(regions[0], sh, ph, nullptr,
                                         analogShapeCBox.get());

        appendLabelAndComponentIfVisible(regions[0], sh, ph, &orderLabel,
                                         orderSlider.get());
        appendLabelAndComponentIfVisible(regions[0], sh, ph, &cutoffLabel,
                                         cutoffSlider.get());
        appendLabelAndComponentIfVisible(
            regions[0], sh, ph,
            (cutoffSlider->isVisible()) ? nullptr : &cutoffLabel,
            cutoff2Slider.get());
        appendLabelAndComponentIfVisible(regions[0], sh, ph, &rpLabel,
                                         rpSlider.get());
        appendLabelAndComponentIfVisible(regions[0], sh, ph, &rsLabel,
                                         rsSlider.get());
        appendLabelAndComponentIfVisible(regions[0], sh, ph, &qualityLabel,
                                         qualitySlider.get());
        appendLabelAndComponentIfVisible(regions[0], sh, ph, &gainDBLabel,
                                         gainDBSlider.get());
    }
}
