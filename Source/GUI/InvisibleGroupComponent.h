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
