#pragma once
#include <cmath>

class Vec3d {
public:
    float x, y, z;

    Vec3d() : x(0), y(0), z(0) {}
    Vec3d(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3d operator+(const Vec3d& o) const;
    Vec3d operator-(const Vec3d& o) const;
    Vec3d operator-() const;
    Vec3d operator*(float s) const;
    Vec3d& operator+=(const Vec3d& o);
    Vec3d& operator-=(const Vec3d& o);

    float dot(const Vec3d& o) const;
    Vec3d cross(const Vec3d& o) const;
    float length() const;
    Vec3d normalized() const;

    void print() const;
};

inline Vec3d operator*(float s, const Vec3d& v) { return v * s; }
