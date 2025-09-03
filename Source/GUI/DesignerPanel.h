/*
  ==============================================================================

    DesignerPanel.h

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

#pragma once
#include "../DSP/FilterDesign.h"
#include "../PluginProcessor.h"
#include "ApplicationPropertiesListeners.h"
#include "CustomButtons.h"
#include <JuceHeader.h>

// =============================================================================
/** Filter design panel  */
class DesignerPanel : public juce::GroupComponent, public SampleRateListener
{
public:
    // =========================================================================
    class CBoxListener : public juce::ComboBox::Listener
    {
    public:
        //======================================================================
        CBoxListener(std::function<void(int)>);

        //======================================================================
        virtual void comboBoxChanged(juce::ComboBox*) override;

    private:
        //======================================================================
        std::function<void(int)> callback;

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CBoxListener)
    };

    // =========================================================================
    class SliderListener : public juce::Slider::Listener
    {
    public:
        //======================================================================
        SliderListener(std::function<void(double)>);

        //======================================================================
        virtual void sliderValueChanged(juce::Slider*) override;

    private:
        //======================================================================
        std::function<void(double)> callback;

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderListener)
    };

    // =========================================================================
    class ButtonListener : public juce::Button::Listener
    {
    public:
        //======================================================================
        ButtonListener(std::function<void(bool)>);

        //======================================================================
        virtual void buttonClicked(juce::Button*) override;
        virtual void buttonStateChanged(juce::Button*) override;

    private:
        //======================================================================
        std::function<void(bool)> callback;

        //======================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonListener)
    };

    // =========================================================================
    DesignerPanel(ZePolAudioProcessor&, juce::ApplicationProperties&);
    ~DesignerPanel();

    // =========================================================================
    /** Set filter type from ComboBox Id (+1 offset) */
    void setTypeFromCBoxId(int);
    /** Set filter shape from ComboBox Id (+1 offset) */
    void setShapeFromCBoxId(int);
    /** Set filter order */
    void setOrder(double);
    /** Set filter cutoff frequency */
    void setCutoff(double);
    /** Set the ripple amount (dB) in the passband */
    void setPassbandRipple(double);
    /** Set the ripple amount (dB) in the stopband */
    void setStopbandRipple(double);
    /** Set the designer in automatic (or manual) mode */
    void setAuto(bool);

    // =========================================================================
    virtual void sampleRateChangedCallback(double) override;
    void resized() override;

private:
    // =========================================================================
    /** Update visibility of passband ripple slider */
    void updatePassbandRippleVisibility();
    /** Update visibility of stopband ripple slider */
    void updateStopbandRippleVisibility();

    // =========================================================================
    void autoDesignFilter();
    void designFilter();
    void applyFilterElement(int i, std::complex<double>, bool type,
                            double gain);

    // =========================================================================
    std::unique_ptr<ApplicationPropertiesButtonAttachment> autoButtonAttachment;
    std::unique_ptr<ApplicationPropertiesComboBoxAttachment> typeCBoxAttachment,
        shapeCBoxAttachment;
    std::unique_ptr<ApplicationPropertiesSliderAttachment>
        orderSliderAttachment, cutoffSliderAttachment, rpSliderAttachment,
        rsSliderAttachment;
    CBoxListener typeCBoxListener, shapeCBoxListener;
    SliderListener orderSliderListener, cutoffSliderListener, rpSliderListener,
        rsSliderListener;
    ButtonListener autoButtonListener;
    ZePolAudioProcessor& processor;
    juce::Label panelLabel, orderLabel, cutoffLabel, rpLabel, rsLabel;
    std::shared_ptr<juce::ComboBox> typeCBox, shapeCBox;
    std::shared_ptr<juce::Slider> orderSlider, cutoffSlider, rpSlider, rsSlider;
    std::shared_ptr<juce::ToggleButton> autoButton;
    juce::TextButton applyButton;
    FilterParameters filterParams;
    bool autoUpdate;
    juce::ApplicationProperties& applicationProperties;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
