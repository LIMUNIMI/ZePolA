/*
  ==============================================================================

    Mappers.h

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
#include <functional>

// =============================================================================
/** Linear map function class */
template <typename ValueType>
class LinearMapper
{
public:
    // =========================================================================
    LinearMapper(ValueType x0, ValueType y0, ValueType x1, ValueType y1);

    // =========================================================================
    /** Apply map */
    virtual ValueType map(ValueType x) const;

private:
    // =========================================================================
    ValueType m, q;
};

// =============================================================================
/** Identity function */
template <typename ValueType>
ValueType identity(ValueType);

// =============================================================================
/** Linear map function class with transformed outputs */
template <typename ValueType>
class InputTransformMapper : public LinearMapper<ValueType>
{
public:
    // =========================================================================
    InputTransformMapper(ValueType x0, ValueType y0, ValueType x1, ValueType y1,
                         std::function<ValueType(ValueType)> transform
                         = identity<ValueType>);

    // =========================================================================
    /** Apply map */
    ValueType map(ValueType x) const override;

private:
    // =========================================================================
    std::function<ValueType(ValueType)> transform;
};

// =============================================================================
/** Linear map function class with transformed outputs */
template <typename ValueType>
class OutputTransformMapper : public LinearMapper<ValueType>
{
public:
    // =========================================================================
    OutputTransformMapper(ValueType x0, ValueType y0, ValueType x1,
                          ValueType y1,
                          std::function<ValueType(ValueType)> transform
                          = identity<ValueType>,
                          std::function<ValueType(ValueType)> transform_inverse
                          = identity<ValueType>);

    // =========================================================================
    /** Apply map */
    ValueType map(ValueType x) const override;

private:
    // =========================================================================
    std::function<ValueType(ValueType)> transform_inverse;
};
