/*
  ==============================================================================

    CustomButtons.h

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
/** Get the appropriate button label */
juce::String Button_getOnOffLabel(const juce::Button&, bool state);
/** Get the appropriate button label */
juce::String Button_getOnOffLabel(const juce::Button&);
/** Set the button label for both states */
void Button_setOnOffLabel(juce::Button&, const juce::String& labelOff,
                          const juce::String& labelOn);

// =============================================================================
/** Checkbox class */
class ToggleButtonCheckbox : public juce::ToggleButton
{
public:
    // =========================================================================
    ToggleButtonCheckbox();

private:
    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButtonCheckbox)
};

// =============================================================================
/** Toggle button with labels */
class LabelledToggleButton : public juce::ToggleButton
{
public:
    // =========================================================================
    LabelledToggleButton(const juce::String& label0, const juce::String& label1,
                         int colourID0, int colourID1, bool ledPosition0,
                         bool ledPosition1);

    // =========================================================================
    const juce::String& getCurrentLabel() const;
    int getCurrentColourID() const;
    bool getCurrentLedPosition() const;

    // =========================================================================
    void paintButton(juce::Graphics&, bool, bool) override;

private:
    // =========================================================================
    juce::String label0, label1;
    int colourID0, colourID1;
    bool ledPosition0, ledPosition1;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabelledToggleButton)
};

// =============================================================================
/** Look and feel methods for LabelledToggleButton */
class LabelledToggleButtonLookAndFeelMethods
{
public:
    // =========================================================================
    virtual void drawLabelledToggleButton(juce::Graphics& g,
                                          LabelledToggleButton& button,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
        = 0;
};

// =============================================================================
class TextAndImageButton : public juce::Button
{
public:
    // =============================================================================
    TextAndImageButton(const juce::String&, juce::Drawable*,
                       const juce::Justification&);

    // =============================================================================
    void resized() override;

protected:
    // =========================================================================
    void paintButton(juce::Graphics&, bool shouldDrawButtonAsHighlighted,
                     bool shouldDrawButtonAsDown) override;

private:
    // =========================================================================
    std::unique_ptr<juce::Drawable> image;
    juce::Label label;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextAndImageButton)
};
