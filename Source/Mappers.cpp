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
