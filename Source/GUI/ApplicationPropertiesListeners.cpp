#include "ApplicationPropertiesListeners.h"

// =============================================================================
ApplicationPropertyButtonListener::ApplicationPropertyButtonListener(
    const juce::String& pID, juce::ApplicationProperties& properties)
    : propertyID(pID), applicationProperties(properties)
{
}

// =============================================================================
void ApplicationPropertyButtonListener::buttonClicked(juce::Button* b)
{
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue(propertyID, b->getToggleState());
}
void ApplicationPropertyButtonListener::buttonStateChanged(juce::Button* b) {}

// =============================================================================
ButtonApplicationPropertyListener::ButtonApplicationPropertyListener(
    const juce::String& pID, std::shared_ptr<juce::Button> btn)
    : propertyID(pID), button(btn)
{
}

// =============================================================================
void ButtonApplicationPropertyListener::changeListenerCallback(
    juce::ChangeBroadcaster* source)
{
    if (auto pf = dynamic_cast<juce::PropertiesFile*>(source))
        button->setToggleState(pf->getBoolValue(propertyID, false),
                               juce::NotificationType::sendNotification);
}

// =============================================================================
ApplicationPropertiesButtonAttachment::ApplicationPropertiesButtonAttachment(
    juce::ApplicationProperties& properties, const juce::String& pID,
    std::shared_ptr<juce::Button> btn)
    : applicationProperties(properties)
    , propertyID(pID)
    , button(btn)
    , buttonListener(pID, properties)
    , propertyListener(pID, btn)
{
    button->addListener(&buttonListener);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
    {
        pf->addChangeListener(&propertyListener);
        propertyListener.changeListenerCallback(pf);
    }
}
ApplicationPropertiesButtonAttachment::~ApplicationPropertiesButtonAttachment()
{
    button->removeListener(&buttonListener);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->removeChangeListener(&propertyListener);
}
