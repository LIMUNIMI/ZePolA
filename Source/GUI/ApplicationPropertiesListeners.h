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
