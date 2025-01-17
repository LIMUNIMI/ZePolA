#include "DraggableLabel.h"
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
    setText(juce::String(f, numberOfDecimalPlaces), nt);
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
                * std::powf(std::fabs(delta), deltaGamma);

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
    auto nr = param->getNormalisableRange();
    float f = nr.convertFrom0to1(
        nr.convertTo0to1(label->getText().getFloatValue() / scale));
    Parameters::setParameterValue(param, f);
    setTextFromFloatDynamicCast(label, f * scale,
                                juce::NotificationType::dontSendNotification);
}

// =============================================================================
void DraggableLabelListener::setScale(float f) { scale = f; }

// =============================================================================
DraggableLabelParameterListener::DraggableLabelParameterListener(juce::Label& l)
    : label(l), scale(1.0f)
{
}

// =============================================================================
void DraggableLabelParameterListener::parameterChanged(const juce::String&,
                                                       float f)
{
    setTextFromFloatDynamicCast(&label, f * scale,
                                juce::NotificationType::dontSendNotification);
}

// =============================================================================
void DraggableLabelParameterListener::setScale(float f) { scale = f; }

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
