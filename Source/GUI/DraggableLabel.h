#pragma once
#include <JuceHeader.h>

// =============================================================================
/** Draggable label  */
class DraggableLabel : public juce::Label
{
public:
    // =========================================================================
    DraggableLabel(float defaultValue = 0.0f, int numberOfDecimalPlaces = 3);

    // =========================================================================
    /** Set text from a float value */
    void setTextFromFloat(float, juce::NotificationType);

    // =========================================================================
    void mouseEnter(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseDrag(const juce::MouseEvent&) override;
    void mouseDoubleClick(const juce::MouseEvent&) override;

private:
    //==========================================================================
    int numberOfDecimalPlaces, eventStartY;
    float eventStartValue, deltaScale, deltaGamma, defaultValue;
};

// =============================================================================
/** Draggable label listener */
class DraggableLabelListener : public juce::Label::Listener
{
public:
    // =========================================================================
    DraggableLabelListener(juce::RangedAudioParameter*);
    void labelTextChanged(juce::Label*) override;

    // =========================================================================
    /** Set the conversion scale between parameter value and text */
    void setScale(float);

private:
    //==========================================================================
    juce::RangedAudioParameter* param;
    float scale;
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

    // =========================================================================
    /** Set the conversion scale between parameter value and text */
    void setScale(float);

private:
    //==========================================================================
    juce::Label& label;
    float scale;
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

    // =========================================================================
    /** Set the conversion scale between parameter value and text */
    void setScale(float);

private:
    //==========================================================================
    juce::AudioProcessorValueTreeState& valueTreeState;
    juce::String parameterID;
    juce::Label& label;
    DraggableLabelParameterListener paramListener;
    DraggableLabelListener labelListener;
};
