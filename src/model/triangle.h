#pragma once
#include "src/math/vec3d.h"
#include <cstdint>

class Triangle {
public:
    Vec3d a, b, c;
    Vec3d na, nb, nc;
    uint32_t color;

    Triangle();
    Triangle(const Vec3d& a, const Vec3d& b, const Vec3d& c, const Vec3d& na, const Vec3d& nb, const Vec3d& nc, uint32_t color);
};
