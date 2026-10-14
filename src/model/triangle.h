#pragma once
#include "src/math/vec3d.h"
#include <cstdint>

class Triangle {
public:
    Vec3d a, b, c;
    uint32_t color;
    int id;

    Triangle();
    Triangle(int id, const Vec3d& a, const Vec3d& b, const Vec3d& c, uint32_t color);
};
