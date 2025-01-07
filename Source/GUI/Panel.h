#pragma once
#include "Settings.h"
#include <JuceHeader.h>

// =============================================================================
/** Rectangular panel for the GUI  */
class Panel : public juce::Component
{
public:
    // ===========================================================================
    Panel();

    //===================================================================
    void paint(juce::Graphics&) override;

private:
    // ===========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Panel);
};
