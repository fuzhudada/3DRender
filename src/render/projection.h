#pragma once
#include "src/math/mat4.h"
constexpr float PI_F = 3.1415926535f;

class Projection {
private:
    float fovY, n, f;
    int w, h;

public:
    Projection() : fovY(PI_F / 3), n(0.1f), f(100.0f), w(800), h(600) {}
    Projection(float fovY, int w, int h, float n, float f) : fovY(fovY), n(n), f(f), w(w), h(h) {}

    float getFovY() const { return fovY; }
    int getWidth() const { return w; }
    int getHeight() const { return h; }
    float getNear() const { return n; }
    float getFar() const { return f; }

    void setFovY(float fovY) { this->fovY = fovY; }
    void setWidthAndHeight(int w, int h) { this->w = w, this->h = h; }
    void setNear(float n) { this->n = n; }
    void setFar(float f) { this->f = f; }

    Mat4d getMat() const { return Mat4d::perspective(fovY, 1.0f * w / h, n, f); }

    Vec4d operator*(const Vec4d& v) const { return getMat() * v; }
};
