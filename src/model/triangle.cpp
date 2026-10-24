#include "triangle.h"

Triangle::Triangle() : a(0, 0, 0), b(0, 0, 0), c(0, 0, 0), na(0, 0, 0), nb(0, 0, 0), nc(0, 0, 0), color(0) {}

Triangle::Triangle(const Vec3d& a, const Vec3d& b, const Vec3d& c, const Vec3d& na, const Vec3d& nb, const Vec3d& nc, uint32_t color) : a(a), b(b), c(c), na(na), nb(nb), nc(nc), color(color) {}
