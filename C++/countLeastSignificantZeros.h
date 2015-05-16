#pragma once

#include <cassert>
#include <cstdint>

#if defined(_MSC_VER)

#include <intrin.h>

inline int countLeastSignificantZeros(uint32_t value)
{
    assert(value != 0);
    unsigned long result;
    _BitScanForward(&result, value);
    return result;
}

#elif defined(__GNUC__) // #if defined(_MSC_VER)

inline int countLeastSignificantZeros(uint32_t value)
{
    assert(value != 0);
    return __builtin_ffs(value) - 1;
}

#endif // #if defined(_MSC_VER)
