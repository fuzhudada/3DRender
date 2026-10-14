#include "camera.h"

Camera::Camera() : mat(Mat4d::lookAt({0, 0, 0}, {0, 0, -1}, {0, 1, 0})), worldUp({0, 1, 0}) {}

Camera::Camera(const Vec3d& eye, const Vec3d& center, const Vec3d& up) : mat(Mat4d::lookAt(eye, center, up)), worldUp(up.normalized()) {}

Vec3d Camera::getEye() const {
    Mat4d tmat = mat.transposed();

    Vec4d vecm = {mat.m[0][3], mat.m[1][3], mat.m[2][3], 1};

    Vec4d negeye = tmat * vecm;
    return -negeye.xyz();
}

Vec3d Camera::getFace() const {
    return Vec3d{-mat.m[2][0], -mat.m[2][1], -mat.m[2][2]};
}

Vec3d Camera::getUp() const {
    return Vec3d{mat.m[1][0], mat.m[1][1], mat.m[1][2]};
}

void Camera::setEye(const Vec3d& eye) {
    Vec3d face = getFace();
    Vec3d center = eye + face;
    mat = Mat4d::lookAt(eye, center, worldUp);
}

void Camera::setFace(const Vec3d& face) {
    Vec3d norface = face.normalized();
    Vec3d eye = getEye();

    if(norface.cross(worldUp).length() > 1e-9) {
        Vec3d center = eye + norface;
        mat = Mat4d::lookAt(eye, center, worldUp);
    }
}

void Camera::setEyeAndFace(const Vec3d& eye, const Vec3d& center) {
    if((center - eye).normalized().cross(worldUp).length() > 1e-9) {
        mat = Mat4d::lookAt(eye, center, worldUp);
    }
}
