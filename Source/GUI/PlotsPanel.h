#pragma once
#include "../Parameters.h"
#include "../PluginProcessor.h"
#include "ApplicationPropertiesListeners.h"
#include <JuceHeader.h>

// =============================================================================
/** Plots panel  */
class PlotComponent : public juce::Component, public juce::Button::Listener
{
public:
    // =========================================================================
    PlotComponent(size_t n_points = 1024);

    // =====================================================================
    void buttonClicked(juce::Button*) override;
    void buttonStateChanged(juce::Button*) override;

    //==========================================================================
    /** Set text to print in the top right */
    void setTopRightText(const juce::String&);
    /** Set x scale as logarithmic or not */
    void setLogX(bool b = true);
    /** Get if x scale as logarithmic or not */
    bool getLogX();
    /** Set period. Set a negative value for non-periodic plot */
    void setPeriod(float p = -1.0f);
    /** Get number of points */
    size_t getSize();
    /** Set point value */
    void setPoint(int, float x, float y);
    /**
     * Set y grid values and labels
     *
     * @param ticks Tick values
     * @param labels Labels, including the label for the minimum
     *   value (labels.front()) and for the maximum value (labels.back())
     */
    void setYGrid(const std::vector<float>& ticks,
                  const std::vector<juce::String>& labels);
    /** Set y grid values and automatic labels */
    void setYGrid(const std::vector<float>&);
    /**
     * Set x grid values and labels
     *
     * @param ticks Tick values
     * @param labels Labels, including the label for the minimum
     *   value (labels.front()) and for the maximum value (labels.back())
     */
    void setXGrid(const std::vector<float>& ticks,
                  const std::vector<juce::String>& labels);
    /** Set x grid values and automatic labels */
    void setXGrid(const std::vector<float>&);
    /** Get minimum y value in the plot */
    float getYMin();
    /** Get maximum y value in the plot */
    float getYMax();

    //==========================================================================
    void paint(juce::Graphics&) override;

private:
    // =========================================================================
    std::vector<juce::String> y_labels, x_labels;
    std::vector<float> y_values, x_values, y_grid, x_grid;
    juce::String topRightText;
    float period;
    bool log_x;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlotComponent);
};

// =============================================================================
/** LookAndFeelMethods for plots panels  */
class PlotComponentLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void
    drawPlotComponent(juce::Graphics&, float x, float y, float width,
                      float height, const std::vector<float>& x_values,
                      const std::vector<float>& y_values, float period,
                      const std::vector<float>& x_grid,
                      const std::vector<float>& y_grid,
                      const std::vector<juce::String>& x_labels,
                      const std::vector<juce::String>& y_labels, bool log_x,
                      const juce::String& topRightText, PlotComponent&)
        = 0;
};

// =============================================================================
/** Plots panel  */
class PlotsPanel : public juce::GroupComponent,
                   public juce::AudioProcessorValueTreeState::Listener,
                   public juce::Button::Listener,
                   public SampleRateListener
{
public:
    // =========================================================================
    class UnsafeOutputWarningPanel : public juce::GroupComponent,
                                     public juce::Value::Listener
    {
    public:
        // =====================================================================
        UnsafeOutputWarningPanel();

        // =====================================================================
        void valueChanged(juce::Value& value) override;
        void resized() override;

    private:
        // =====================================================================
        juce::Label message;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UnsafeOutputWarningPanel);
    };

    // =========================================================================
    PlotsPanel(PolesAndZerosEQAudioProcessor&, juce::ApplicationProperties&);
    ~PlotsPanel();

    //==========================================================================
    virtual void sampleRateChangedCallback(double) override;
    void updateValues();

    //==========================================================================
    void buttonClicked(juce::Button*) override;
    void buttonStateChanged(juce::Button*) override;
    void parameterChanged(const juce::String&, float);
    void resized() override;
    void paint(juce::Graphics&) override;
    /** Mark panel as to be recomputed */
    void recomputePoints();

private:
    // =========================================================================
    UnsafeOutputWarningPanel uowPanel;
    std::unique_ptr<ApplicationPropertiesButtonAttachment>
        linLogFreqAPAttachment, linLogAmpAPAttachment;
    std::shared_ptr<juce::ToggleButton> linLogFreqButton, linLogAmpButton;
    PolesAndZerosEQAudioProcessor& processor;
    PlotComponent mPlot, pPlot;
    juce::Label mLabel, pLabel;
    bool db;
    bool shouldRecomputePoints;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlotsPanel);
};
