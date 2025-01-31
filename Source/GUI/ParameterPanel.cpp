#include "ParameterPanel.h"
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
    , mSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(MAGNITUDE_ID_PREFIX + juce::String(i),
                                         mSlider))
    , pSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(PHASE_ID_PREFIX + juce::String(i),
                                         pSlider))
    , fLabel(0.0f, 0)
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
ZPoint::MultiAttachment::MultiAttachment(VTSAudioProcessor& p, ZPoint* z, int i)
    : processor(p), idx(i), m_listen(z), a_listen(z)
{
    juce::String i_str(idx);
    juce::String m_id = MAGNITUDE_ID_PREFIX + i_str;
    juce::String a_id = PHASE_ID_PREFIX + i_str;

    processor.addParameterListener(m_id, &m_listen);
    processor.addParameterListener(a_id, &a_listen);
    m_listen.parameterChanged(m_id, p.getParameterUnnormValue(m_id));
    a_listen.parameterChanged(a_id, p.getParameterUnnormValue(a_id));
}
ZPoint::MultiAttachment::~MultiAttachment()
{
    processor.removeParameterListener(MAGNITUDE_ID_PREFIX + juce::String(idx),
                                      &m_listen);
    processor.removeParameterListener(PHASE_ID_PREFIX + juce::String(idx),
                                      &a_listen);
}

// =============================================================================
ZPoint::ZPoint() : z(0.0f, 0.0f) {}

// =============================================================================
void ZPoint::setPointX(float x)
{
    z = std::complex<float>(x, getPointY());
    setBoundsRelativeToParent();
}
void ZPoint::setPointY(float y)
{
    z = std::complex<float>(getPointX(), y);
    setBoundsRelativeToParent();
}
void ZPoint::setPointMagnitude(float m)
{
    z = std::polar<float>(m, getPointArg());
    setBoundsRelativeToParent();
}
void ZPoint::setPointArg(float a)
{
    z = std::polar<float>(getPointMagnitude(), a);
    setBoundsRelativeToParent();
}
float ZPoint::getPointX() const { return z.real(); }
float ZPoint::getPointY() const { return z.imag(); }
float ZPoint::getPointMagnitude() const { return abs(z); }
float ZPoint::getPointArg() const { return arg(z); }

// =============================================================================
void ZPoint::setBoundsRelativeToPlane(juce::Component* parent, float radius)
{
    juce::Point<float> c(getPointX(), getPointY());
    juce::Point<float> scale(parent->getWidth() * 0.5f / radius,
                             parent->getHeight() * -0.5f / radius);
    juce::Point<float> offset(0.5f * parent->getWidth(),
                              0.5f * parent->getHeight());
    c *= scale;
    c += offset;

    juce::Rectangle<float> r(0.0f, 0.0f, static_cast<float>(parent->getWidth()),
                             static_cast<float>(parent->getHeight()));
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        r *= claf->getRelativePointSize();
    }
    else
    {
        r *= 0.05f;
    }
    r.setCentre(c);

    setBounds(r.toNearestInt());
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
    setBoundsRelativeToPlane(getParentComponent());
}

// =============================================================================
void ZPoint::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<ZPoint::LookAndFeelMethods*>(&getLookAndFeel()))
        laf->drawZPoint(g, getX(), getY(), getWidth(), getHeight(), getPointX(),
                        getPointY(), *this);
}

// =============================================================================
GaussianPlanePanel::GaussianPlanePanel(PolesAndZerosEQAudioProcessor& p)
    : radius(1.05f)
{
    jassert(radius > 0.0f);
    for (auto i = 0; i < 1 /*p.getNElements()*/; ++i)
    {
        points.push_back(std::make_unique<ZPoint>());
        addAndMakeVisible(*points[i].get());
        point_attachments.push_back(
            std::make_unique<ZPoint::MultiAttachment>(p, points[i].get(), i));
    }
    jassert(point_attachments.size() == points.size());
}

// =============================================================================
void GaussianPlanePanel::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<GaussianPlanePanel::LookAndFeelMethods*>(
            &getLookAndFeel()))
        laf->drawGaussianPlane(g, getX(), getY(), getWidth(), getHeight(),
                               radius, *this);
}
void GaussianPlanePanel::resized()
{
    for (auto& p : points) p->setBoundsRelativeToPlane(this, radius);
}

// =============================================================================
void GaussianPlanePanel::setRadius(float r) { radius = r; }
float GaussianPlanePanel::getRadius() const { return radius; }

// =============================================================================
ParameterPanel::ParameterPanel(PolesAndZerosEQAudioProcessor& p, size_t n)
    : zplane(p)
{
    for (auto s : {"RADIUS", "ANGLE", "TYPE", "ACTIVE", "GAIN"})
        headerLabels.push_back(
            std::make_unique<juce::Label>(juce::String(), s));
    for (auto i = 0; i < n; ++i)
        strips.push_back(std::make_unique<ParameterStrip>(p, i));

    for (auto& l : headerLabels)
    {
        l->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(*l.get());
    }
    for (auto& s : strips) addAndMakeVisible(*s.get());
    addAndMakeVisible(zplane);
    addAndMakeVisible(shortcutsPanel);
}

// =============================================================================
void ParameterPanel::paint(Graphics& g)
{
    juce::GroupComponent::paint(g);
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        float x     = static_cast<float>(getX());
        float width = static_cast<float>(getWidth());
        std::vector<float> y;
        if (!strips.empty()) y.push_back(static_cast<float>(strips[0]->getY()));
        for (auto& s : strips) y.push_back(static_cast<float>(s->getBottom()));
        claf->drawParameterStripSeparators(g, x, y, width, *this);
    }
}
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
        for (auto i = 0; i < n; ++i) strips[i]->setBounds(strip_rects[i]);

        zplane.setBounds(regions[3].removeFromLeft(regions[3].getHeight()));
        regions[3].setLeft(claf->getPanelInnerRect(regions[3]).getX());
        shortcutsPanel.setBounds(regions[3]);
    }
}
