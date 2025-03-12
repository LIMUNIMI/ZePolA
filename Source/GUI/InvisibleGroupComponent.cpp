#include "InvisibleGroupComponent.h"

// =============================================================================
InvisibleGroupComponent::InvisibleGroupComponent() {}

// =============================================================================
void InvisibleGroupComponent::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<InvisibleGroupComponentLookAndFeelMethods*>(
            &getLookAndFeel()))
        laf->dontDrawGroupComponent(g, getWidth(), getHeight(), getText(), 0,
                                    *this);
}

// =============================================================================
SeparatorComponent::SeparatorComponent()
    : drawTop(false), drawBottom(false), drawLeft(false), drawRight(false)
{
}
void SeparatorComponent::paint(juce::Graphics& g)
{
    if (auto laf = dynamic_cast<SeparatorComponent::LookAndFeelMethods*>(
            &getLookAndFeel()))
        laf->drawSeparator(g, getX(), getY(), getWidth(), getHeight(), drawTop,
                           drawBottom, drawLeft, drawRight, *this);
}
