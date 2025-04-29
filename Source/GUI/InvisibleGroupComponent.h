/*
  ==============================================================================

    InvisibleGroupComponent.h

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
/** Invisible GroupComponent  */
class InvisibleGroupComponent : public juce::GroupComponent
{
public:
    // =========================================================================
    InvisibleGroupComponent();

    //==========================================================================
    void paint(juce::Graphics& g) override;

private:
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InvisibleGroupComponent)
};

// =============================================================================
/** Look an feel for invisible GroupComponents  */
class InvisibleGroupComponentLookAndFeelMethods
{
public:
    virtual void dontDrawGroupComponent(juce::Graphics&, int width, int height,
                                        const juce::String& text,
                                        const juce::Justification&,
                                        juce::GroupComponent&)
        = 0;
};

// =============================================================================
/** Component separator */
class SeparatorComponent : public juce::Component
{
public:
    // =========================================================================
    class LookAndFeelMethods
    {
    public:
        virtual void drawSeparator(juce::Graphics&, int x, int y, int width,
                                   int height, bool drawTop, bool drawBottom,
                                   bool drawLeft, bool drawRight,
                                   SeparatorComponent&)
            = 0;
    };

    // =========================================================================
    SeparatorComponent();

    //==========================================================================
    void paint(juce::Graphics& g) override;

    //==========================================================================
    bool drawTop, drawBottom, drawLeft, drawRight;

private:
    //==========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeparatorComponent)
};
