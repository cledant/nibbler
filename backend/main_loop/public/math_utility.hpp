#ifndef MATH_UTILITY_HPP
#define MATH_UTILITY_HPP

#include <cstdint>

constexpr uint64_t
factorial(uint64_t i)
{
    if (!i) {
        return (1);
    }
    return (i * factorial(i - 1));
}

constexpr double
power(double x, uint64_t y)
{
    if (!y) {
        return (1.0);
    }
    return (x * power(x, y - 1));
}

constexpr double
exponential_recursive_taylor(double x, double acc, uint64_t i)
{
    if (!i) {
        return (acc);
    }
    acc = (1.0 + ((x * acc) / static_cast<double>(i)));
    return (exponential_recursive_taylor(x, acc, i - 1));
}

constexpr double
exponential(double x)
{
    if (x < -10.0) {
        return (0.0);
    }
    return (exponential_recursive_taylor(x, 1.0, 64));
}

#endif