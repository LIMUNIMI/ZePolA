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
ParameterStrip::ParentRepaintButtonListener::ParentRepaintButtonListener() { }

// =============================================================================
void ParameterStrip::ParentRepaintButtonListener::buttonClicked(juce::Button* b)
{
    b->getParentComponent()->repaint();
}
void ParameterStrip::ParentRepaintButtonListener::buttonStateChanged(
    juce::Button* b)
{
    buttonClicked(b);
}

// =============================================================================
ParameterStrip::ParameterStrip(VTSAudioProcessor& p, int i)
    : mSliderAttachment(
        p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                         juce::Slider>(MAGNITUDE_ID_PREFIX + juce::String(i),
                                       mSlider))
    , pSliderAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::SliderAttachment,
                           juce::Slider>(PHASE_ID_PREFIX + juce::String(i),
                                         pSlider))
    , aButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(ACTIVE_ID_PREFIX + juce::String(i),
                                         aButton))
    , tButtonAttachment(
          p.makeAttachment<juce::AudioProcessorValueTreeState::ButtonAttachment,
                           juce::Button>(TYPE_ID_PREFIX + juce::String(i),
                                         tButton))
{
    aButton.addListener(&aButtonListener);
    addAndMakeVisible(mSlider);
    addAndMakeVisible(pSlider);
    addAndMakeVisible(aButton);
    addAndMakeVisible(tButton);
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
        tButton.setBounds(rects[3]);
        aButton.setBounds(rects[4]);
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
ParameterPanel::ParameterPanel(VTSAudioProcessor& p, size_t n)
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
        jassert(regions.size() == 4);

        regions[0].setTop(0);
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
    }
}
