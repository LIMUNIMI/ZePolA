/*
  ==============================================================================

    PlotsPanel.cpp

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

#include "PlotsPanel.h"
#include "../Macros.h"
#include "../Mappers.h"
#include "CustomButtons.h"
#include "LookAndFeel.h"

// =============================================================================
PlotComponent::PlotComponent(size_t n_points)
    : y_values(n_points, 0.0f)
    , x_values(n_points, 0.0f)
    , period(-1.0f)
    , y_grid({0.0f})
    , y_labels({""})
    , x_grid({0.0f})
    , x_labels({""})
    , log_x(false)
    , topRightText()
{
}

// =============================================================================
void PlotComponent::buttonClicked(juce::Button* b)
{
    setLogX(b->getToggleState());
}
void PlotComponent::buttonStateChanged(juce::Button*) {}

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
float PlotComponent::getYMin() { return y_grid.front(); }
float PlotComponent::getYMax() { return y_grid.back(); }

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
PlotsPanel::UnsafeOutputWarningPanel::UnsafeOutputWarningPanel()
    : message(
        "",
        "Caution! The current plugin configuration has caused an excessively "
        "high output, and the audio stream has been stopped. Please reset the "
        "plugin parameters to values that allow for a lower output volume.")
{
    message.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(message);
}
void PlotsPanel::UnsafeOutputWarningPanel::valueChanged(juce::Value& value)
{
    setVisible(value.getValue());
}
void PlotsPanel::UnsafeOutputWarningPanel::resized()
{
    auto r = getLocalBounds();
    r.setHeight(getHeight() / 2);
    message.setBounds(r);
}

// =============================================================================
PlotsPanel::PlotsPanel(ZePolAudioProcessor& p,
                       juce::ApplicationProperties& properties)
    : processor(p)
    , db(false)
    , linLogFreqButton(new LabelledToggleButton(
          "LIN", "LOG", CustomLookAndFeel::ColourIDs::PlotButtons_linColourId,
          CustomLookAndFeel::ColourIDs::PlotButtons_logColourId, false, true))
    , linLogAmpButton(new LabelledToggleButton(
          "LIN", "DB", CustomLookAndFeel::ColourIDs::PlotButtons_linColourId,
          CustomLookAndFeel::ColourIDs::PlotButtons_logColourId, false, true))
    , shouldRecomputePoints(true)
    , mLabel("", "MAGNITUDE RESPONSE")
    , pLabel("", "PHASE RESPONSE")
{
    addAndMakeVisible(*linLogFreqButton.get());
    addAndMakeVisible(*linLogAmpButton.get());
    addAndMakeVisible(mPlot);
    addAndMakeVisible(pPlot);
    addAndMakeVisible(mLabel);
    addAndMakeVisible(pLabel);
    addAndMakeVisible(uowPanel);
    mLabel.setJustificationType(juce::Justification::centred);
    pLabel.setJustificationType(juce::Justification::centred);
    for (auto i : processor.parameterIDs())
        processor.addParameterListener(i, this);
    linLogFreqButton->addListener(&mPlot);
    linLogFreqButton->addListener(&pPlot);
    linLogFreqButton->addListener(this);
    linLogAmpButton->addListener(this);
    linLogFreqAPAttachment.reset(new ApplicationPropertiesButtonAttachment(
        properties, "linLogFreq", linLogFreqButton));
    linLogAmpAPAttachment.reset(new ApplicationPropertiesButtonAttachment(
        properties, "linLogAmp", linLogAmpButton));
    processor.addSampleRateListener(this);
    processor.addUnsafeOutputListener(&uowPanel);
}
PlotsPanel::~PlotsPanel()
{
    processor.removeUnsafeOutputListener(&uowPanel);
    processor.removeSampleRateListener(this);
    for (auto i : processor.parameterIDs())
        processor.removeParameterListener(i, this);
    linLogFreqButton->removeListener(&mPlot);
    linLogFreqButton->removeListener(&pPlot);
    linLogFreqButton->removeListener(this);
    linLogAmpButton->removeListener(this);
}

// =============================================================================
void PlotsPanel::sampleRateChangedCallback(double /* sr */)
{
    recomputePoints();
}

// =============================================================================
void PlotsPanel::updateValues()
{
    shouldRecomputePoints = false;
    auto n                = mPlot.getSize();
    auto sr               = processor.getSampleRate();
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
    repaint();
    mPlot.repaint();
    pPlot.repaint();
}

// =============================================================================
void PlotsPanel::buttonClicked(juce::Button* b)
{
    if (b == linLogAmpButton.get()) db = b->getToggleState();
    recomputePoints();
}
void PlotsPanel::buttonStateChanged(juce::Button*) {}
void PlotsPanel::parameterChanged(const juce::String&, float)
{
    recomputePoints();
}
void PlotsPanel::resized()
{
    if (auto claf = dynamic_cast<CustomLookAndFeel*>(&getLookAndFeel()))
    {
        auto regions = claf->splitProportionalPanel(
            claf->getPanelInnerRect(getLocalBounds()));
        jassert(regions.size() == 5);
        auto middle_row = claf->splitProportionalLinLogRow(regions[2]);
        jassert(middle_row.size() == 5);

        mPlot.setBounds(regions[1]);
        pPlot.setBounds(regions[3]);
        linLogAmpButton->setBounds(middle_row[1]);
        linLogFreqButton->setBounds(middle_row[3]);

        regions[4].setX(middle_row[2].getX());
        regions[4].setWidth(middle_row[2].getWidth());
        regions[4].setBottom(getHeight());

        mLabel.setBounds(middle_row[2]);
        pLabel.setBounds(regions[4]);
    }
    uowPanel.setBounds(getLocalBounds());
}
void PlotsPanel::paint(juce::Graphics& g)
{
    if (shouldRecomputePoints) updateValues();
    juce::GroupComponent::paint(g);
}
void PlotsPanel::recomputePoints()
{
    shouldRecomputePoints = true;
    SAFE_MessageManager_LOCK(this, repaint(););
}
