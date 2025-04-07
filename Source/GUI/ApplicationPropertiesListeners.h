/*
  ==============================================================================

    ApplicationPropertiesListeners.h

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
/** Value listener for an application property */
class ApplicationPropertyValueListener : public juce::Value::Listener
{
public:
    // =========================================================================
    ApplicationPropertyValueListener(const juce::String& propertyID,
                                     juce::ApplicationProperties& properties);

    // =========================================================================
    void valueChanged(juce::Value&) override;

private:
    // =========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertyValueListener)
};

// =============================================================================
/** Application property listener for a Value */
class ValueApplicationPropertyListener : public juce::ChangeListener
{
public:
    // =========================================================================
    enum ValueType
    {
        UNSET = 0,
        STRING,
        INT,
        DOUBLE,
        BOOL
    };

    // =========================================================================
    ValueApplicationPropertyListener(const juce::String& propertyID,
                                     std::shared_ptr<juce::Value>,
                                     ValueType t = ValueType::UNSET);

    // =========================================================================
    /** Set the value type */
    void setValueType(ValueType);

    // =========================================================================
    virtual void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    // =========================================================================
    std::shared_ptr<juce::Value> value;
    juce::String propertyID;
    ValueType type;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ValueApplicationPropertyListener)
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

protected:
    //==========================================================================
    juce::ApplicationProperties& applicationProperties;
    juce::String propertyID;
    std::shared_ptr<ComponentType> component;
    ComponentListenerType componentListener;
    ApplicationPropertyListenerType propertyListener;

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertiesComponentAttachment)
};

// =============================================================================
/** Attachment for a Value and an application property */
class ApplicationPropertiesValueAttachment
    : public ApplicationPropertiesComponentAttachment<
          juce::Value, ApplicationPropertyValueListener,
          ValueApplicationPropertyListener>
{
public:
    // =========================================================================
    ApplicationPropertiesValueAttachment(
        juce::ApplicationProperties& properties, const juce::String& propertyID,
        std::shared_ptr<juce::Value>,
        ValueApplicationPropertyListener::ValueType t
        = ValueApplicationPropertyListener::ValueType::STRING);

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        ApplicationPropertiesValueAttachment)
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
