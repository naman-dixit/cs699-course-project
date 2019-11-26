#if !defined(LINEAR_ALGEBRA_H_INCLUDE_GUARD)

#include "mathematics.h"

/* 2-D Vectors */

typedef union {
    struct { F32 x, y; };
    struct { F32 u, v; };
    struct { F32 w, h; };
    alignas(8) Byte alignment[8];
} Vec2;

header_function
Vec2 vec2New (F32 x, F32 y)
{
    Vec2 result = {0};
    result.x = x;
    result.y = y;

    return result;
}

header_function
Vec2 vec2Invert (Vec2 v)
{
    return vec2New(-v.x, -v.y);
}

header_function
Vec2 vec2Add (Vec2 v, Vec2 u)
{
    return vec2New(v.x + u.x,
                   v.y + u.y);
}

header_function
Vec2 vec2Sub (Vec2 v, Vec2 u)
{
    return vec2New(v.x - u.x,
                   v.y - u.y);
}

header_function
Vec2 vec2Mul (Vec2 v, Vec2 u)
{
    return vec2New(v.x * u.x,
                   v.y * u.y);
}

header_function
Vec2 vec2Scale (Vec2 v, F32 r)
{
    return vec2New(v.x * r,
                   v.y * r);
}

header_function
F32 vec2Dot (Vec2 v, Vec2 u)
{
    return ((v.x * u.x) +
            (v.y * u.y));
}

header_function
Vec2 vec2Lerp (Vec2 v, Vec2 u, F32 factor)
{
    return vec2New(v.x + factor * (u.x - v.x),
                   v.y + factor * (u.y - v.y));
}

header_function
F32 vec2Mag (Vec2 v)
{
    return mathSqrtF32((v.x * v.x) +
                       (v.y * v.y));
}

header_function
F32 vec2MagSq (Vec2 v)
{
    return ((v.x * v.x) +
            (v.y * v.y));
}

header_function
Vec2 vec2Norm (Vec2 v)
{
    F32 mag = vec2Mag(v);
    // NOTE(naman): An exact comparison with zero is what we want.
    if (mag != 0.0f) {
        F32 inv_mag = 1.0f / mag;
        return vec2New(v.x * inv_mag,
                       v.y * inv_mag);
    } else {
        return vec2New(0.0f, 0.0f);
    }
}


/* 3-D Vector */

typedef union {
    struct {
        F32 x, y, z, _pad;
    };
    alignas(16) Byte alignment[16];
} Vec3;

header_function
Vec3 vec3New (F32 x, F32 y, F32 z)
{
    Vec3 result = {x, y, z};
    return result;
}

header_function
Vec3 vec3Invert (Vec3 v)
{
    return vec3New(-v.x, -v.y, -v.z);
}

header_function
Vec3 vec3Add (Vec3 v, Vec3 u)
{
    return vec3New(v.x + u.x,
                   v.y + u.y,
                   v.z + u.z);
}

header_function
Vec3 vec3Sub (Vec3 v, Vec3 u)
{
    return vec3New(v.x - u.x,
                   v.y - u.y,
                   v.z - u.z);
}

header_function
Vec3 vec3Mul (Vec3 v, Vec3 u)
{
    return vec3New(v.x * u.x,
                   v.y * u.y,
                   v.z * u.z);
}

header_function
Vec3 vec3Scale (Vec3 v, F32 r)
{
    return vec3New(v.x * r,
                   v.y * r,
                   v.z * r);
}

header_function
F32 vec3Dot (Vec3 v, Vec3 u)
{
    return ((v.x * u.x) +
            (v.y * u.y) +
            (v.z * u.z));
}

header_function
Vec3 vec3Cross (Vec3 v, Vec3 u)
{
    return vec3New((v.y * u.z) - (v.z * u.y),
                   (v.z * u.x) - (v.x * u.z),
                   (v.x * u.y) - (v.y * u.x));
}

header_function
Vec3 vec3Lerp (Vec3 v, Vec3 u, F32 factor)
{
    return vec3New(v.x + factor * (u.x - v.x),
                   v.y + factor * (u.y - v.y),
                   v.z + factor * (u.z - v.z));
}

header_function
F32 vec3Mag (Vec3 v)
{
    return mathSqrtF32((v.x * v.x) +
                       (v.y * v.y) +
                       (v.z * v.z));
}

header_function
F32 vec3MagSq (Vec3 v)
{
    return ((v.x * v.x) +
            (v.y * v.y) +
            (v.z * v.z));
}

header_function
Vec3 vec3Norm (Vec3 v)
{
    F32 mag = vec3Mag(v);
    // NOTE(naman): An exact comparison with zero is what we want.
    if (mag != 0.0f) {
        F32 inv_mag = 1.0f/mag;
        return vec3New(v.x * inv_mag,
                       v.y * inv_mag,
                       v.z * inv_mag);
    } else {
        return vec3New(0.0f, 0.0f, 0.0f);
    }
}

/* Quaternion */

typedef union {
    struct {
        F32 r, i, j, k;
    };
    alignas(16) Byte alignment[16];
} Quat;

header_function
Quat quatNew (F32 r, F32 i, F32 j, F32 k)
{
    return (Quat){r, i, j, k};
}

static inline
Quat quatMul (Quat q, Quat p)
{
    return quatNew((q.r * p.r) - (q.i * p.i) - (q.j * p.j) - (q.k * p.k),
                   (q.r * p.i) + (q.i * p.r) + (q.j * p.k) - (q.k * p.j),
                   (q.r * p.j) + (q.j * p.r) + (q.k * p.i) - (q.i * p.k),
                   (q.r * p.k) + (q.k * p.r) + (q.i * p.j) - (q.j * p.i));
}

static inline
F32 quatMagSq (Quat q)
{
    return ((q.r * q.r) +
            (q.i * q.i) +
            (q.j * q.j) +
            (q.k * q.k));
}

static inline
F32 quatMag (Quat q)
{
    return mathSqrtF32(quatMagSq(q));
}

static inline
Quat quatNorm (Quat q)
{
    F32 mag = quatMag(q);
    // NOTE(naman): An exact comparison with zero is what we want.
    if (mag != 0.0f) {
        F32 inv_mag = 1.0f/mag;
        return quatNew(q.r * inv_mag,
                       q.i * inv_mag,
                       q.j * inv_mag,
                       q.k * inv_mag);
    } else {
        return quatNew(0.0f, 0.0f, 0.0f, 0.0f);
    }
}


/* Matrix */

typedef union Mat4 {
    /* Column-major:
      _            _
      | 0  4  8  12 |
      | 1  5  9  13 |
      | 2  6  10 14 |
      | 3  7  11 15 |
      -            -
    */
    F32 elem[16];

    alignas(64) Byte cacheline[64];

    struct {
        /*   0    1    2    3 */
        F32 a00, a10, a20, a30;
        /*   4    5    6    7 */
        F32 a01, a11, a21, a31;
        /*   8    9   10   11 */
        F32 a02, a12, a22, a32;
        /*  12   13   14   15 */
        F32 a03, a13, a23, a33;
    };

    struct {
        F32 c0[4];
        F32 c1[4];
        F32 c2[4];
        F32 c3[4];
    } column;
} Mat4;

// NOTE(naman): All matrix functions operate on column-major matrices with a
// right-handed coordinate system.

header_function
Mat4 mat4New (void)
{
    Mat4 m;

    m.elem[0] = 0;
    m.elem[1] = 0;
    m.elem[2] = 0;
    m.elem[3] = 0;

    m.elem[4] = 0;
    m.elem[5] = 0;
    m.elem[6] = 0;
    m.elem[7] = 0;

    m.elem[8] = 0;
    m.elem[9] = 0;
    m.elem[10] = 0;
    m.elem[11] = 0;

    m.elem[12] = 0;
    m.elem[13] = 0;
    m.elem[14] = 0;
    m.elem[15] = 0;

    return m;
}

header_function
Mat4 mat4Identity (void)
{
    Mat4 m;

    m.elem[0] = 1;
    m.elem[1] = 0;
    m.elem[2] = 0;
    m.elem[3] = 0;

    m.elem[4] = 0;
    m.elem[5] = 1;
    m.elem[6] = 0;
    m.elem[7] = 0;

    m.elem[8] = 0;
    m.elem[9] = 0;
    m.elem[10] = 1;
    m.elem[11] = 0;

    m.elem[12] = 0;
    m.elem[13] = 0;
    m.elem[14] = 0;
    m.elem[15] = 1;

    return m;
}

header_function
Mat4 mat4Add (Mat4 m, Mat4 n)
{
    Mat4 o = {0};

    for (int i = 0; i < 16; ++i) {
        o.elem[i] = m.elem[i] + n.elem[i];
    }

    return o;
}

header_function
Mat4 mat4Sub (Mat4 m, Mat4 n)
{
    Mat4 o = {0};

    for (int i = 0; i < 16; ++i) {
        o.elem[i] = m.elem[i] - n.elem[i];
    }

    return o;
}


header_function
Mat4 mat4Mul (Mat4 m, Mat4 n)
{
    Mat4 o = {0};

    for (int i = 0; i < 16; i = i + 4) {
        for (int j = 0; j < 16; j = j + 4) {
            int index = i/4;
            o.elem[index+j] = ((m.elem[index] * n.elem[j]) +
                               (m.elem[index+4] * n.elem[j+1]) +
                               (m.elem[index+8] * n.elem[j+2]) +
                               (m.elem[index+12] * n.elem[j+3]));
        }
    }

    return o;
}

header_function
Mat4 mat4MulScalar (Mat4 m, F32 r)
{
    Mat4 n = {0};

    for (int i = 0; i < 16; ++i) {
        n.elem[i] = m.elem[i] * r;
    }

    return n;
}

header_function
F32 mat4Determinant (Mat4 m)
{
    F32 a = (m.elem[6] * m.elem[11]) - (m.elem[10] * m.elem[7]);
    F32 b = (m.elem[10] * m.elem[15]) - (m.elem[14] * m.elem[11]);
    F32 c = (m.elem[6] * m.elem[15]) - (m.elem[14] * m.elem[7]);
    F32 d = (m.elem[2] * m.elem[11]) - (m.elem[10] * m.elem[3]);
    F32 e = (m.elem[2] * m.elem[15]) - (m.elem[14] * m.elem[3]);
    F32 f = (m.elem[2] * m.elem[7]) - (m.elem[6] * m.elem[3]);


    F32 aa = (m.elem[5] * b) - (m.elem[9] * c) + (m.elem[13] * a);
    F32 bb = (m.elem[1] * b) - (m.elem[9] * e) + (m.elem[13] * d);
    F32 cc = (m.elem[1] * c) - (m.elem[5] * e) + (m.elem[13] * f);
    F32 dd = (m.elem[1] * a) - (m.elem[5] * d) + (m.elem[9] * f);

    F32 det = ((m.elem[0] * aa) - (m.elem[4] * bb) +
                   (m.elem[8] * cc) - (m.elem[12] * dd));

    return det;
}

header_function
Mat4 mat4Inverse (Mat4 m)
{
    Mat4 n;

    n.elem[0] = ((m.elem[5]  * m.elem[10] * m.elem[15]) -
                 (m.elem[5]  * m.elem[11] * m.elem[14]) -
                 (m.elem[9]  * m.elem[6]  * m.elem[15]) +
                 (m.elem[9]  * m.elem[7]  * m.elem[14]) +
                 (m.elem[13] * m.elem[6]  * m.elem[11]) -
                 (m.elem[13] * m.elem[7]  * m.elem[10]));

    n.elem[1] = ((-m.elem[4]  * m.elem[10] * m.elem[15]) +
                 (m.elem[4]  * m.elem[11] * m.elem[14]) +
                 (m.elem[8]  * m.elem[6]  * m.elem[15]) -
                 (m.elem[8]  * m.elem[7]  * m.elem[14]) -
                 (m.elem[12] * m.elem[6]  * m.elem[11]) +
                 (m.elem[12] * m.elem[7]  * m.elem[10]));

    n.elem[2] = ((m.elem[4]  * m.elem[9] * m.elem[15]) -
                 (m.elem[4]  * m.elem[11] * m.elem[13]) -
                 (m.elem[8]  * m.elem[5] * m.elem[15]) +
                 (m.elem[8]  * m.elem[7] * m.elem[13]) +
                 (m.elem[12] * m.elem[5] * m.elem[11]) -
                 (m.elem[12] * m.elem[7] * m.elem[9]));

    n.elem[3] = ((-m.elem[4]  * m.elem[9] * m.elem[14]) +
                 (m.elem[4]  * m.elem[10] * m.elem[13]) +
                 (m.elem[8]  * m.elem[5] * m.elem[14]) -
                 (m.elem[8]  * m.elem[6] * m.elem[13]) -
                 (m.elem[12] * m.elem[5] * m.elem[10]) +
                 (m.elem[12] * m.elem[6] * m.elem[9]));

    n.elem[4] = ((-m.elem[1]  * m.elem[10] * m.elem[15]) +
                 (m.elem[1]  * m.elem[11] * m.elem[14]) +
                 (m.elem[9]  * m.elem[2] * m.elem[15]) -
                 (m.elem[9]  * m.elem[3] * m.elem[14]) -
                 (m.elem[13] * m.elem[2] * m.elem[11]) +
                 (m.elem[13] * m.elem[3] * m.elem[10]));

    n.elem[5] = ((m.elem[0]  * m.elem[10] * m.elem[15]) -
                 (m.elem[0]  * m.elem[11] * m.elem[14]) -
                 (m.elem[8]  * m.elem[2] * m.elem[15]) +
                 (m.elem[8]  * m.elem[3] * m.elem[14]) +
                 (m.elem[12] * m.elem[2] * m.elem[11]) -
                 (m.elem[12] * m.elem[3] * m.elem[10]));

    n.elem[6] = ((-m.elem[0]  * m.elem[9] * m.elem[15]) +
                 (m.elem[0]  * m.elem[11] * m.elem[13]) +
                 (m.elem[8]  * m.elem[1] * m.elem[15]) -
                 (m.elem[8]  * m.elem[3] * m.elem[13]) -
                 (m.elem[12] * m.elem[1] * m.elem[11]) +
                 (m.elem[12] * m.elem[3] * m.elem[9]));

    n.elem[7] = ((m.elem[0]  * m.elem[9] * m.elem[14]) -
                 (m.elem[0]  * m.elem[10] * m.elem[13]) -
                 (m.elem[8]  * m.elem[1] * m.elem[14]) +
                 (m.elem[8]  * m.elem[2] * m.elem[13]) +
                 (m.elem[12] * m.elem[1] * m.elem[10]) -
                 (m.elem[12] * m.elem[2] * m.elem[9]));

    n.elem[8] = ((m.elem[1]  * m.elem[6] * m.elem[15]) -
                 (m.elem[1]  * m.elem[7] * m.elem[14]) -
                 (m.elem[5]  * m.elem[2] * m.elem[15]) +
                 (m.elem[5]  * m.elem[3] * m.elem[14]) +
                 (m.elem[13] * m.elem[2] * m.elem[7]) -
                 (m.elem[13] * m.elem[3] * m.elem[6]));

    n.elem[9] = ((-m.elem[0]  * m.elem[6] * m.elem[15]) +
                 (m.elem[0]  * m.elem[7] * m.elem[14]) +
                 (m.elem[4]  * m.elem[2] * m.elem[15]) -
                 (m.elem[4]  * m.elem[3] * m.elem[14]) -
                 (m.elem[12] * m.elem[2] * m.elem[7]) +
                 (m.elem[12] * m.elem[3] * m.elem[6]));

    n.elem[10] = ((m.elem[0]  * m.elem[5] * m.elem[15]) -
                  (m.elem[0]  * m.elem[7] * m.elem[13]) -
                  (m.elem[4]  * m.elem[1] * m.elem[15]) +
                  (m.elem[4]  * m.elem[3] * m.elem[13]) +
                  (m.elem[12] * m.elem[1] * m.elem[7]) -
                  (m.elem[12] * m.elem[3] * m.elem[5]));

    n.elem[11] = ((-m.elem[0]  * m.elem[5] * m.elem[14]) +
                  (m.elem[0]  * m.elem[6] * m.elem[13]) +
                  (m.elem[4]  * m.elem[1] * m.elem[14]) -
                  (m.elem[4]  * m.elem[2] * m.elem[13]) -
                  (m.elem[12] * m.elem[1] * m.elem[6]) +
                  (m.elem[12] * m.elem[2] * m.elem[5]));

    n.elem[12] = ((-m.elem[1] * m.elem[6] * m.elem[11]) +
                  (m.elem[1] * m.elem[7] * m.elem[10]) +
                  (m.elem[5] * m.elem[2] * m.elem[11]) -
                  (m.elem[5] * m.elem[3] * m.elem[10]) -
                  (m.elem[9] * m.elem[2] * m.elem[7]) +
                  (m.elem[9] * m.elem[3] * m.elem[6]));

    n.elem[13] = ((m.elem[0] * m.elem[6] * m.elem[11]) -
                  (m.elem[0] * m.elem[7] * m.elem[10]) -
                  (m.elem[4] * m.elem[2] * m.elem[11]) +
                  (m.elem[4] * m.elem[3] * m.elem[10]) +
                  (m.elem[8] * m.elem[2] * m.elem[7]) -
                  (m.elem[8] * m.elem[3] * m.elem[6]));

    n.elem[14] = ((-m.elem[0] * m.elem[5] * m.elem[11]) +
                  (m.elem[0] * m.elem[7] * m.elem[9]) +
                  (m.elem[4] * m.elem[1] * m.elem[11]) -
                  (m.elem[4] * m.elem[3] * m.elem[9]) -
                  (m.elem[8] * m.elem[1] * m.elem[7]) +
                  (m.elem[8] * m.elem[3] * m.elem[5]));

    n.elem[15] = ((m.elem[0] * m.elem[5] * m.elem[10]) -
                  (m.elem[0] * m.elem[6] * m.elem[9]) -
                  (m.elem[4] * m.elem[1] * m.elem[10]) +
                  (m.elem[4] * m.elem[2] * m.elem[9]) +
                  (m.elem[8] * m.elem[1] * m.elem[6]) -
                  (m.elem[8] * m.elem[2] * m.elem[5]));

    F32 det = (m.elem[0] * n.elem[0] +
                   m.elem[1] * n.elem[1] +
                   m.elem[2] * n.elem[2] +
                   m.elem[3] * n.elem[3]);

    det = 1.0f / det;

    return mat4MulScalar(n, det);
}

header_function
Mat4 mat4Translate2D (Vec2 v)
{
    Mat4 m;

    m.elem[0] = 1.0f;
    m.elem[1] = 0.0f;
    m.elem[2] = 0.0f;
    m.elem[3] = 0.0f;

    m.elem[4] = 0.0f;
    m.elem[5] = 1.0f;
    m.elem[6] = 0.0f;
    m.elem[7] = 0.0f;

    m.elem[8] = 0.0f;
    m.elem[9] = 0.0f;
    m.elem[10] = 1.0f;
    m.elem[11] = 0.0f;

    m.elem[12] = v.x;
    m.elem[13] = v.y;
    m.elem[14] = 0.0f;
    m.elem[15] = 1.0f;

    return m;
}

header_function
Mat4 mat4Translate3D (Vec3 v)
{
    Mat4 m;

    m.elem[0] = 1.0f;
    m.elem[1] = 0.0f;
    m.elem[2] = 0.0f;
    m.elem[3] = 0.0f;

    m.elem[4] = 0.0f;
    m.elem[5] = 1.0f;
    m.elem[6] = 0.0f;
    m.elem[7] = 0.0f;

    m.elem[8] = 0.0f;
    m.elem[9] = 0.0f;
    m.elem[10] = 1.0f;
    m.elem[11] = 0.0f;

    m.elem[12] = v.x;
    m.elem[13] = v.y;
    m.elem[14] = v.z;
    m.elem[15] = 1.0f;

    return m;
}


header_function
Mat4 mat4Rotate3D (Quat q)
{
    Mat4 m;

    m.elem[0] = 1.0f - (2.0f * q.j * q.j) - (2.0f * q.k * q.k);
    m.elem[1] = (2.0f * q.i * q.j) + (2.0f * q.k * q.r);
    m.elem[2] = (2.0f * q.i * q.k) - (2.0f * q.j * q.r);
    m.elem[3] = 0.0f;

    m.elem[4] = (2.0f * q.i * q.j) - (2.0f * q.k * q.r);
    m.elem[5] = 1.0f - (2.0f * q.i * q.i) - (2.0f * q.k * q.k);
    m.elem[6] = (2.0f * q.j * q.k) + (2.0f * q.i * q.r);
    m.elem[7] = 0.0f;

    m.elem[8] = (2.0f * q.i * q.k) + (2.0f * q.j * q.r);
    m.elem[9] = (2.0f * q.j * q.k) - (2.0f * q.i * q.r);
    m.elem[10] = 1.0f - (2.0f * q.i * q.i) - (2.0f * q.j * q.j);
    m.elem[11] = 0.0f;

    m.elem[12] = 0.0f;
    m.elem[13] = 0.0f;
    m.elem[14] = 0.0f;
    m.elem[15] = 1.0f;

    return m;
}


header_function
Mat4 mat4Scale2D (Vec2 v)
{
    Mat4 m;

    m.elem[0] = v.x;
    m.elem[1] = 0;
    m.elem[2] = 0;
    m.elem[3] = 0;

    m.elem[4] = 0;
    m.elem[5] = v.y;
    m.elem[6] = 0;
    m.elem[7] = 0;

    m.elem[8] = 0;
    m.elem[9] = 0;
    m.elem[10] = 1;
    m.elem[11] = 0;

    m.elem[12] = 0;
    m.elem[13] = 0;
    m.elem[14] = 0;
    m.elem[15] = 1;

    return m;
}

header_function
Mat4 mat4Scale3D (Vec3 v)
{
    Mat4 m;

    m.elem[0] = v.x;
    m.elem[1] = 0;
    m.elem[2] = 0;
    m.elem[3] = 0;

    m.elem[4] = 0;
    m.elem[5] = v.y;
    m.elem[6] = 0;
    m.elem[7] = 0;

    m.elem[8] = 0;
    m.elem[9] = 0;
    m.elem[10] = v.z;
    m.elem[11] = 0;

    m.elem[12] = 0;
    m.elem[13] = 0;
    m.elem[14] = 0;
    m.elem[15] = 1;

    return m;
}

header_function
Mat4 mat4LookAt (Vec3 eye, Vec3 lookat, Vec3 world_up)
{
    Vec3 direction = vec3Sub(lookat, eye);
    direction = vec3Norm(direction);

    Vec3 right = vec3Cross(direction, world_up);
    right = vec3Norm(right);

    Vec3 up = vec3Cross(right, direction);

    Mat4 m;

    m.elem[0] = right.x;
    m.elem[1] = up.x;
    m.elem[2] = -direction.x;
    m.elem[3] = 0.0f;

    m.elem[4] = right.y;
    m.elem[5] = up.y;
    m.elem[6] = -direction.y;
    m.elem[7] = 0.0f;

    m.elem[8] = right.z;
    m.elem[9] = up.z;
    m.elem[10] = -direction.z;
    m.elem[11] = 0.0f;

    m.elem[12] = 0.0f;
    m.elem[13] = 0.0f;
    m.elem[14] = 0.0f;
    m.elem[15] = 1.0f;

    Mat4 n = mat4Translate3D(vec3Invert(eye));

    Mat4 result = mat4Mul(m, n);

    return result;
}

/* NOTE(naman): Ortograpic and Perspective projecction matrices are different for OpenGL
   and Direct3D because of difference in z-buffer depth ([-1,1] vs [0,1]). */

// Right-handed coordinate system with depth range [-1, 1]
header_function
Mat4 mat4OrthographicGL (F32 left, F32 right, F32 bottom, F32 top, F32 near_plane, F32 far_plane)
{
    F32 plane_add = near_plane + far_plane;
    F32 plane_sub = near_plane - far_plane;

    Mat4 m;

    m.elem[0] = 2.0f / (right - left);
    m.elem[1] = 0.0f;
    m.elem[2] = 0.0f;
    m.elem[3] = 0.0f;

    m.elem[4] = 0.0f;
    m.elem[5] = 2.0f / (top - bottom);
    m.elem[6] = 0.0f;
    m.elem[7] = 0.0f;

    m.elem[8] = 0.0f;
    m.elem[9] = 0.0f;
    m.elem[10] = 2.0f / plane_sub;
    m.elem[11] = 0.0f;

    m.elem[12] = (left + right) / (left - right);
    m.elem[13] = (bottom + top) / (bottom - top);
    m.elem[14] = plane_add / plane_sub;
    m.elem[15] = 1.0f;

    return m;
}

// Right-handed coordinate system with depth range [0, 1]
header_function
Mat4 mat4OrthographicD3D11 (F32 width, F32 height, F32 near_plane, F32 far_plane)
{
    F32 plane_sub = near_plane - far_plane;

    Mat4 m;

    m.elem[0] = 2.0f / width;
    m.elem[1] = 0.0f;
    m.elem[2] = 0.0f;
    m.elem[3] = 0.0f;

    m.elem[4] = 0.0f;
    m.elem[5] = 2.0f / height;
    m.elem[6] = 0.0f;
    m.elem[7] = 0.0f;

    m.elem[8] = 0.0f;
    m.elem[9] = 0.0f;
    m.elem[10] = 1.0f / plane_sub;
    m.elem[11] = 0.0f;

    m.elem[12] = 0.0f;
    m.elem[13] = 0.0f;
    m.elem[14] = near_plane / plane_sub;
    m.elem[15] = 1.0f;

    return m;
}

// Right-handed coordinate system with depth range [-1, 1]
header_function
Mat4 mat4PerspectiveGL (F32 y_fov, F32 aspect, F32 near_plane, F32 far_plane)
{
    F32 a = 1.0f/mathTanF32(y_fov/2.0f);

    F32 plane_add = near_plane + far_plane;
    F32 plane_sub = near_plane - far_plane;

    Mat4 m;

    m.elem[0] = a / aspect;
    m.elem[1] = 0.0f;
    m.elem[2] = 0.0f;
    m.elem[3] = 0.0f;

    m.elem[4] = 0.0f;
    m.elem[5] = a;
    m.elem[6] = 0.0f;
    m.elem[7] = 0.0f;

    m.elem[8] = 0.0f;
    m.elem[9] = 0.0f;
    m.elem[10] = plane_add/plane_sub;
    m.elem[11] = -1.0f;

    m.elem[12] = 0.0f;
    m.elem[13] = 0.0f;
    m.elem[14] = (2.0f * far_plane * near_plane) / plane_sub;
    m.elem[15] = 1.0f;

    return m;
}

// Right-handed coordinate system with depth range [0, 1]
header_function
Mat4 mat4PerspectiveD3D11 (F32 y_fov, F32 aspect, F32 near_plane, F32 far_plane)
{
    F32 a = 1.0f/mathTanF32(y_fov/2.0f);

    F32 plane_sub = near_plane - far_plane;

    Mat4 m;

    m.elem[0] = a / aspect;
    m.elem[1] = 0.0f;
    m.elem[2] = 0.0f;
    m.elem[3] = 0.0f;

    m.elem[4] = 0.0f;
    m.elem[5] = a;
    m.elem[6] = 0.0f;
    m.elem[7] = 0.0f;

    m.elem[8] = 0.0f;
    m.elem[9] = 0.0f;
    m.elem[10] = far_plane/plane_sub;
    m.elem[11] = -1.0f;

    m.elem[12] = 0.0f;
    m.elem[13] = 0.0f;
    m.elem[14] = (far_plane * near_plane) / plane_sub;
    m.elem[15] = 0.0f;

    return m;
}

#define LINEAR_ALGEBRA_H_INCLUDE_GUARD
#endif
