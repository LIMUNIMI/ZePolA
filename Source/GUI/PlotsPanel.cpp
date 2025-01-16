#include "PlotsPanel.h"
#include "LookAndFeel.h"

// =============================================================================
PlotComponent::PlotComponent(size_t n_points)
    : y_values(n_points, 0.0f)
    , x_values(n_points, 0.0f)
    , period(-1.0f)
    , y_grid({-1.0f, 1.0f})
    , y_labels({"-1", "1"})
    , x_grid({-1.0f, 1.0f})
    , x_labels({"-1", "1"})
{
}

// =============================================================================
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
    for (auto s : ticks) labels.push_back(juce::String(s));
    setXGrid(ticks, labels);
}

// =============================================================================
void PlotComponent::paint(juce::Graphics& g)
{
    if (auto laf
        = dynamic_cast<PlotComponentLookAndFeelMethods*>(&getLookAndFeel()))
        laf->drawPlotComponent(
            g, static_cast<float>(getX()), static_cast<float>(getY()),
            static_cast<float>(getWidth()), static_cast<float>(getHeight()),
            x_values, y_values, period, x_grid, y_grid, x_labels, y_labels,
            *this);
}

// =============================================================================
PlotsPanel::PlotsPanel(PolesAndZerosEQAudioProcessor& p)
    : processor(p)
    , timer_ms(20)
    , callbackTimer(std::bind(&PlotsPanel::updateValues, this))
{
    addAndMakeVisible(mPlot);
    addAndMakeVisible(pPlot);
    for (auto i : processor.parameterIDs())
        processor.addParameterListener(i, this);
    callbackTimer.startTimer(timer_ms);
}
PlotsPanel::~PlotsPanel()
{
    for (auto i : processor.parameterIDs())
        processor.removeParameterListener(i, this);
}

// =============================================================================
void PlotsPanel::updateValues()
{
    callbackTimer.stopTimer();
    auto n  = mPlot.getSize();
    auto sr = processor.getSampleRate();
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        claf->setMagnitudePlotProperties(mPlot, sr);
        claf->setPhasePlotProperties(pPlot, sr);
    }
    for (auto i = 0; i < n; ++i)
    {
        auto omega = (i * juce::MathConstants<double>::pi) / (n - 1);
        auto nu    = static_cast<float>(omega * sr
                                     / juce::MathConstants<double>::twoPi);
        auto h     = processor.dtft(omega);
        mPlot.setPoint(i, nu, static_cast<float>(abs(h)));
        pPlot.setPoint(i, nu, static_cast<float>(std::arg(h)));
    }
}

// =============================================================================
void PlotsPanel::parameterChanged(const juce::String&, float)
{
    if (!callbackTimer.isTimerRunning()) callbackTimer.startTimer(timer_ms);
}
void PlotsPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->splitProportionalPanel(
            claf->getPanelInnerRect(getLocalBounds()));
        jassert(regions.size() == 5);

        mPlot.setBounds(regions[1]);
        pPlot.setBounds(regions[3]);
    }
}
