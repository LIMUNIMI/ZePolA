#include "DraggableLabel.h"
#include "../Parameters.h"

// =============================================================================
DraggableLabel::DraggableLabel(int ndp)
    : numberOfDecimalPlaces(ndp)
    , eventStartY(0)
    , eventStartValue(0.0f)
    , deltaScale(0.01f)
    , deltaGamma(1.75f)
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
        eventStartY     = event.getPosition().getY();
        eventStartValue = getText().getDoubleValue();
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

// =============================================================================
DraggableLabelListener::DraggableLabelListener(juce::RangedAudioParameter* p)
    : param(p)
{
}
void DraggableLabelListener::labelTextChanged(juce::Label* label)
{
    auto nr    = param->getNormalisableRange();
    float gain = nr.convertFrom0to1(
        nr.convertTo0to1(label->getText().getFloatValue()));
    Parameters::setParameterValue(param, gain);
    setTextFromFloatDynamicCast(label, gain,
                                juce::NotificationType::dontSendNotification);
}

// =============================================================================
DraggableLabelParameterListener::DraggableLabelParameterListener(juce::Label& l)
    : label(l)
{
}

// =============================================================================
void DraggableLabelParameterListener::parameterChanged(
    const juce::String& parameterID, float f)
{
    setTextFromFloatDynamicCast(&label, f,
                                juce::NotificationType::dontSendNotification);
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
