#ifndef MATH_UTILITY_HPP
#define MATH_UTILITY_HPP

#include <cstdint>

constexpr double
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
exponential(double x)
{
    return (1.0 + x + power(x, 2) / factorial(2) + power(x, 3) / factorial(3) +
            power(x, 4) / factorial(4) + power(x, 5) / factorial(5));
}

#endif