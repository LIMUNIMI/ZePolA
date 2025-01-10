#include "DraggableLabel.h"
#include "../Parameters.h"

// =============================================================================
DraggableLabel::DraggableLabel(int ndp) : numberOfDecimalPlaces(ndp)
{
    setJustificationType(juce::Justification::centred);
    setEditable(true, true, false);
}
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
}
DraggableLabelAttachment::~DraggableLabelAttachment()
{
    label.removeListener(&labelListener);
    valueTreeState.removeParameterListener(parameterID, &paramListener);
}
