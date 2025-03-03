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
void ApplicationPropertyButtonListener::buttonStateChanged(
    juce::Button* /* b */)
{
}

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
template <typename ComponentType, typename ComponentListenerType,
          typename ApplicationPropertyListenerType>
ApplicationPropertiesComponentAttachment<ComponentType, ComponentListenerType,
                                         ApplicationPropertyListenerType>::
    ApplicationPropertiesComponentAttachment(
        juce::ApplicationProperties& properties, const juce::String& pID,
        std::shared_ptr<ComponentType> comp)
    : applicationProperties(properties)
    , propertyID(pID)
    , component(comp)
    , componentListener(pID, properties)
    , propertyListener(pID, comp)
{
    component->addListener(&componentListener);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
    {
        pf->addChangeListener(&propertyListener);
        propertyListener.changeListenerCallback(pf);
    }
}
template <typename ComponentType, typename ComponentListenerType,
          typename ApplicationPropertyListenerType>
ApplicationPropertiesComponentAttachment<ComponentType, ComponentListenerType,
                                         ApplicationPropertyListenerType>::
    ~ApplicationPropertiesComponentAttachment()
{
    component->removeListener(&componentListener);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->removeChangeListener(&propertyListener);
}

// =============================================================================
template class ApplicationPropertiesComponentAttachment<
    juce::Button, ApplicationPropertyButtonListener,
    ButtonApplicationPropertyListener>;
