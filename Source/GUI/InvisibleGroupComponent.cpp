#include "InvisibleGroupComponent.h"

// =============================================================================
InvisibleGroupComponent::InvisibleGroupComponent() { }

// =============================================================================
void InvisibleGroupComponent::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<InvisibleGroupComponentLookAndFeelMethods*>(
            &getLookAndFeel()))
        laf->dontDrawGroupComponent(g, getWidth(), getHeight(), getText(), 0,
                                    *this);
}
