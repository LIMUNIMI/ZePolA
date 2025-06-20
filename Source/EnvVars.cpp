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
#include <algorithm>  // std::transform
#include <cctype>     // std::tolower
#include <cstdlib>    // std::getenv
#include <unordered_set>

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
        try
        {
            v = parse(s);
        }
        catch (...)
        {
        }
    }
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
static char _safe_tolower(unsigned char c)
{
    return static_cast<char>(std::tolower(c));
}
template <>
bool EnvVar<bool>::parse(const char* v)
{
    static const std::unordered_set<std::string> _TRUES {"1", "true", "yes",
                                                         "on"};
    std::string s(v);
    std::transform(s.begin(), s.end(), s.begin(), _safe_tolower);
    return _TRUES.count(s) > 0;
}

// =============================================================================
template class EnvVar<int>;
template class EnvVar<double>;
template class EnvVar<bool>;
