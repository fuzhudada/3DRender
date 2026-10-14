#pragma once
#include "vec3d.h"
#include "vec4d.h"
#include <cmath>

// 4x4 matrix, row-major storage: m[row][col]
// Transform composition: A * B means apply B first, then A
// Right-handed coordinate system, camera looks down -z
class Mat4d {
public:
    float m[4][4];

    // ---------- Construction ----------
    Mat4d();                          // default ctor: no initialization (for performance)
    static Mat4d identity();          // identity matrix

    // ---------- Basic transforms ----------
    // Translation matrix: m[0][3]=t.x, m[1][3]=t.y, m[2][3]=t.z
    static Mat4d translation(const Vec3d& t);

    // Scale matrix: diagonal (s.x, s.y, s.z, 1)
    static Mat4d scale(const Vec3d& s);
    static Mat4d scale(float s);      // uniform scale

    // Rotation around axis, angle in radians
    // rotationX: affects y,z rows/cols
    //   [1   0      0     0]
    //   [0  cos   -sin    0]
    //   [0  sin    cos    0]
    //   [0   0      0     1]
    static Mat4d rotationX(float rad);
    static Mat4d rotationY(float rad);
    static Mat4d rotationZ(float rad);

    // Rotation around an arbitrary axis (optional, advanced)
    static Mat4d rotationAxis(const Vec3d& axis, float rad);

    // ---------- Projection ----------
    // Perspective projection matrix, right-handed, camera looks down -z
    // fovY:   vertical field of view (radians)
    // aspect: width / height
    // n, f:   near / far plane distances (positive, measured along -z)
    //
    // Resulting matrix (row-major):
    //   [ 1/aspect*t 0        0              0        ]
    //   [ 0          1/t      0              0        ]
    //   [ 0          0    (f+n)/(n-f)   2*f*n/(n-f)   ]
    //   [ 0          0       -1              0        ]
    // where t = 1 / tan(fovY/2)
    static Mat4d perspective(float fovY, float aspect, float n, float f);

    // Orthographic projection (optional, for debugging)
    static Mat4d orthographic(float l, float r, float b, float t, float n, float f);

    // ---------- View ----------
    // View matrix: transforms world space to camera space
    // eye:    camera position
    // center: look-at target
    // up:     approximate up direction
    //
    // Steps:
    //   f = normalize(center - eye)       // camera forward (looking down -z, so z-axis is -f)
    //   s = normalize(cross(f, up))       // right direction (x-axis)
    //   u = cross(s, f)                   // true up direction (y-axis)
    //   view = [ s.x  s.y  s.z  -dot(s,eye) ]
    //          [ u.x  u.y  u.z  -dot(u,eye) ]
    //          [-f.x -f.y -f.z   dot(f,eye) ]
    //          [  0    0    0        1      ]
    static Mat4d lookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up);

    // ---------- Multiplication ----------
    Mat4d operator*(const Mat4d& o) const;   // matrix * matrix
    Vec4d operator*(const Vec4d& v) const;   // matrix * vector (column vector)

    // ---------- Helpers ----------
    Mat4d transposed() const;                // transpose (useful for inversion)

    // Debug print
    void print() const;

    // Element access
    float&       at(int row, int col)       { return m[row][col]; }
    const float& at(int row, int col) const { return m[row][col]; }
};
