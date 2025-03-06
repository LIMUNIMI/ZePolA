#include "TopMenuPanel.h"

// =============================================================================
TopMenuPanel::TopMenuPanel(VTSAudioProcessor&)
{
    addAndMakeVisible(sep);
    sep.drawBottom = true;
}
void TopMenuPanel::resized() { sep.setBounds(getLocalBounds()); }
