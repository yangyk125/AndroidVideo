/*
 * NXGLMatrix.h
 *
 *  Created on: 2015-9-8
 *      Author: yangyk
 */

#pragma once

#include <math.h>

typedef union tag_NXVector3
{
    struct { float x, y, z; };
    struct { float r, g, b; };
    struct { float s, t, p; };
    float v[3];
} NXVector3;

typedef union tag_NXVector4
{
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	struct { float s, t, p, q; };
	float v[4];
} NXVector4;

typedef union tag_NXMatrix3
{
    struct
    {
        float m00, m01, m02 ;
        float m10, m11, m12 ;
        float m20, m21, m22 ;
    };
    float m[9];
} NXMatrix3 ;

typedef union tag_NXMatrix4
{
    struct
    {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float m[16];
} NXMatrix4 ;



NXINLINE NXMatrix4 NXMatrix4MakeOrtho(float left, float right, float bottom, float top, float nearZ, float farZ)
{
    float ral = right + left;
    float rsl = right - left;
    float tab = top + bottom;
    float tsb = top - bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;

    NXMatrix4 m =  { 2.0f / rsl, 0.0f, 0.0f, 0.0f,
                     0.0f, 2.0f / tsb, 0.0f, 0.0f,
                     0.0f, 0.0f, -2.0f / fsn, 0.0f,
                     -ral / rsl, -tab / tsb, -fan / fsn, 1.0f };

    return m;
}

NXINLINE NXMatrix4 NXMatrix4RotationX(float radian)
{
    /*
         |  1  0       0       0 |
     M = |  0  cos(A) -sin(A)  0 |
         |  0  sin(A)  cos(A)  0 |
         |  0  0       0       1 |

    */
	NXMatrix4 mat4 ;

    mat4.m[0] = 1.0f;
    mat4.m[1] = 0.0f;
    mat4.m[2] = 0.0f;
    mat4.m[3] = 0.0f;

    mat4.m[4] = 0.0f;
    mat4.m[5] = cosf(radian);
    mat4.m[6] = sinf(radian);
    mat4.m[7] = 0.0f;

    mat4.m[8] = 0.0f;
    mat4.m[9] = -sinf(radian);
    mat4.m[10] = cosf(radian);
    mat4.m[11] = 0.0f;

    mat4.m[12] = 0.0f;
    mat4.m[13] = 0.0f;
    mat4.m[14] = 0.0f;
    mat4.m[15] = 1.0f;

    return mat4;
}

/**
 * Builds a rotation matrix using the rotation around the Y-axis
 * The result is stored in pOut, pOut is returned.
 */
NXINLINE NXMatrix4 NXMatrix4RotationY(float radian)
{
    /*
         |  cos(A)  0   sin(A)  0 |
     M = |  0       1   0       0 |
         | -sin(A)  0   cos(A)  0 |
         |  0       0   0       1 |
    */
	NXMatrix4 mat4 ;

    mat4.m[0] = cosf(radian);
    mat4.m[1] = 0.0f;
    mat4.m[2] = -sinf(radian);
    mat4.m[3] = 0.0f;

    mat4.m[4] = 0.0f;
    mat4.m[5] = 1.0f;
    mat4.m[6] = 0.0f;
    mat4.m[7] = 0.0f;

    mat4.m[8] = sinf(radian);
    mat4.m[9] = 0.0f;
    mat4.m[10] = cosf(radian);
    mat4.m[11] = 0.0f;

    mat4.m[12] = 0.0f;
    mat4.m[13] = 0.0f;
    mat4.m[14] = 0.0f;
    mat4.m[15] = 1.0f;

    return mat4;
}

NXINLINE NXMatrix4 NXMatrix4RotationZ(float radian)
{
    /*
         |  cos(A)  -sin(A)   0   0 |
     M = |  sin(A)   cos(A)   0   0 |
         |  0        0        1   0 |
         |  0        0        0   1 |
    */

	NXMatrix4 mat4 ;

    mat4.m[0] = cosf(radian);
    mat4.m[1] = sinf(radian);
    mat4.m[2] = 0.0f;
    mat4.m[3] = 0.0f;

    mat4.m[4] = -sinf(radian);;
    mat4.m[5] = cosf(radian);
    mat4.m[6] = 0.0f;
    mat4.m[7] = 0.0f;

    mat4.m[8] = 0.0f;
    mat4.m[9] = 0.0f;
    mat4.m[10] = 1.0f;
    mat4.m[11] = 0.0f;

    mat4.m[12] = 0.0f;
    mat4.m[13] = 0.0f;
    mat4.m[14] = 0.0f;
    mat4.m[15] = 1.0f;

    return mat4 ;
}

NXINLINE NXVector3 NXVector3Add(NXVector3 vectorLeft, NXVector3 vectorRight)
{
	NXVector3 v =  { vectorLeft.v[0] + vectorRight.v[0],
                     vectorLeft.v[1] + vectorRight.v[1],
                     vectorLeft.v[2] + vectorRight.v[2] };
    return v;
}

NXINLINE NXVector3 NXVector3Negate(NXVector3 vector)
{
	NXVector3 v = { -vector.v[0], -vector.v[1], -vector.v[2] };
    return v;
}

NXINLINE float NXVector3Length(NXVector3 vector)
{
    return sqrt(vector.v[0] * vector.v[0] + vector.v[1] * vector.v[1] + vector.v[2] * vector.v[2]);
}

NXINLINE NXVector3 NXVector3Normalize(NXVector3 vector)
{
    float scale = 1.0f / NXVector3Length(vector);
    NXVector3 v = { vector.v[0] * scale, vector.v[1] * scale, vector.v[2] * scale };
    return v;
}

NXINLINE float NXVector3DotProduct(NXVector3 vectorLeft, NXVector3 vectorRight)
{
    return vectorLeft.v[0] * vectorRight.v[0] + vectorLeft.v[1] * vectorRight.v[1] + vectorLeft.v[2] * vectorRight.v[2];
}

NXINLINE NXVector3 NXVector3CrossProduct(NXVector3 vectorLeft, NXVector3 vectorRight)
{
	NXVector3 v =  { vectorLeft.v[1] * vectorRight.v[2] - vectorLeft.v[2] * vectorRight.v[1],
                     vectorLeft.v[2] * vectorRight.v[0] - vectorLeft.v[0] * vectorRight.v[2],
                     vectorLeft.v[0] * vectorRight.v[1] - vectorLeft.v[1] * vectorRight.v[0] };
    return v;
}

NXINLINE NXMatrix4 NXMatrix4MakeLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
{
    NXVector3 ev = { eyeX, eyeY, eyeZ };
    NXVector3 cv = { centerX, centerY, centerZ };
    NXVector3 uv = { upX, upY, upZ };
    NXVector3 n = NXVector3Normalize(NXVector3Add(ev, NXVector3Negate(cv)));
    NXVector3 u = NXVector3Normalize(NXVector3CrossProduct(uv, n));
    NXVector3 v = NXVector3CrossProduct(n, u);

    NXMatrix4 m =  { u.v[0], v.v[0], n.v[0], 0.0f,
                     u.v[1], v.v[1], n.v[1], 0.0f,
                     u.v[2], v.v[2], n.v[2], 0.0f,
                     NXVector3DotProduct(NXVector3Negate(u), ev),
                     NXVector3DotProduct(NXVector3Negate(v), ev),
                     NXVector3DotProduct(NXVector3Negate(n), ev),
                     1.0f };

    return m;
}

NXINLINE NXMatrix4 NXMatrix4Multiply(NXMatrix4 matrixLeft, NXMatrix4 matrixRight)
{
	NXMatrix4 m;

    m.m[0]  = matrixLeft.m[0] * matrixRight.m[0]  + matrixLeft.m[4] * matrixRight.m[1]  + matrixLeft.m[8] * matrixRight.m[2]   + matrixLeft.m[12] * matrixRight.m[3];
	m.m[4]  = matrixLeft.m[0] * matrixRight.m[4]  + matrixLeft.m[4] * matrixRight.m[5]  + matrixLeft.m[8] * matrixRight.m[6]   + matrixLeft.m[12] * matrixRight.m[7];
	m.m[8]  = matrixLeft.m[0] * matrixRight.m[8]  + matrixLeft.m[4] * matrixRight.m[9]  + matrixLeft.m[8] * matrixRight.m[10]  + matrixLeft.m[12] * matrixRight.m[11];
	m.m[12] = matrixLeft.m[0] * matrixRight.m[12] + matrixLeft.m[4] * matrixRight.m[13] + matrixLeft.m[8] * matrixRight.m[14]  + matrixLeft.m[12] * matrixRight.m[15];

	m.m[1]  = matrixLeft.m[1] * matrixRight.m[0]  + matrixLeft.m[5] * matrixRight.m[1]  + matrixLeft.m[9] * matrixRight.m[2]   + matrixLeft.m[13] * matrixRight.m[3];
	m.m[5]  = matrixLeft.m[1] * matrixRight.m[4]  + matrixLeft.m[5] * matrixRight.m[5]  + matrixLeft.m[9] * matrixRight.m[6]   + matrixLeft.m[13] * matrixRight.m[7];
	m.m[9]  = matrixLeft.m[1] * matrixRight.m[8]  + matrixLeft.m[5] * matrixRight.m[9]  + matrixLeft.m[9] * matrixRight.m[10]  + matrixLeft.m[13] * matrixRight.m[11];
	m.m[13] = matrixLeft.m[1] * matrixRight.m[12] + matrixLeft.m[5] * matrixRight.m[13] + matrixLeft.m[9] * matrixRight.m[14]  + matrixLeft.m[13] * matrixRight.m[15];

	m.m[2]  = matrixLeft.m[2] * matrixRight.m[0]  + matrixLeft.m[6] * matrixRight.m[1]  + matrixLeft.m[10] * matrixRight.m[2]  + matrixLeft.m[14] * matrixRight.m[3];
	m.m[6]  = matrixLeft.m[2] * matrixRight.m[4]  + matrixLeft.m[6] * matrixRight.m[5]  + matrixLeft.m[10] * matrixRight.m[6]  + matrixLeft.m[14] * matrixRight.m[7];
	m.m[10] = matrixLeft.m[2] * matrixRight.m[8]  + matrixLeft.m[6] * matrixRight.m[9]  + matrixLeft.m[10] * matrixRight.m[10] + matrixLeft.m[14] * matrixRight.m[11];
	m.m[14] = matrixLeft.m[2] * matrixRight.m[12] + matrixLeft.m[6] * matrixRight.m[13] + matrixLeft.m[10] * matrixRight.m[14] + matrixLeft.m[14] * matrixRight.m[15];

	m.m[3]  = matrixLeft.m[3] * matrixRight.m[0]  + matrixLeft.m[7] * matrixRight.m[1]  + matrixLeft.m[11] * matrixRight.m[2]  + matrixLeft.m[15] * matrixRight.m[3];
	m.m[7]  = matrixLeft.m[3] * matrixRight.m[4]  + matrixLeft.m[7] * matrixRight.m[5]  + matrixLeft.m[11] * matrixRight.m[6]  + matrixLeft.m[15] * matrixRight.m[7];
	m.m[11] = matrixLeft.m[3] * matrixRight.m[8]  + matrixLeft.m[7] * matrixRight.m[9]  + matrixLeft.m[11] * matrixRight.m[10] + matrixLeft.m[15] * matrixRight.m[11];
	m.m[15] = matrixLeft.m[3] * matrixRight.m[12] + matrixLeft.m[7] * matrixRight.m[13] + matrixLeft.m[11] * matrixRight.m[14] + matrixLeft.m[15] * matrixRight.m[15];

    return m;
}
