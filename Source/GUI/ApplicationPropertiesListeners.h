#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Button listener for an application property */
class ApplicationPropertyButtonListener : public juce::Button::Listener
{
public:
    // =========================================================================
    ApplicationPropertyButtonListener(const juce::String& propertyID,
                                      juce::ApplicationProperties& properties);

    //==========================================================================
    virtual void buttonClicked(juce::Button*) override;
    virtual void buttonStateChanged(juce::Button*) override;

private:
    // =========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertyButtonListener)
};

// =============================================================================
/** Application property listener for a button */
class ButtonApplicationPropertyListener : public juce::ChangeListener
{
public:
    // =========================================================================
    ButtonApplicationPropertyListener(const juce::String& propertyID,
                                      std::shared_ptr<juce::Button> button);

    // =========================================================================
    virtual void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    // =========================================================================
    std::shared_ptr<juce::Button> button;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ButtonApplicationPropertyListener)
};

// =============================================================================
/** ComboBox listener for an application property */
class ApplicationPropertyComboBoxListener : public juce::ComboBox::Listener
{
public:
    // =========================================================================
    ApplicationPropertyComboBoxListener(
        const juce::String& propertyID,
        juce::ApplicationProperties& properties);

    //==========================================================================
    virtual void comboBoxChanged(juce::ComboBox*) override;

private:
    // =========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertyComboBoxListener)
};

// =============================================================================
/** Slider listener for an application property */
class ApplicationPropertySliderListener : public juce::Slider::Listener
{
public:
    // =========================================================================
    ApplicationPropertySliderListener(const juce::String& propertyID,
                                      juce::ApplicationProperties& properties);

    //==========================================================================
    virtual void sliderValueChanged(juce::Slider*) override;

private:
    // =========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertySliderListener)
};

// =============================================================================
/** Application property listener for a Slider */
class SliderApplicationPropertyListener : public juce::ChangeListener
{
public:
    // =========================================================================
    SliderApplicationPropertyListener(const juce::String& propertyID,
                                      std::shared_ptr<juce::Slider>);

    // =========================================================================
    virtual void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    // =========================================================================
    std::shared_ptr<juce::Slider> slider;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        SliderApplicationPropertyListener)
};

// =============================================================================
/** Application property listener for a ComboBox */
class ComboBoxApplicationPropertyListener : public juce::ChangeListener
{
public:
    // =========================================================================
    ComboBoxApplicationPropertyListener(const juce::String& propertyID,
                                        std::shared_ptr<juce::ComboBox>);

    // =========================================================================
    virtual void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    // =========================================================================
    std::shared_ptr<juce::ComboBox> comboBox;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ComboBoxApplicationPropertyListener)
};

// =============================================================================
/** Template attachment for a component and an application property */
template <typename ComponentType, typename ComponentListenerType,
          typename ApplicationPropertyListenerType>
class ApplicationPropertiesComponentAttachment
{
public:
    // =========================================================================
    ApplicationPropertiesComponentAttachment(
        juce::ApplicationProperties& properties, const juce::String& propertyID,
        std::shared_ptr<ComponentType>);
    ~ApplicationPropertiesComponentAttachment();

private:
    //==========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;
    std::shared_ptr<ComponentType> component;
    ComponentListenerType componentListener;
    ApplicationPropertyListenerType propertyListener;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertiesComponentAttachment)
};

using ApplicationPropertiesButtonAttachment
    = ApplicationPropertiesComponentAttachment<
        juce::Button, ApplicationPropertyButtonListener,
        ButtonApplicationPropertyListener>;
using ApplicationPropertiesComboBoxAttachment
    = ApplicationPropertiesComponentAttachment<
        juce::ComboBox, ApplicationPropertyComboBoxListener,
        ComboBoxApplicationPropertyListener>;
using ApplicationPropertiesSliderAttachment
    = ApplicationPropertiesComponentAttachment<
        juce::Slider, ApplicationPropertySliderListener,
        SliderApplicationPropertyListener>;
