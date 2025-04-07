/*
  ==============================================================================

    Mappers.cpp

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

#include "Mappers.h"
#include <iostream>
#include <math.h>

// =============================================================================
template <typename ValueType>
LinearMapper<ValueType>::LinearMapper(ValueType x0, ValueType y0, ValueType x1,
                                      ValueType y1)
    : m((y1 - y0) / (x1 - x0)), q((y1 - y0) * x0 / (x0 - x1) + y0)
{
}

template <typename ValueType>
ValueType LinearMapper<ValueType>::map(ValueType x) const
{
    return m * x + q;
}

template class LinearMapper<float>;

// =============================================================================
template <typename ValueType>
ValueType identity(ValueType x)
{
    return x;
}
template float identity(float x);

// =============================================================================
template <typename ValueType>
InputTransformMapper<ValueType>::InputTransformMapper(
    ValueType x0, ValueType y0, ValueType x1, ValueType y1,
    std::function<ValueType(ValueType)> t)
    : LinearMapper<ValueType>(t(x0), y0, t(x1), y1), transform(t)
{
}

template <typename ValueType>
ValueType InputTransformMapper<ValueType>::map(ValueType x) const
{
    return LinearMapper<ValueType>::map(transform(x));
}

template class InputTransformMapper<float>;

// =============================================================================
template <typename ValueType>
OutputTransformMapper<ValueType>::OutputTransformMapper(
    ValueType x0, ValueType y0, ValueType x1, ValueType y1,
    std::function<ValueType(ValueType)> t,
    std::function<ValueType(ValueType)> t_i)
    : transform_inverse(t_i), LinearMapper<ValueType>(x0, t(y0), x1, t(y1))
{
}

template <typename ValueType>
ValueType OutputTransformMapper<ValueType>::map(ValueType x) const
{
    return transform_inverse(LinearMapper<ValueType>::map(x));
}

template class OutputTransformMapper<float>;
