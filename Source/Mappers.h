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
