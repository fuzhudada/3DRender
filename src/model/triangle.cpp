#include "triangle.h"

Triangle::Triangle() : id(0), a(0, 0, 0), b(0, 0, 0), c(0, 0, 0), color(0) {}

Triangle::Triangle(int id, const Vec3d& a, const Vec3d& b, const Vec3d& c, uint32_t color) : id(id), a(a), b(b), c(c), color(color) {}
