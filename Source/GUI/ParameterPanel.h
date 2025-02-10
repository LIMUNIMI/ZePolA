#pragma once
#include "../DSP/Filter.h"
#include "../Parameters.h"
#include "../PluginProcessor.h"
#include "DraggableLabel.h"
#include "InvisibleGroupComponent.h"
#include <JuceHeader.h>

// =============================================================================
/** Parameter slider for parameter strip */
class ParameterSlider : public juce::Slider
{
public:
    // =========================================================================
    ParameterSlider();

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSlider)
};

// =============================================================================
/** Single parameter row  */
class ParameterStrip : public InvisibleGroupComponent
{
public:
    // =========================================================================
    class ParentRepaintButtonListener : public juce::Button::Listener
    {
    public:
        // =====================================================================
        ParentRepaintButtonListener();

        // =====================================================================
        virtual void buttonClicked(juce::Button*) override;
        virtual void buttonStateChanged(juce::Button*) override;

    private:
        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            ParentRepaintButtonListener)
    };

    // =========================================================================
    class FrequencyLabelSampleRateListener : public SampleRateListener
    {
    public:
        // =====================================================================
        FrequencyLabelSampleRateListener(DraggableLabelAttachment&);

        // =====================================================================
        virtual void sampleRateChangedCallback(double) override;

    private:
        // =====================================================================
        DraggableLabelAttachment& dla;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            FrequencyLabelSampleRateListener)
    };

    // =========================================================================
    ParameterStrip(VTSAudioProcessor&, int index);
    ~ParameterStrip();

    //==========================================================================
    void resized() override;

    //==========================================================================
    /** Return true if the active button is down */
    bool isActive();
    /** Return true if the parent component is an active ParameterStrip or not a
     * ParameterStrip */
    static bool parentComponentIsActive(const juce::Component&);

private:
    // =========================================================================
    ParameterSlider mSlider, pSlider;
    juce::ToggleButton aButton, tButton;
    DraggableLabel gLabel, fLabel;
    VTSAudioProcessor& processor;

    // =========================================================================
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        mSliderAttachment, pSliderAttachment, gSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        aButtonAttachment, tButtonAttachment;
    std::unique_ptr<DraggableLabelAttachment> gLabelAttachment,
        fLabelAttachment;
    std::unique_ptr<FrequencyLabelSampleRateListener> srListener;

    ParentRepaintButtonListener aButtonListener;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterStrip)
};

// =============================================================================
/** Gaussian plane draggable point  */
class ZPoint : public juce::Component
{
public:
    // =========================================================================
    class LookAndFeelMethods
    {
    public:
        // =====================================================================
        virtual void drawZPoint(juce::Graphics&, float x, float y, float width,
                                float height, float p_x, float p_y,
                                FilterElement::Type, ZPoint&)
            = 0;
    };

    // =========================================================================
    class MagnitudeListener
        : public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        // =====================================================================
        MagnitudeListener(ZPoint* parent);

        // =====================================================================
        void parameterChanged(const juce::String&, float) override;

    private:
        // =====================================================================
        ZPoint* parent;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MagnitudeListener)
    };

    // =========================================================================
    class ArgListener : public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        // =====================================================================
        ArgListener(ZPoint* parent);

        // =====================================================================
        void parameterChanged(const juce::String&, float) override;

    private:
        // =====================================================================
        ZPoint* parent;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArgListener)
    };

    // =========================================================================
    class ActiveListener : public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        // =====================================================================
        ActiveListener(ZPoint* parent);

        // =====================================================================
        void parameterChanged(const juce::String&, float) override;

    private:
        // =====================================================================
        ZPoint* parent;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ActiveListener)
    };

    // =========================================================================
    class TypeListener : public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        // =====================================================================
        TypeListener(ZPoint* parent);

        // =====================================================================
        void parameterChanged(const juce::String&, float) override;

    private:
        // =====================================================================
        ZPoint* parent;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TypeListener)
    };

    // =========================================================================
    class MultiAttachment
    {
    public:
        // =====================================================================
        MultiAttachment(VTSAudioProcessor&, ZPoint*, int idx);
        ~MultiAttachment();

    private:
        // =====================================================================
        MagnitudeListener m_listen;
        ArgListener a_listen;
        ActiveListener v_listen;
        TypeListener t_listen;
        VTSAudioProcessor& processor;
        int idx;

        // =====================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiAttachment)
    };

    // =========================================================================
    ZPoint();

    // =========================================================================
    /** Set the point cartesian coordinates in the Gaussian plane */
    void setPointXY(float x, float y);
    /** Set the point X coordinate in the Gaussian plane (i.e. the real part) */
    void setPointX(float);
    /** Set the point Y coordinate in the Gaussian plane (i.e. the imaginary
     * part) */
    void setPointY(float);
    /** Set the point distance from the origin of the Gaussian plane (i.e. the
     * magnitude) */
    void setPointMagnitude(float);
    /** Set the point angle in radians in the Gaussian plane (i.e. the argument)
     */
    void setPointArg(float);
    /** Get the point X coordinate in the Gaussian plane (i.e. the real part) */
    float getPointX() const;
    /** Get the point Y coordinate in the Gaussian plane (i.e. the imaginary
     * part) */
    float getPointY() const;
    /** Get the point distance from the origin of the Gaussian plane (i.e. the
     * magnitude) */
    float getPointMagnitude() const;
    /** Get the point angle in radians in the Gaussian plane (i.e. the argument)
     */
    float getPointArg() const;

    /** Set the point type */
    void setType(FilterElement::Type);
    /** Get the point type */
    FilterElement::Type getType() const;

    // =========================================================================
    /** Set the point bounds based on its coordinates and the plane bounds */
    void setBoundsRelativeToPlane(juce::Component*, float radius);
    /** Set the point bounds based on its coordinates and the plane bounds */
    void setBoundsRelativeToPlane(juce::Component*);
    /** Set the point bounds based on its coordinates and the parent bounds */
    void setBoundsRelativeToParent();

    // =========================================================================
    void paint(juce::Graphics&) override;

private:
    // =========================================================================
    FilterElement::Type type;
    float r, a;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZPoint)
};

// =============================================================================
/** Gaussian plane with draggable points  */
class GaussianPlanePanel : public juce::Component
{
public:
    // =========================================================================
    class LookAndFeelMethods
    {
    public:
        // =====================================================================
        virtual void drawGaussianPlane(juce::Graphics&, float x, float y,
                                       float width, float height, float radius,
                                       GaussianPlanePanel&)
            = 0;
    };

    // =========================================================================
    GaussianPlanePanel(PolesAndZerosEQAudioProcessor&);

    // =========================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // =========================================================================
    /** Set the maximum represented value inside the plane. If 1, the unit
     * circle will touch the border of the panel. If bigger, it will be inside
     * the panel. */
    void setRadius(float);
    /** Get the maximum represented value inside the plane. */
    float getRadius() const;

private:
    // =========================================================================
    std::vector<std::unique_ptr<ZPoint::MultiAttachment>> point_attachments;
    std::vector<std::unique_ptr<ZPoint>> points;
    float radius;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GaussianPlanePanel)
};

// =============================================================================
/** Parameter control panel  */
class ParameterPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    ParameterPanel(PolesAndZerosEQAudioProcessor&, size_t);

    //==========================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    // =========================================================================
    std::vector<std::unique_ptr<ParameterStrip>> strips;
    std::vector<std::unique_ptr<juce::Label>> headerLabels;
    GaussianPlanePanel zplane;
    juce::GroupComponent shortcutsPanel;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterPanel)
};

// =============================================================================
/** Look and Feel methods for parameter control panel  */
class ParameterPanelLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void drawParameterStripSeparators(juce::Graphics&, float x,
                                              std::vector<float> y, float width,
                                              ParameterPanel&)
        = 0;
};
