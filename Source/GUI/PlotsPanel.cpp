#include "PlotsPanel.h"
#include "LookAndFeel.h"

// =============================================================================
PlotComponent::PlotComponent(size_t n_points)
    : y_values(n_points, 0.0f)
    , x_values(n_points, 0.0f)
    , y_min(-1.0f)
    , y_max(1.0f)
    , period(-1.0f)
{
}

// =============================================================================
void PlotComponent::setPeriod(float p) { period = p; }
void PlotComponent::setYMin(float f) { y_min = f; }
void PlotComponent::setYMax(float f) { y_max = f; }
size_t PlotComponent::getSize() { return y_values.size(); }
void PlotComponent::setPoint(int i, float f)
{
    y_values[i] = f;
    repaint();
}

// =============================================================================
void PlotComponent::paint(juce::Graphics& g)
{
    if (auto laf
        = dynamic_cast<PlotComponentLookAndFeelMethods*>(&getLookAndFeel()))
        laf->drawPlotComponent(
            g, static_cast<float>(getX()), static_cast<float>(getY()),
            static_cast<float>(getWidth()), static_cast<float>(getHeight()),
            y_values, y_min, y_max, period, *this);
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
    auto n = mPlot.getSize();
    mPlot.setYMin(0.0);
    mPlot.setYMax(2.0);
    mPlot.setPeriod();
    pPlot.setYMin(-juce::MathConstants<float>::pi);
    pPlot.setYMax(juce::MathConstants<float>::pi);
    pPlot.setPeriod(juce::MathConstants<float>::twoPi);
    for (auto i = 0; i < n; ++i)
    {
        auto omega = (i * juce::MathConstants<double>::pi) / (n - 1);
        auto h     = processor.dtft(omega);
        mPlot.setPoint(i, static_cast<float>(abs(h)));
        pPlot.setPoint(i, static_cast<float>(std::arg(h)));
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
        jassert(regions.size() == 4);

        mPlot.setBounds(regions[1]);
        pPlot.setBounds(regions[3]);
    }
}
