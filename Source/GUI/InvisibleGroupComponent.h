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
