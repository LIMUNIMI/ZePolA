#pragma once
#include <JuceHeader.h>

/** This is a macro for logging and asserting about unhandles switch cases */
#define UNHANDLED_SWITCH_CASE(S)                                               \
    JUCE_BLOCK_WITH_FORCED_SEMICOLON(DBG(S); jassertfalse;)

/** This is a macro for including a piece of code only in debug mode
    Similar to juce's DBG, but it doesn't log anything
*/
#if (JUCE_DEBUG && !JUCE_DISABLE_ASSERTIONS) || DOXYGEN
#define ONLY_ON_DEBUG(X) X
#define ONLY_ON_RELEASE(X)
#else
#define ONLY_ON_DEBUG(X)
#define ONLY_ON_RELEASE(X) X
#endif

#define DEBUG_VS_RELEASE(D, R) ONLY_ON_DEBUG(D) ONLY_ON_RELEASE(R)
