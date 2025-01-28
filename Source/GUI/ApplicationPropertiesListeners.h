#pragma once
#include <JuceHeader.h>

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

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertiesButtonAttachment)
};
