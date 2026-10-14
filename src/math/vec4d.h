#pragma once
#include "vec3d.h"

class Vec4d {
public:
    float x, y, z, w;

    Vec4d() : x(0), y(0), z(0), w(0) {}
    Vec4d(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    explicit Vec4d(const Vec3d& v, float w = 1.0f)
        : x(v.x), y(v.y), z(v.z), w(w) {}

    Vec4d operator+(const Vec4d& o) const;
    Vec4d operator-(const Vec4d& o) const;
    Vec4d operator-() const;
    Vec4d operator*(float s) const;
    Vec4d& operator+=(const Vec4d& o);
    Vec4d& operator-=(const Vec4d& o);

    float dot(const Vec4d& o) const;
    float length() const;
    Vec4d normalized() const;

    Vec3d xyz() const { return {x, y, z}; }

    void print() const;
};

inline Vec4d operator*(float s, const Vec4d& v) { return v * s; }
