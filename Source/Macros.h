/*
  ==============================================================================

    Macros.h

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

#define SAFE_MessageManager_LOCK(THIS, BODY)                                   \
    if ((juce::MessageManager::getInstanceWithoutCreating() != nullptr         \
         && juce::MessageManager::getInstanceWithoutCreating()                 \
                ->currentThreadHasLockedMessageManager())                      \
        || THIS->getPeer() == nullptr)                                         \
    {                                                                          \
        const juce::MessageManagerLock mmLock;                                 \
        {                                                                      \
            BODY                                                               \
        }                                                                      \
    }
