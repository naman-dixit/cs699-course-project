#if !defined(MATHEMATICS_H_INCLUDE_GUARD)

#include <math.h>

/* Wrapper for needed math.h functions */

#define MATH_PI 3.14159265358979323846f

/* =========================================================================
 * Exponents
 * =========
 */

/* -------------------------------------------------------------------------
 * Square Root
 * ------ ----
 */

#define mathSqrt(x) _Generic((x),                       \
                             F32 : mathSqrt##F32)(x)

header_function
F32 mathSqrtF32 (F32 x)
{
    F32 y = sqrtf(x);
    return y;
}

/* -------------------------------------------------------------------------
 * Power Of 2
 * ----- -- -
 */

#define mathPow2(x) _Generic((x),                       \
                             F32 : mathPow2##F32,       \
                             U32 : mathPow2##U32)(x)

header_function
F32 mathPow2F32 (F32 x)
{
    F32 y = exp2f(x);
    return y;
}

header_function
U32 mathPow2U32 (U32 x)
{
    U32 y = 1 << x;
    return y;
}

/* =========================================================================
 * Trigonometry
 * ============
 */

/* -------------------------------------------------------------------------
 * Sin
 * ---
 */

#define mathSin(x) _Generic((x),                        \
                            F32 : mathSin##F32)(x)

header_function
F32 mathSinF32 (F32 x)
{
    F32 y = sinf(x);
    return y;
}

/* -------------------------------------------------------------------------
 * Cos
 * ---
 */

#define mathCos(x) _Generic((x),                        \
                            F32 : mathCos##F32)(x)

header_function
F32 mathCosF32 (F32 x)
{
    F32 y = cosf(x);
    return y;
}

/* -------------------------------------------------------------------------
 * Tan
 * ---
 */

#define mathTan(x) _Generic((x),                        \
                            F32 : mathTan##F32)(x)

header_function
F32 mathTanF32 (F32 x)
{
    F32 y = tanf(x);
    return y;
}

/* ========================================================================
 * Trancedental
 * ============
 */

/* -----------------------------------------------------------------------
 * Logarithm of 2
 * --------- -- -
 */

#define mathLog2(x) _Generic((x),                       \
                             F32 : mathLog2##F32,       \
                             U32: mathLog2##U32)(x)

header_function
F32 mathLog2F32 (F32 x)
{
    F32 y = log2f(x);
    return y;
}

header_function
U32 mathLog2U32 (U32 x)
{
    U32 v = x; // find the log base 2 of 32-bit v

    alignas(32) U8 MultiplyDeBruijnBitPosition[32] = {
        0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
        8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
    };

    v |= v >> 1; // first round down to one less than a power of 2
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    U32 y = MultiplyDeBruijnBitPosition[(U32)(v * 0x07C4ACDDU) >> 27];

    return y;
}

/* ========================================================================
 * Algebraic
 * =========
 */

/* -----------------------------------------------------------------------
 * Fractional Part
 * ---------- ----
 */

#define mathFrac(x) _Generic((x),                       \
                             F32 : mathFrac##F32)(x)

header_function
F32 mathFracF32 (F32 x)
{
    F32 temp = 0;
    F32 y = modff(x, &temp);
    return y;
}

/* ========================================================================
 * Miscellaneous
 * =============
 */

/* -----------------------------------------------------------------------
 * Radians from Degrees
 * ------- ---- -------
 */

#define mathRadians(x) _Generic((x),                            \
                                F32 : mathRadians##F32)(x)

header_function
F32 mathRadiansF32 (F32 degrees)
{
    F32 radians = (degrees * MATH_PI) / 180.0f;
    return radians;
}

/* -----------------------------------------------------------------------
 * Degrees from Radians
 * ------- ---- -------
 */

#define mathDegrees(x) _Generic((x),                            \
                                F32 : mathDegrees##F32)(x)

header_function
F32 mathDegreesF32 (F32 radians)
{
    F32 degrees = (radians * 180.0f) / MATH_PI;
    return degrees;
}

#define MATHEMATICS_H_INCLUDE_GUARD
#endif
