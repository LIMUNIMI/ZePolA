/*
  ==============================================================================

    EnvVars.cpp

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

#include "EnvVars.h"
#include "Macros.h"
#include <JuceHeader.h>

// =============================================================================
template <typename TypeName>
EnvVar<TypeName>::EnvVar(const std::string& k, TypeName d) : key(k), dflt(d)
{
}
template <typename TypeName>
EnvVar<TypeName>::operator TypeName() const
{
    TypeName v = dflt;
    if (const char* s = std::getenv(key.c_str()))
    {
        DBG("Environment variable '" << key << "' found");
        try
        {
            v = parse(s);
        }
        catch (...)
        {
            DBG("Error parsing environment variable: '" << key << "'");
        }
    }
    ONLY_ON_DEBUG(
        else { DBG("Environment variable '" << key << "' not found"); });
    DBG(key << "=" << v);
    return v;
}

// =============================================================================
template <>
int EnvVar<int>::parse(const char* v)
{
    return std::stoi(v);
}
template <>
double EnvVar<double>::parse(const char* v)
{
    return std::stod(v);
}

// =============================================================================
template class EnvVar<int>;
template class EnvVar<double>;
