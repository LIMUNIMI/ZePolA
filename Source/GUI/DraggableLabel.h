#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Draggable label  */
class DraggableLabel : public juce::Label
{
public:
    // =========================================================================
    DraggableLabel(int numberOfDecimalPlaces = 3);
    /** Set text from a float value */
    void setTextFromFloat(float, juce::NotificationType);

private:
    //==========================================================================
    int numberOfDecimalPlaces;
};

// =============================================================================
/** Draggable label listener */
class DraggableLabelListener : public juce::Label::Listener
{
public:
    // =========================================================================
    DraggableLabelListener(juce::RangedAudioParameter*);
    void labelTextChanged(juce::Label*) override;

private:
    //==========================================================================
    juce::RangedAudioParameter* param;
};

// =============================================================================
/** Parameter listener for draggable label */
class DraggableLabelParameterListener
    : public juce::AudioProcessorValueTreeState::Listener
{
public:
    // =========================================================================
    DraggableLabelParameterListener(juce::Label&);
    void parameterChanged(const juce::String&, float) override;

private:
    //==========================================================================
    juce::Label& label;
};

// =============================================================================
/** Draggable label attachment */
class DraggableLabelAttachment
{
public:
    // =========================================================================
    DraggableLabelAttachment(juce::AudioProcessorValueTreeState& stateToUse,
                             const juce::String& parameterID,
                             juce::Label& label);
    ~DraggableLabelAttachment();

private:
    //==========================================================================
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::String parameterID;
    juce::Label& label;
    DraggableLabelParameterListener paramListener;
    DraggableLabelListener labelListener;
};
