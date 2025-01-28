#include "ApplicationPropertiesListeners.h"

// =============================================================================
ApplicationPropertiesButtonAttachment::ApplicationPropertiesButtonAttachment(
    juce::ApplicationProperties& properties, const juce::String& pID,
    std::shared_ptr<juce::Button> btn)
    : applicationProperties(properties), propertyID(pID), button(btn)
{
}
ApplicationPropertiesButtonAttachment::~ApplicationPropertiesButtonAttachment()
{
}
