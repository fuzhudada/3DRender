#include "vec4d.h"
#include <cmath>
#include <cstdio>

Vec4d Vec4d::operator+(const Vec4d& o) const { return {x + o.x, y + o.y, z + o.z, w + o.w}; }
Vec4d Vec4d::operator-(const Vec4d& o) const { return {x - o.x, y - o.y, z - o.z, w - o.w}; }
Vec4d Vec4d::operator-() const                { return {-x, -y, -z, -w}; }
Vec4d Vec4d::operator*(float s) const         { return {x * s, y * s, z * s, w * s}; }
Vec4d& Vec4d::operator+=(const Vec4d& o)      { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
Vec4d& Vec4d::operator-=(const Vec4d& o)      { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }

float Vec4d::dot(const Vec4d& o) const { return x * o.x + y * o.y + z * o.z + w * o.w; }

float Vec4d::length() const { return std::sqrt(x * x + y * y + z * z + w * w); }

Vec4d Vec4d::normalized() const {
    float len = length();
    if (len < 1e-8f) return {0, 0, 0, 0};
    return (*this) * (1.0f / len);
}

void Vec4d::print() const { std::printf("(%.3f, %.3f, %.3f, %.3f)\n", x, y, z, w); }
