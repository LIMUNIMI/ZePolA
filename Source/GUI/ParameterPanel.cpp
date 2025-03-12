#include "ParameterPanel.h"
#include "../Macros.h"
#include "LookAndFeel.h"

// =============================================================================
ParameterSlider::ParameterSlider()
{
    setSliderStyle(juce::Slider::LinearHorizontal);
    setTextBoxStyle(juce::Slider::TextBoxRight, false, getTextBoxWidth(),
                    getTextBoxHeight());
}

// =============================================================================
ParameterStrip::ParentRepaintButtonListener::ParentRepaintButtonListener() {}

// =============================================================================
void ParameterStrip::ParentRepaintButtonListener::buttonClicked(juce::Button* b)
{
    b->getParentComponent()->repaint();
}
void ParameterStrip::ParentRepaintButtonListener::buttonStateChanged(
    juce::Button*)
{
}

// =============================================================================
ParameterStrip::FrequencyLabelSampleRateListener::
    FrequencyLabelSampleRateListener(DraggableLabelAttachment& a)
    : dla(a)
{
}
void ParameterStrip::FrequencyLabelSampleRateListener::
    sampleRateChangedCallback(double sr)
{
    dla.setScale(static_cast<float>(sr * 0.5));
}

// =============================================================================
ParameterStrip::ParameterStrip(VTSAudioProcessor& p, int i)
    : processor(p)
    , tButton(FilterElement::typeToString(0), FilterElement::typeToString(1),
              CustomLookAndFeel::ColourIDs::ZPoint_zerosColourId,
              CustomLookAndFeel::ColourIDs::ZPoint_polesColourId, false, true)
    , mSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(MAGNITUDE_ID_PREFIX + juce::String(i),
                                         mSlider))
    , pSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(PHASE_ID_PREFIX + juce::String(i),
                                         pSlider))
    , fLabel(0.0f, 0)
    , gLabel(0.0f, 3)
    , fLabelAttachment(
          p.makeAttachment<DraggableLabelAttachment, DraggableLabel>(
              PHASE_ID_PREFIX + juce::String(i), fLabel))
    , aButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(ACTIVE_ID_PREFIX + juce::String(i),
                                         aButton))
    , tButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(TYPE_ID_PREFIX + juce::String(i),
                                         tButton))
    , gLabelAttachment(
          p.makeAttachment<DraggableLabelAttachment, DraggableLabel>(
              GAIN_ID_PREFIX + juce::String(i), gLabel))
{
    srListener = std::make_unique<FrequencyLabelSampleRateListener>(
        *fLabelAttachment.get());
    p.addSampleRateListener(srListener.get());
    aButton.addListener(&aButtonListener);

    addAndMakeVisible(mSlider);
    addAndMakeVisible(pSlider);
    addAndMakeVisible(fLabel);
    addAndMakeVisible(aButton);
    addAndMakeVisible(tButton);
    addAndMakeVisible(gLabel);
}
ParameterStrip::~ParameterStrip()
{
    processor.removeSampleRateListener(srListener.get());
    // Reset this before the draggable label attachment
    srListener.reset();
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
        fLabel.setBounds(
            rects[2]
                .withHeight(juce::roundToInt(fLabel.getFont().getHeight()))
                .withCentre(rects[2].getCentre()));
        tButton.setBounds(rects[3]);
        aButton.setBounds(rects[4]);
        gLabel.setBounds(
            rects[5]
                .withHeight(juce::roundToInt(gLabel.getFont().getHeight()))
                .withCentre(rects[5].getCentre()));

        claf->resizeToggleButton(tButton);
        claf->resizeToggleButton(aButton);
        claf->resizeSlider(mSlider);
        claf->resizeSlider(pSlider);
    }
}

// =============================================================================
bool ParameterStrip::isActive() { return aButton.getToggleState(); }
bool ParameterStrip::parentComponentIsActive(const juce::Component& c)
{
    auto ps = dynamic_cast<ParameterStrip*>(c.getParentComponent());
    return !ps || ps->isActive();
}

// =============================================================================
ZPoint::MagnitudeListener::MagnitudeListener(ZPoint* p) : parent(p) {}
void ZPoint::MagnitudeListener::parameterChanged(const juce::String&, float m)
{
    parent->setPointMagnitude(m);
}

// =============================================================================
ZPoint::ArgListener::ArgListener(ZPoint* p) : parent(p) {}
void ZPoint::ArgListener::parameterChanged(const juce::String&, float a)
{
    parent->setPointArg(juce::MathConstants<float>::pi * a);
}

// =============================================================================
ZPoint::ActiveListener::ActiveListener(ZPoint* p) : parent(p) {}
void ZPoint::ActiveListener::parameterChanged(const juce::String&, float a)
{
    parent->setVisible(a);
}

// =============================================================================
ZPoint::TypeListener::TypeListener(ZPoint* p) : parent(p) {}
void ZPoint::TypeListener::parameterChanged(const juce::String&, float a)
{
    parent->setType(FilterElement::floatToType(a));
}

// =============================================================================
ZPoint::DraggablePointListener::DraggablePointListener(
    juce::RangedAudioParameter* r, juce::RangedAudioParameter* a)
    : r_param(r), a_param(a)
{
}
void ZPoint::DraggablePointListener::mouseDrag(const juce::MouseEvent& event)
{
    if (auto z = dynamic_cast<ZPoint*>(event.eventComponent))
    {
        Parameters::setParameterValue(r_param, z->getPointMagnitude());
        Parameters::setParameterValue(
            a_param, z->getPointArg() / juce::MathConstants<float>::pi);
    }
}

// =============================================================================
ZPoint::ScrollablePointListener::ScrollablePointListener(
    juce::RangedAudioParameter* p)
    : param(p), deltaGain(0.025f)
{
}
void ZPoint::ScrollablePointListener::mouseWheelMove(
    const juce::MouseEvent& /* event */, const juce::MouseWheelDetails& wheel)
{
    Parameters::setParameterValue(
        param, param->convertFrom0to1(std::clamp(
                   param->getValue() + deltaGain * wheel.deltaY, 0.0f, 1.0f)));
}
void ZPoint::ScrollablePointListener::setDeltaGain(float f) { deltaGain = f; }
float ZPoint::ScrollablePointListener::getDeltaGain() { return deltaGain; }

// =============================================================================
ZPoint::TogglableTypePointListener::TogglableTypePointListener(
    juce::RangedAudioParameter* p)
    : param(p)
{
}
void ZPoint::TogglableTypePointListener::mouseDoubleClick(
    const juce::MouseEvent&)
{
    FilterElement::Type t = FilterElement::ZERO;
    switch (
        FilterElement::floatToType(param->convertFrom0to1(param->getValue())))
    {
    default:
        UNHANDLED_SWITCH_CASE("Unhandled case for filter element type. "
                              "Switching to type 'ZERO'");
    case FilterElement::POLE: break;
    case FilterElement::ZERO: t = FilterElement::POLE; break;
    }
    Parameters::setParameterValue(param, FilterElement::typeToFloat(t));
}

// =============================================================================
ZPoint::RickClickableActivePointListener::RickClickableActivePointListener(
    juce::RangedAudioParameter* p)
    : param(p)
{
}
void ZPoint::RickClickableActivePointListener::mouseDown(
    const juce::MouseEvent& e)
{
    if (e.mods.isRightButtonDown())
        Parameters::setParameterValue(
            param, param->convertFrom0to1(1.0f - param->getValue()));
}

// =============================================================================
ZPoint::MultiAttachment::MultiAttachment(VTSAudioProcessor& p, ZPoint* z, int i)
    : processor(p)
    , point(z)
    , m_id(MAGNITUDE_ID_PREFIX + juce::String(i))
    , a_id(PHASE_ID_PREFIX + juce::String(i))
    , v_id(ACTIVE_ID_PREFIX + juce::String(i))
    , t_id(TYPE_ID_PREFIX + juce::String(i))
    , g_id(GAIN_ID_PREFIX + juce::String(i))
    , m_listen(z)
    , a_listen(z)
    , v_listen(z)
    , t_listen(z)
    , z_p_listen(p.getParameterById(m_id), p.getParameterById(a_id))
    , g_p_listen(p.getParameterById(g_id))
    , t_p_listen(p.getParameterById(t_id))
    , v_p_listen(p.getParameterById(v_id))
{
    processor.addParameterListener(m_id, &m_listen);
    processor.addParameterListener(a_id, &a_listen);
    processor.addParameterListener(v_id, &v_listen);
    processor.addParameterListener(t_id, &t_listen);
    point->addMouseListener(&z_p_listen, false);
    point->addMouseListener(&g_p_listen, false);
    point->addMouseListener(&t_p_listen, false);
    point->addMouseListener(&v_p_listen, false);

    m_listen.parameterChanged(m_id, p.getParameterUnnormValue(m_id));
    a_listen.parameterChanged(a_id, p.getParameterUnnormValue(a_id));
    v_listen.parameterChanged(v_id, p.getParameterUnnormValue(v_id));
    t_listen.parameterChanged(t_id, p.getParameterUnnormValue(t_id));
}
ZPoint::MultiAttachment::~MultiAttachment()
{
    point->removeMouseListener(&z_p_listen);
    point->removeMouseListener(&g_p_listen);
    point->removeMouseListener(&t_p_listen);
    point->removeMouseListener(&v_p_listen);
    processor.removeParameterListener(m_id, &m_listen);
    processor.removeParameterListener(a_id, &a_listen);
    processor.removeParameterListener(v_id, &v_listen);
    processor.removeParameterListener(t_id, &t_listen);
}

// =============================================================================
ZPoint::ZPoint()
    : r(0.0f)
    , a(0.0f)
    , type(FilterElement::Type::ZERO)
    , conjugate(false)
    , z_conj(nullptr)
{
}

// =============================================================================
void ZPoint::setPointXY(float x, float y)
{
    setPointMagnitude(sqrt(x * x + y * y));
    setPointArg(atan2(y, x));
}
void ZPoint::setPointX(float x) { setPointXY(x, getPointY()); }
void ZPoint::setPointY(float y) { setPointXY(getPointX(), y); }
void ZPoint::setPointMagnitude(float f)
{
    r = std::clamp(f, 0.0f, 1.0f);
    if (z_conj) z_conj->setPointMagnitude(r);
    setBoundsRelativeToParent();
}
void ZPoint::setPointArg(float f)
{
    a = std::fmod((conjugate) ? -f : f, juce::MathConstants<float>::twoPi);
    if (a < 0.0f) a += juce::MathConstants<float>::twoPi;
    if (a > 3 * juce::MathConstants<float>::halfPi)
        a = 0.0f;
    else if (a > juce::MathConstants<float>::pi)
        a = juce::MathConstants<float>::pi;
    if (conjugate) a = -a;
    if (z_conj) z_conj->setPointArg(-a);
    setBoundsRelativeToParent();
}
float ZPoint::getPointX() const { return r * cos(a); }
float ZPoint::getPointY() const { return r * sin(a); }
float ZPoint::getPointMagnitude() const { return r; }
float ZPoint::getPointArg() const { return a; }
void ZPoint::setType(FilterElement::Type t)
{
    type = t;
    if (z_conj) z_conj->setType(t);
    repaint();
}
FilterElement::Type ZPoint::getType() const { return type; }
void ZPoint::setConjugate(bool c)
{
    conjugate = c;
    repaint();
}
bool ZPoint::getConjugate() const { return conjugate; }

// =============================================================================
void ZPoint::setBoundsRelativeToPlane(juce::Component* parent, float radius)
{
    jassert(parent);
    juce::Point<float> c(getPointX(), getPointY());
    juce::Point<float> scale(parent->getWidth() * 0.5f / radius,
                             parent->getHeight() * -0.5f / radius);
    juce::Point<float> offset(0.5f * parent->getWidth(),
                              0.5f * parent->getHeight());
    c *= scale;
    c += offset;

    juce::Rectangle<float> rect(0.0f, 0.0f,
                                static_cast<float>(parent->getWidth()),
                                static_cast<float>(parent->getHeight()));
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        rect *= claf->getRelativePointSize();
    }
    else
    {
        rect *= 0.05f;
    }
    rect.setCentre(c);

    if ((MessageManager::getInstanceWithoutCreating() != nullptr
         && MessageManager::getInstanceWithoutCreating()
                ->currentThreadHasLockedMessageManager())
        || getPeer() == nullptr)
    {
        const MessageManagerLock mmLock;
        setBounds(rect.toNearestInt());
    }
    if (z_conj) z_conj->setBoundsRelativeToPlane(parent, radius);
}
void ZPoint::setBoundsRelativeToPlane(juce::Component* parent)
{
    float radius = 1.0f;
    if (auto zplane = dynamic_cast<GaussianPlanePanel*>(parent))
        radius = zplane->getRadius();
    setBoundsRelativeToPlane(parent, radius);
}
void ZPoint::setBoundsRelativeToParent()
{
    auto parent = getParentComponent();
    if (!parent) return;
    setBoundsRelativeToPlane(parent);
}
void ZPoint::setConjugatePoint(ZPoint* z) { z_conj = z; }

// =============================================================================
void ZPoint::setVisible(bool shouldBeVisible)
{
    juce::Component::setVisible(shouldBeVisible);
    if (z_conj) z_conj->setVisible(shouldBeVisible);
}
void ZPoint::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<ZPoint::LookAndFeelMethods*>(&getLookAndFeel()))
        laf->drawZPoint(
            g, static_cast<float>(getX()), static_cast<float>(getY()),
            static_cast<float>(getWidth()), static_cast<float>(getHeight()),
            getPointX(), getPointY(), type, conjugate, *this);
}
void ZPoint::mouseDrag(const juce::MouseEvent& event)
{
    if (conjugate) return;
    if (auto zplane = findParentComponentOfClass<GaussianPlanePanel>())
    {
        setPointXY(getPointX()
                       + (event.x - getWidth() * 0.5f) * 0.5f
                             * zplane->getRadius() / zplane->getWidth(),
                   getPointY()
                       - (event.y - getHeight() * 0.5f) * 0.5f
                             * zplane->getRadius() / zplane->getHeight());
    }
}

// =============================================================================
GaussianPlanePanel::GaussianPlanePanel(PolesAndZerosEQAudioProcessor& p)
    : radius(1.05f)
{
    auto n = p.getNElements();
    for (auto i = 0; i < n; ++i)
    {
        points.push_back(std::make_unique<ZPoint>());
        conj_points.push_back(std::make_unique<ZPoint>());
        points[i]->setConjugatePoint(conj_points[i].get());
        conj_points[i]->setConjugate(true);

        addChildComponent(*conj_points[i].get(), i);
        addChildComponent(*points[i].get(), n + i);

        point_attachments.push_back(
            std::make_unique<ZPoint::MultiAttachment>(p, points[i].get(), i));
    }
    jassert(points.size() == n);
    jassert(conj_points.size() == n);
    jassert(point_attachments.size() == n);
}
GaussianPlanePanel::~GaussianPlanePanel()
{
    // <!> Order matters <!>
    point_attachments.clear();
    points.clear();
    conj_points.clear();
}

// =============================================================================
void GaussianPlanePanel::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<GaussianPlanePanel::LookAndFeelMethods*>(
            &getLookAndFeel()))
        laf->drawGaussianPlane(g, static_cast<float>(getX()),
                               static_cast<float>(getY()),
                               static_cast<float>(getWidth()),
                               static_cast<float>(getHeight()), radius, *this);
}
void GaussianPlanePanel::resized()
{
    for (auto& p : points) p->setBoundsRelativeToPlane(this, radius);
}
void GaussianPlanePanel::mouseDown(const juce::MouseEvent& e)
{
    juce::Component::mouseDown(e);
    if (e.mods.isRightButtonDown())
        if (auto n = points.size())
        {
            auto pos    = e.getMouseDownPosition().toFloat();
            auto i_min  = n;
            float d_min = std::numeric_limits<float>::infinity(), d;
            for (auto i = 0; i < n; ++i)
                if (!points[i]->isShowing())
                {
                    juce::Point p(
                        points[i]->getX() + points[i]->getWidth() * 0.5f,
                        points[i]->getY() + points[i]->getHeight() * 0.5f);
                    d = pos.getDistanceFrom(p);
                    if (d < d_min)
                    {
                        i_min = i;
                        d_min = d;
                    }
                }
            if (i_min < n)
                point_attachments[i_min]->v_p_listen.mouseDown(
                    e.getEventRelativeTo(points[i_min].get()));
        }
}

// =============================================================================
void GaussianPlanePanel::setRadius(float r)
{
    radius = r;
    jassert(radius > 0.0f);
}
float GaussianPlanePanel::getRadius() const { return radius; }

// =============================================================================
ShortcutsPanel::ShortcutsPanel(PolesAndZerosEQAudioProcessor& p)
    : processor(p)
    , allOnButton("ALL ON")
    , allOffButton("ALL OFF")
    , doublePhaseButton(juce::CharPointer_UTF8("PHASES ×2"))
    , halfPhaseButton(juce::CharPointer_UTF8("PHASES ÷2"))
    , swapTypeButton("SWAP Ps/Zs")
    , panelLabel("", "SHORTCUTS")
{
    allOnButton.onClick       = [this] { triggerAllOn(); };
    allOffButton.onClick      = [this] { triggerAllOff(); };
    doublePhaseButton.onClick = [this] { triggerDoublePhases(); };
    halfPhaseButton.onClick   = [this] { triggerHalfPhases(); };
    swapTypeButton.onClick    = [this] { triggerSwapTypes(); };
    panelLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(panelLabel);
    addAndMakeVisible(allOnButton);
    addAndMakeVisible(allOffButton);
    addAndMakeVisible(doublePhaseButton);
    addAndMakeVisible(halfPhaseButton);
    addAndMakeVisible(swapTypeButton);
}
void ShortcutsPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->splitProportionalShortcuts(getLocalBounds());
        jassert(regions.size() == 13);
        panelLabel.setBounds(regions[1]);
        allOnButton.setBounds(regions[3]);
        allOffButton.setBounds(regions[5]);
        doublePhaseButton.setBounds(regions[7]);
        halfPhaseButton.setBounds(regions[9]);
        swapTypeButton.setBounds(regions[11]);
    }
}

// =============================================================================
void ShortcutsPanel::triggerAllOn()
{
    auto n = processor.getNElements();
    for (auto i = 0; i < n; ++i)
        processor.setParameterValue(ACTIVE_ID_PREFIX + juce::String(i), true);
}
void ShortcutsPanel::triggerAllOff()
{
    auto n = processor.getNElements();
    for (auto i = 0; i < n; ++i)
        processor.setParameterValue(ACTIVE_ID_PREFIX + juce::String(i), false);
}
void ShortcutsPanel::triggerDoublePhases()
{
    auto n = processor.getNElements();
    for (auto i = 0; i < n; ++i)
    {
        auto id_i = PHASE_ID_PREFIX + juce::String(i);
        processor.setParameterValue(
            id_i, 2.0f
                      * std::clamp(processor.getParameterUnnormValue(id_i),
                                   0.0f, 1.0f));
    }
}
void ShortcutsPanel::triggerHalfPhases()
{
    auto n = processor.getNElements();
    for (auto i = 0; i < n; ++i)
    {
        auto id_i = PHASE_ID_PREFIX + juce::String(i);
        processor.setParameterValue(
            id_i, 0.5f
                      * std::clamp(processor.getParameterUnnormValue(id_i),
                                   0.0f, 1.0f));
    }
}
void ShortcutsPanel::triggerSwapTypes()
{
    auto n = processor.getNElements();
    for (auto i = 0; i < n; ++i)
    {
        auto id_i             = TYPE_ID_PREFIX + juce::String(i);
        FilterElement::Type t = FilterElement::ZERO;
        switch (
            FilterElement::floatToType(processor.getParameterUnnormValue(id_i)))
        {
        default:
            UNHANDLED_SWITCH_CASE("Unhandled case for filter element type. "
                                  "Switching to type 'ZERO'");
        case FilterElement::POLE: break;
        case FilterElement::ZERO: t = FilterElement::POLE; break;
        }
        processor.setParameterValue(id_i, FilterElement::typeToFloat(t));
    }
}

// =============================================================================
ParameterPanel::ParameterPanel(PolesAndZerosEQAudioProcessor& p)
    : zplane(p), zplane_label("", "GAUSSIAN PLANE"), shortcutsPanel(p)
{
    for (auto s : {"RADIUS", "ANGLE", "TYPE", "ACTIVE", "GAIN"})
        headerLabels.push_back(
            std::make_unique<juce::Label>(juce::String(), s));
    auto n = p.getNElements();
    for (auto i = 0; i < n; ++i)
        strips.push_back(std::make_unique<ParameterStrip>(p, i));
    for (auto i = 0; i <= n; ++i)
        separators.push_back(std::make_unique<SeparatorComponent>());

    for (auto& s : separators) s->drawBottom = true;
    zplane_label.setJustificationType(juce::Justification::centred);
    for (auto& l : headerLabels)
        l->setJustificationType(juce::Justification::centred);

    for (auto& s : separators) addAndMakeVisible(*s.get());
    for (auto& l : headerLabels) addAndMakeVisible(*l.get());
    for (auto& s : strips) addAndMakeVisible(*s.get());
    addAndMakeVisible(zplane_label);
    addAndMakeVisible(zplane);
    addAndMakeVisible(shortcutsPanel);
}

// =============================================================================
void ParameterPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->splitProportionalPanel(
            claf->getPanelInnerRect(getLocalBounds()));
        jassert(regions.size() == 5);

        regions[0].setTop(0);
        regions[1].setBottom(regions[2].getCentreY());
        auto header_rects = claf->splitProportionalStrip(regions[0]);
        jassert(header_rects.size() == 6);
        header_rects[1].setRight(header_rects[2].getRight());
        header_rects.erase(header_rects.begin() + 2);
        jassert(headerLabels.size() <= header_rects.size());
        auto n = headerLabels.size();
        for (auto i = 0; i < n; ++i)
            headerLabels[i]->setBounds(header_rects[i]);

        n = strips.size();
        auto strip_rects
            = claf->splitProportional(regions[1], std::vector<int>(n, 1), true);
        jassert(separators.size() == n + 1);
        auto gct = claf->getGroupComponentThickness();
        separators.back()->setBounds(
            regions[0]
                .withX(juce::roundToInt(std::ceil(gct)))
                .withWidth(
                    juce::roundToInt(std::floor(getWidth() - 2.0f * gct))));
        for (auto i = 0; i < n; ++i)
        {
            strips[i]->setBounds(strip_rects[i]);
            separators[i]->setBounds(
                strip_rects[i]
                    .withX(separators.back()->getX())
                    .withWidth(separators.back()->getWidth()));
        }

        zplane.setBounds(regions[3].removeFromLeft(regions[3].getHeight()));
        regions[3].setLeft(claf->getPanelInnerRect(regions[3]).getX());
        shortcutsPanel.setBounds(regions[3]);

        juce::Rectangle<int> zplane_label_rect(
            zplane.getX(), zplane.getBottom(), zplane.getWidth(), 0);
        zplane_label_rect.setBottom(getHeight());
        zplane_label.setBounds(zplane_label_rect);
    }
}
