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
ApplicationPropertyComboBoxListener::ApplicationPropertyComboBoxListener(
    const juce::String& pID, juce::ApplicationProperties& properties)
    : propertyID(pID), applicationProperties(properties)
{
}

// =============================================================================
void ApplicationPropertyComboBoxListener::comboBoxChanged(
    juce::ComboBox* comboBoxThatHasChanged)
{
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue(propertyID, comboBoxThatHasChanged->getSelectedId());
}

// =============================================================================
ComboBoxApplicationPropertyListener::ComboBoxApplicationPropertyListener(
    const juce::String& pID, std::shared_ptr<juce::ComboBox> cb)
    : propertyID(pID), comboBox(cb)
{
}

// =============================================================================
void ComboBoxApplicationPropertyListener::changeListenerCallback(
    juce::ChangeBroadcaster* source)
{
    if (auto pf = dynamic_cast<juce::PropertiesFile*>(source))
        comboBox->setSelectedId(pf->getIntValue(propertyID, false),
                                juce::NotificationType::sendNotification);
}

// =============================================================================
ApplicationPropertySliderListener::ApplicationPropertySliderListener(
    const juce::String& pID, juce::ApplicationProperties& properties)
    : propertyID(pID), applicationProperties(properties)
{
}

// =============================================================================
void ApplicationPropertySliderListener::sliderValueChanged(juce::Slider* slider)
{
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue(propertyID, slider->getValue());
}

// =============================================================================
SliderApplicationPropertyListener::SliderApplicationPropertyListener(
    const juce::String& pID, std::shared_ptr<juce::Slider> s)
    : propertyID(pID), slider(s)
{
}

// =============================================================================
void SliderApplicationPropertyListener::changeListenerCallback(
    juce::ChangeBroadcaster* source)
{
    if (auto pf = dynamic_cast<juce::PropertiesFile*>(source))
        slider->setValue(std::clamp(pf->getDoubleValue(propertyID, false),
                                    slider->getMinimum(), slider->getMaximum()),
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
        if (pf->containsKey(propertyID))
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
template class ApplicationPropertiesComponentAttachment<
    juce::ComboBox, ApplicationPropertyComboBoxListener,
    ComboBoxApplicationPropertyListener>;
template class ApplicationPropertiesComponentAttachment<
    juce::Slider, ApplicationPropertySliderListener,
    SliderApplicationPropertyListener>;
