/*
  ==============================================================================

    ApplicationPropertiesListeners.cpp

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

#include "ApplicationPropertiesListeners.h"
#include "../Macros.h"

// =============================================================================
ApplicationPropertyButtonListener::ApplicationPropertyButtonListener(
    const juce::String& pID, juce::ApplicationProperties& properties)
    : applicationProperties(properties), propertyID(pID)
{
}
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
    : button(btn), propertyID(pID)
{
}
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
    : applicationProperties(properties), propertyID(pID)
{
}
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
    : comboBox(cb), propertyID(pID)
{
}
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
    : applicationProperties(properties), propertyID(pID)
{
}
void ApplicationPropertySliderListener::sliderValueChanged(juce::Slider* slider)
{
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue(propertyID, slider->getValue());
}

// =============================================================================
SliderApplicationPropertyListener::SliderApplicationPropertyListener(
    const juce::String& pID, std::shared_ptr<juce::Slider> s)
    : slider(s), propertyID(pID)
{
}
void SliderApplicationPropertyListener::changeListenerCallback(
    juce::ChangeBroadcaster* source)
{
    if (auto pf = dynamic_cast<juce::PropertiesFile*>(source))
        slider->setValue(std::clamp(pf->getDoubleValue(propertyID, false),
                                    slider->getMinimum(), slider->getMaximum()),
                         juce::NotificationType::sendNotification);
}

// =============================================================================
ApplicationPropertyValueListener::ApplicationPropertyValueListener(
    const juce::String& pID, juce::ApplicationProperties& properties)
    : applicationProperties(properties), propertyID(pID)
{
}
void ApplicationPropertyValueListener::valueChanged(juce::Value& v)
{
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        pf->setValue(propertyID, v);
}

// =============================================================================
ValueApplicationPropertyListener::ValueApplicationPropertyListener(
    const juce::String& pID, std::shared_ptr<juce::Value> v,
    ValueApplicationPropertyListener::ValueType t)
    : value(v), propertyID(pID), type(t)
{
}

void ValueApplicationPropertyListener::setValueType(ValueType t) { type = t; }
void ValueApplicationPropertyListener::changeListenerCallback(
    juce::ChangeBroadcaster* source)
{
    if (auto pf = dynamic_cast<juce::PropertiesFile*>(source)) switch (type)
        {
        default:
            UNHANDLED_SWITCH_CASE("Unhandled case for property value type");
        case ValueApplicationPropertyListener::ValueType::UNSET: break;
        case ValueApplicationPropertyListener::ValueType::STRING:
            value->setValue(pf->getValue(propertyID));
            break;
        case ValueApplicationPropertyListener::ValueType::INT:
            value->setValue(pf->getIntValue(propertyID));
            break;
        case ValueApplicationPropertyListener::ValueType::DOUBLE:
            value->setValue(pf->getDoubleValue(propertyID));
            break;
        case ValueApplicationPropertyListener::ValueType::BOOL:
            value->setValue(pf->getBoolValue(propertyID));
            break;
        }
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
ApplicationPropertiesValueAttachment::ApplicationPropertiesValueAttachment(
    juce::ApplicationProperties& properties, const juce::String& pID,
    std::shared_ptr<juce::Value> v,
    ValueApplicationPropertyListener::ValueType t)
    : ApplicationPropertiesComponentAttachment<
          juce::Value, ApplicationPropertyValueListener,
          ValueApplicationPropertyListener>(properties, pID, v)
{
    propertyListener.setValueType(t);
    if (juce::PropertiesFile* pf
        = applicationProperties.getCommonSettings(true))
        propertyListener.changeListenerCallback(pf);
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
template class ApplicationPropertiesComponentAttachment<
    juce::Value, ApplicationPropertyValueListener,
    ValueApplicationPropertyListener>;
