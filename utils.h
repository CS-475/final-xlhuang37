#ifndef _g_utils_h_
#define _g_utils_h_


#include "include/GTypes.h"

// Static Hacks
static inline unsigned int divisionHax32(unsigned int x) {
    return ((x + 0x00800080 + ((x & 0xFF00FF00) >> 8)) & 0xFF00FF00) >> 8;
}

static inline unsigned int divisionHax(unsigned int x) {
    return ((x + 128) * 257) >> 16;
}

static inline int getAbsoluteValue(int32_t x) {
    uint32_t mask = x >> 31;
    return (x ^ mask) + (mask & 1);
}

static inline bool isInRangeNoninclusive(int x, int left, int right) {
    // Works only when left < right
    return static_cast<unsigned>(x - (left + 1)) <= static_cast<unsigned>(right - left - 1);
}

static inline bool isInRangeInclusive(int x, int left, int right) {
    // Works only when left < right
    return static_cast<unsigned>(x - left) <= static_cast<unsigned>(right - left);
}

static inline int clamping(int curr, int low, int high) {
    return std::min(std::max(curr, low), high);
}

static inline int GRoundToIntNonNegative(float x) {
    return (int)(x + 0.5f);
}

static inline int GFloorToIntNonNegative(float x) {
    return (int)(x);
}

static inline int returnSignBit(float f) {
    // Directly return the sign bit as a boolean
    return (*reinterpret_cast<unsigned int*>(&f)) >> 31;
}

static inline float myQuotientMinusFloorf(float quotient) {
    return quotient - (int) quotient + returnSignBit(quotient);
}

static inline GPoint eval_quad_GPoint(const float t, const GPoint& A, const GPoint& B, const GPoint& C) {
    return (A - 2 * B + C) * t * t 
            + (2 * (B - A)) * t
            + A;
}

static inline GPoint eval_cubic_GPoint(const float t, const GPoint& A, const GPoint& B, const GPoint& C, const GPoint& D) {
    return (D - A + 3 * (B - C) ) * t * t * t
            + 3 * (A - 2 * B + C) * t * t
            + 3 * (B - A) * t
            + A;
}

// return 1 if non-negative, -1 if negative
static inline int signFunction(int x) {
    return 1 - 2 * ((x >> 31) & 1);
}

// static inline int RoundAndClamp(float val, int min_val, int max_val) {
//     int rounded = static_cast<int>(std::nearbyint(val));
//     return (rounded < min_val) ? min_val : (rounded > max_val ? max_val : rounded);
// }

#endif
