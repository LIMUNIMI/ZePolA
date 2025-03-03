#pragma once
#include "../Parameters.h"
#include <JuceHeader.h>

// =============================================================================
/** Filter design panel  */
class DesignerPanel : public juce::GroupComponent
{
public:
    // =========================================================================
    DesignerPanel(VTSAudioProcessor&);

private:
    // =========================================================================
    VTSAudioProcessor& processor;

    // =========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesignerPanel)
};
