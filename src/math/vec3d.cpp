#include "vec3d.h"
#include <cstdio>

Vec3d Vec3d::operator+(const Vec3d& o) const { return {x + o.x, y + o.y, z + o.z}; }
Vec3d Vec3d::operator-(const Vec3d& o) const { return {x - o.x, y - o.y, z - o.z}; }
Vec3d Vec3d::operator-() const               { return {-x, -y, -z}; }
Vec3d Vec3d::operator*(float s) const        { return {x * s, y * s, z * s}; }
Vec3d& Vec3d::operator+=(const Vec3d& o)     { x += o.x; y += o.y; z += o.z; return *this; }
Vec3d& Vec3d::operator-=(const Vec3d& o)     { x -= o.x; y -= o.y; z -= o.z; return *this; }

float Vec3d::dot(const Vec3d& o) const { return x * o.x + y * o.y + z * o.z; }

Vec3d Vec3d::cross(const Vec3d& o) const {
    return {y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x};
}

float Vec3d::length() const { return std::sqrt(x * x + y * y + z * z); }

Vec3d Vec3d::normalized() const {
    float len = length();
    if (len < 1e-8f) return {0, 0, 0};
    return (*this) * (1.0f / len);
}

void Vec3d::print() const { std::printf("(%.3f, %.3f, %.3f)\n", x, y, z); }
