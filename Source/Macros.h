#pragma once
#include <JuceHeader.h>

#define UNHANDLED_SWITCH_CASE(S)                                               \
    JUCE_BLOCK_WITH_FORCED_SEMICOLON(DBG(S); jassertfalse;)
