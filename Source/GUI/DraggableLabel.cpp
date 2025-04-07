/*
  ==============================================================================

    DraggableLabel.cpp

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

#include "DraggableLabel.h"
#include "../Macros.h"
#include "../Parameters.h"

// =============================================================================
DraggableLabel::DraggableLabel(float v, int ndp)
    : numberOfDecimalPlaces(ndp)
    , eventStartY(0)
    , eventStartValue(0.0f)
    , deltaScale(0.01f)
    , deltaGamma(1.75f)
    , defaultValue(v)
{
    setJustificationType(juce::Justification::centred);
    setEditable(true, true, false);
}

// =============================================================================
void DraggableLabel::setTextFromFloat(float f, juce::NotificationType nt)
{
    SAFE_MessageManager_LOCK(
        this, setText(juce::String(f, numberOfDecimalPlaces), nt););
}
static void setTextFromFloatDynamicCast(juce::Label* label, float f,
                                        juce::NotificationType nt)
{
    if (auto dl = dynamic_cast<DraggableLabel*>(label))
    {
        dl->setTextFromFloat(f, nt);
    }
    else
    {
        label->setText(juce::String(f), nt);
    }
}

// =============================================================================
void DraggableLabel::mouseEnter(const juce::MouseEvent&)
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}
void DraggableLabel::mouseExit(const juce::MouseEvent&)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
}
void DraggableLabel::mouseDown(const juce::MouseEvent& event)
{
    if (isEnabled())
    {
        if (event.mods.isCommandDown() || event.mods.isMiddleButtonDown())
        {
            setTextFromFloat(defaultValue,
                             juce::NotificationType::sendNotification);
        }
        eventStartY     = event.getPosition().getY();
        eventStartValue = static_cast<float>(getText().getDoubleValue());
    }
}
void DraggableLabel::mouseDrag(const juce::MouseEvent& event)
{
    if (isEnabled())
    {
        float delta
            = static_cast<float>(eventStartY - event.getPosition().getY());
        delta = ((delta < 0) ? -deltaScale : deltaScale)
                * std::pow(std::fabs(delta), deltaGamma);

        setTextFromFloat(eventStartValue + delta,
                         juce::NotificationType::sendNotification);
    }
}
void DraggableLabel::mouseDoubleClick(const juce::MouseEvent&)
{
    setTextFromFloat(defaultValue, juce::NotificationType::sendNotification);
}

// =============================================================================
DraggableLabelListener::DraggableLabelListener(juce::RangedAudioParameter* p)
    : param(p), scale(1.0f)
{
}
void DraggableLabelListener::labelTextChanged(juce::Label* label)
{
    float f = param->getNormalisableRange().snapToLegalValue(
        label->getText().getFloatValue() / scale);
    Parameters::setParameterValue(param, f);
    setTextFromFloatDynamicCast(label, f * scale,
                                juce::NotificationType::dontSendNotification);
}

// =============================================================================
void DraggableLabelListener::setScale(float f) { scale = f; }

// =============================================================================
DraggableLabelParameterListener::DraggableLabelParameterListener(juce::Label& l)
    : label(l), scale(1.0f), oldParam(0.0f)
{
}

// =============================================================================
void DraggableLabelParameterListener::parameterChanged(const juce::String&,
                                                       float f)
{
    oldParam = f;
    setTextFromFloatDynamicCast(&label, f * scale,
                                juce::NotificationType::dontSendNotification);
}

// =============================================================================
void DraggableLabelParameterListener::setScale(float f)
{
    scale = f;
    parameterChanged("", oldParam);
}

// =============================================================================
DraggableLabelAttachment::DraggableLabelAttachment(
    juce::AudioProcessorValueTreeState& stateToUse, const juce::String& pID,
    juce::Label& lbl)
    : valueTreeState(stateToUse)
    , parameterID(pID)
    , label(lbl)
    , paramListener(label)
    , labelListener(stateToUse.getParameter(parameterID))
{
    label.addListener(&labelListener);
    valueTreeState.addParameterListener(parameterID, &paramListener);
    paramListener.parameterChanged(
        parameterID,
        valueTreeState.getParameterRange(parameterID)
            .convertFrom0to1(
                valueTreeState.getParameter(parameterID)->getValue()));
}
DraggableLabelAttachment::~DraggableLabelAttachment()
{
    label.removeListener(&labelListener);
    valueTreeState.removeParameterListener(parameterID, &paramListener);
}

// =============================================================================
void DraggableLabelAttachment::setScale(float f)
{
    labelListener.setScale(f);
    paramListener.setScale(f);
}
