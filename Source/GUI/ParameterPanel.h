#pragma once
#include "../DSP/Filter.h"
#include "../Parameters.h"
#include "../PluginProcessor.h"
#include "CustomButtons.h"
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
    juce::ToggleButton aButton;
    LabelledToggleButton tButton;
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
                                FilterElement::Type, bool conjugate, ZPoint&)
            = 0;
    };

    // =========================================================================
    /** Listener for updating the point magnitude */
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
    /** Listener for updating the point angle */
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
    /** Listener for updating the point visibility */
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
    /** Listener for updating the point type */
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

    // =============================================================================
    /** Listener for listening to dragging the point with the mouse */
    class DraggablePointListener : public juce::MouseListener
    {
    public:
        // =========================================================================
        DraggablePointListener(juce::RangedAudioParameter* r,
                               juce::RangedAudioParameter* a);
        void mouseDrag(const juce::MouseEvent&) override;

    private:
        //==========================================================================
        juce::RangedAudioParameter *r_param, *a_param;

        // =========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggablePointListener)
    };

    // =============================================================================
    /** Listener for listening to scrolling the point with the mouse */
    class ScrollablePointListener : public juce::MouseListener
    {
    public:
        // =========================================================================
        ScrollablePointListener(juce::RangedAudioParameter*);
        void mouseWheelMove(const juce::MouseEvent&,
                            const juce::MouseWheelDetails&) override;

        // =========================================================================
        void setDeltaGain(float);
        float getDeltaGain();

    private:
        //==========================================================================
        juce::RangedAudioParameter* param;
        float deltaGain;

        // =========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScrollablePointListener)
    };

    // =============================================================================
    /** Listener for toggling the type parameter when double-clicking the point
     * with the mouse */
    class TogglableTypePointListener : public juce::MouseListener
    {
    public:
        // =========================================================================
        TogglableTypePointListener(juce::RangedAudioParameter*);
        void mouseDoubleClick(const juce::MouseEvent&) override;

    private:
        //==========================================================================
        juce::RangedAudioParameter* param;

        // =========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TogglableTypePointListener)
    };

    // =============================================================================
    /** Listener for toggling the activation state parameter when right-clicking
     * the point with the mouse */
    class RickClickableActivePointListener : public juce::MouseListener
    {
    public:
        // =========================================================================
        RickClickableActivePointListener(juce::RangedAudioParameter*);
        void mouseDown(const juce::MouseEvent&) override;

    private:
        //==========================================================================
        juce::RangedAudioParameter* param;

        // =========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
            RickClickableActivePointListener)
    };

    // =========================================================================
    class MultiAttachment
    {
    public:
        // =====================================================================
        friend class GaussianPlanePanel;

        // =====================================================================
        MultiAttachment(VTSAudioProcessor&, ZPoint*, int idx);
        ~MultiAttachment();

    private:
        // =====================================================================
        VTSAudioProcessor& processor;
        ZPoint* point;
        juce::String m_id, a_id, v_id, t_id, g_id;
        MagnitudeListener m_listen;
        ArgListener a_listen;
        ActiveListener v_listen;
        TypeListener t_listen;
        DraggablePointListener z_p_listen;
        ScrollablePointListener g_p_listen;
        TogglableTypePointListener t_p_listen;
        RickClickableActivePointListener v_p_listen;

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
    /** Set the point as conjugate or not */
    void setConjugate(bool);
    /** Get the conjugate state of the point */
    bool getConjugate() const;

    // =========================================================================
    /** Set the point bounds based on its coordinates and the plane bounds */
    void setBoundsRelativeToPlane(juce::Component*, float radius);
    /** Set the point bounds based on its coordinates and the plane bounds */
    void setBoundsRelativeToPlane(juce::Component*);
    /** Set the point bounds based on its coordinates and the parent bounds */
    void setBoundsRelativeToParent();

    // =========================================================================
    void setConjugatePoint(ZPoint* z = nullptr);

    // =========================================================================
    virtual void setVisible(bool) override;
    void paint(juce::Graphics&) override;
    void mouseDrag(const juce::MouseEvent& event) override;

private:
    // =========================================================================
    FilterElement::Type type;
    float r, a;
    bool conjugate;
    ZPoint* z_conj;

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
    ~GaussianPlanePanel();

    // =========================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;

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
    std::vector<std::unique_ptr<ZPoint>> conj_points;
    float radius;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GaussianPlanePanel)
};

// =============================================================================
/** Shortcuts panel  */
class ShortcutsPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    ShortcutsPanel(PolesAndZerosEQAudioProcessor&);

    // =========================================================================
    void resized() override;

    // =========================================================================
    void triggerAllOn();
    void triggerAllOff();
    void triggerDoublePhases();
    void triggerHalfPhases();
    void triggerSwapTypes();

private:
    // =========================================================================
    PolesAndZerosEQAudioProcessor& processor;
    juce::TextButton allOnButton, allOffButton, doublePhaseButton,
        halfPhaseButton, swapTypeButton;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShortcutsPanel)
};

// =============================================================================
/** Parameter control panel  */
class ParameterPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    ParameterPanel(PolesAndZerosEQAudioProcessor&);

    //==========================================================================
    void paint(Graphics&) override;
    void resized() override;

private:
    // =========================================================================
    std::vector<std::unique_ptr<ParameterStrip>> strips;
    std::vector<std::unique_ptr<juce::Label>> headerLabels;
    juce::Label zplane_label;
    GaussianPlanePanel zplane;
    ShortcutsPanel shortcutsPanel;

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
