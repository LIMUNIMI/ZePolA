/*
  ==============================================================================

    DraggableLabel.h

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

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableLabel)
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

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableLabelListener)
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
    float scale, oldParam;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        DraggableLabelParameterListener)
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

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DraggableLabelAttachment)
};
