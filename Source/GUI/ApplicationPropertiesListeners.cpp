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
ApplicationPropertiesButtonAttachment::ApplicationPropertiesButtonAttachment(
    juce::ApplicationProperties& properties, const juce::String& pID,
    std::shared_ptr<juce::Button> btn)
    : applicationProperties(properties)
    , propertyID(pID)
    , button(btn)
    , buttonListener(pID, properties)
{
    button->addListener(&buttonListener);
}
ApplicationPropertiesButtonAttachment::~ApplicationPropertiesButtonAttachment()
{
    button->removeListener(&buttonListener);
}
