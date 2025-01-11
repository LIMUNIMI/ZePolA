#pragma once
#include "../Parameters.h"
#include "../PluginProcessor.h"
#include <JuceHeader.h>

// =============================================================================
/** Plots panel  */
class PlotComponent : public juce::GroupComponent
{
public:
    // =========================================================================
    PlotComponent(size_t n_points = 1024);

    //==========================================================================
    /** Set minimum y value */
    void setYMin(float);
    /** Set maximum y value */
    void setYMax(float);
    /** Get number of points */
    size_t getSize();
    /** Set point value */
    void setPoint(int, float);

    //==========================================================================
    void paint(juce::Graphics&) override;

private:
    // =========================================================================
    std::vector<float> y_values, x_values;
    float y_min, y_max;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlotComponent);
};

// =============================================================================
/** LookAndFeelMethods for plots panels  */
class PlotComponentLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void drawPlotComponent(juce::Graphics&, float x, float y,
                                   float width, float height,
                                   const std::vector<float>& y_values,
                                   float y_min, float y_max, PlotComponent&)
        = 0;
};

// =============================================================================
/** Plots panel  */
class PlotsPanel : public juce::GroupComponent,
                   public juce::AudioProcessorValueTreeState::Listener
{
public:
    // =========================================================================
    PlotsPanel(PolesAndZerosEQAudioProcessor&);
    ~PlotsPanel();

    //==========================================================================
    void updateValues();

    //==========================================================================
    void parameterChanged(const juce::String&, float);
    void resized() override;

private:
    // =========================================================================
    PolesAndZerosEQAudioProcessor& processor;
    PlotComponent mPlot, pPlot;
    juce::TimedCallback callbackTimer;
    int timer_ms;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlotsPanel);
};
