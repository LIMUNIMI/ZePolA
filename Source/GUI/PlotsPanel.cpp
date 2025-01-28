#include "PlotsPanel.h"
#include "../Mappers.h"
#include "LookAndFeel.h"

// =============================================================================
PlotComponent::PlotComponent(size_t n_points)
    : y_values(n_points, 0.0f)
    , x_values(n_points, 0.0f)
    , period(-1.0f)
    , y_grid({1.0f, 2.0f})
    , y_labels({"1", "2"})
    , x_grid({1.0f, 2.0f})
    , x_labels({"1", "2"})
    , log_x(false)
    , topRightText()
{
}

// =============================================================================
void PlotComponent::buttonClicked(juce::Button* b)
{
    setLogX(b->getToggleState());
}
void PlotComponent::buttonStateChanged(juce::Button* b)
{
    setLogX(b->getToggleState());
}

// =============================================================================
void PlotComponent::setTopRightText(const juce::String& s) { topRightText = s; }
void PlotComponent::setLogX(bool b)
{
    log_x = b;
    repaint();
}
bool PlotComponent::getLogX() { return log_x; }
void PlotComponent::setPeriod(float p) { period = p; }
size_t PlotComponent::getSize() { return y_values.size(); }
void PlotComponent::setPoint(int i, float x, float y)
{
    x_values[i] = x;
    y_values[i] = y;
    repaint();
}
void PlotComponent::setYGrid(const std::vector<float>& ticks,
                             const std::vector<juce::String>& labels)
{
    jassert(ticks.size() > 1);
    jassert(labels.size() == ticks.size());
    y_grid   = ticks;
    y_labels = labels;
}
void PlotComponent::setYGrid(const std::vector<float>& ticks)
{
    std::vector<juce::String> labels;
    for (auto s : ticks) labels.push_back(juce::String(s));
    setYGrid(ticks, labels);
}
void PlotComponent::setXGrid(const std::vector<float>& ticks,
                             const std::vector<juce::String>& labels)
{
    jassert(labels.size() == ticks.size());
    x_grid   = ticks;
    x_labels = labels;
}
void PlotComponent::setXGrid(const std::vector<float>& ticks)
{
    std::vector<juce::String> labels;
    for (auto s : ticks)
        labels.push_back((s < 1000) ? juce::String(s)
                                    : juce::String(s / 1000) + "k");
    setXGrid(ticks, labels);
}
float PlotComponent::getYMin() { return y_grid[0]; }
float PlotComponent::getYMax() { return y_grid[y_grid.size() - 1]; }

// =============================================================================
void PlotComponent::paint(juce::Graphics& g)
{
    if (auto laf
        = dynamic_cast<PlotComponentLookAndFeelMethods*>(&getLookAndFeel()))
        laf->drawPlotComponent(
            g, static_cast<float>(getX()), static_cast<float>(getY()),
            static_cast<float>(getWidth()), static_cast<float>(getHeight()),
            x_values, y_values, period, x_grid, y_grid, x_labels, y_labels,
            log_x, topRightText, *this);
}

// =============================================================================
PlotsPanel::PlotsPanel(PolesAndZerosEQAudioProcessor& p,
                       juce::ApplicationProperties& properties)
    : processor(p)
    , timer_ms(20)
    , db(false)
    , callbackTimer(std::bind(&PlotsPanel::updateValues, this))
    , linLogFreqButton(new juce::ToggleButton())
{
    addAndMakeVisible(*linLogFreqButton.get());
    addAndMakeVisible(linLogAmpButton);
    addAndMakeVisible(mPlot);
    addAndMakeVisible(pPlot);
    for (auto i : processor.parameterIDs())
        processor.addParameterListener(i, this);
    callbackTimer.startTimer(timer_ms);
    linLogFreqButton->addListener(&mPlot);
    linLogFreqButton->addListener(&pPlot);
    linLogFreqButton->addListener(this);
    linLogAmpButton.addListener(this);
    linLogFreqAttachment.reset(new ApplicationPropertiesButtonAttachment(
        properties, "linLogFreq", linLogFreqButton));
}
PlotsPanel::~PlotsPanel()
{
    for (auto i : processor.parameterIDs())
        processor.removeParameterListener(i, this);
    linLogFreqButton->removeListener(&mPlot);
    linLogFreqButton->removeListener(&pPlot);
    linLogFreqButton->removeListener(this);
    linLogAmpButton.removeListener(this);
}

// =============================================================================
void PlotsPanel::updateValues()
{
    callbackTimer.stopTimer();
    auto n  = mPlot.getSize();
    auto sr = processor.getSampleRate();
    mPlot.setTopRightText("Sample rate: " + juce::String(sr) + " Hz");
    double loFreq = (mPlot.getLogX()) ? 10.0 : 0.0;
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        claf->setMagnitudePlotProperties(mPlot, sr, db);
        claf->setPhasePlotProperties(pPlot, sr);
        if (mPlot.getLogX()) loFreq = claf->getLogPlotLowFreq(sr);
    }
    OutputTransformMapper<float> omegaTransform(
        0.0f,
        static_cast<float>(loFreq * juce::MathConstants<double>::twoPi / sr),
        n - 1.0f, juce::MathConstants<float>::pi,
        (mPlot.getLogX()) ? static_cast<float (*)(float)>(logf)
                          : identity<float>,
        (mPlot.getLogX()) ? static_cast<float (*)(float)>(expf)
                          : identity<float>);
    auto m_min_db = mPlot.getYMin() - 6.0f;
    for (auto i = 0; i < n; ++i)
    {
        auto omega = omegaTransform.map(static_cast<float>(i));
        auto nu    = static_cast<float>(omega * sr
                                     / juce::MathConstants<double>::twoPi);
        auto h     = processor.dtft(omega);
        auto m     = static_cast<float>(abs(h));
        if (db) m = juce::Decibels::gainToDecibels(m, m_min_db);
        mPlot.setPoint(i, nu, m);
        pPlot.setPoint(i, nu, static_cast<float>(std::arg(h)));
    }
}

// =============================================================================
void PlotsPanel::startTimer()
{
    if (!callbackTimer.isTimerRunning()) callbackTimer.startTimer(timer_ms);
}
void PlotsPanel::buttonClicked(juce::Button* b)
{
    if (b == &linLogAmpButton)
    {
        db = b->getToggleState();
    }
    startTimer();
}
void PlotsPanel::buttonStateChanged(juce::Button* b)
{
    if (b == &linLogAmpButton)
    {
        db = b->getToggleState();
    }
    startTimer();
}
void PlotsPanel::parameterChanged(const juce::String&, float) { startTimer(); }
void PlotsPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->splitProportionalPanel(
            claf->getPanelInnerRect(getLocalBounds()));
        jassert(regions.size() == 5);
        auto middle_regions
            = claf->splitProportional(regions[2], {1, 4, 1}, true);
        jassert(middle_regions.size() == 3);
        middle_regions
            = claf->splitProportional(middle_regions[1], {1, 8, 62, 8, 1});
        jassert(middle_regions.size() == 5);

        mPlot.setBounds(regions[1]);
        pPlot.setBounds(regions[3]);

        linLogAmpButton.setBounds(middle_regions[1]);
        linLogFreqButton->setBounds(middle_regions[3]);
    }
}
