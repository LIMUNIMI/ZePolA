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
/** Attachment for a button and an application property */
class ApplicationPropertiesButtonAttachment
{
public:
    // =========================================================================
    ApplicationPropertiesButtonAttachment(
        juce::ApplicationProperties& properties, const juce::String& propertyID,
        std::shared_ptr<juce::Button> button);
    ~ApplicationPropertiesButtonAttachment();

private:
    //==========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;
    std::shared_ptr<juce::Button> button;
    ApplicationPropertyButtonListener buttonListener;
    ButtonApplicationPropertyListener propertyListener;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertiesButtonAttachment)
};
