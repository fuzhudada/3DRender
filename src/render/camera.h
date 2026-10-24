#pragma once
#include "src/math/mat4.h"

class Camera {
private:
    Mat4d mat;
    Vec3d worldUp, eye;

public:
    Camera();
    Camera(const Vec3d& eye, const Vec3d& center, const Vec3d& up);

    Vec3d getEye() const;
    Vec3d getFace() const;
    Vec3d getSide() const;
    Vec3d getUp() const;

    void setEye(const Vec3d& eye);
    void setFace(const Vec3d& face);
    void setEyeAndFace(const Vec3d& eye, const Vec3d& center);

    const Mat4d& getMat() const { return mat; }
    const Vec3d& getWorldUp() const { return worldUp; }

    void setWorldUp(const Vec3d& up) { worldUp = up.normalized(); }

    Vec4d operator*(const Vec4d& v) const { return mat * v; }
};
