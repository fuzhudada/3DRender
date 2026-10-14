#include "mat4.h"
#include <cstdio>

Mat4d::Mat4d() : m{} {}

Mat4d Mat4d::identity() {
    Mat4d r;
    r.m[0][0] = 1;
    r.m[1][1] = 1;
    r.m[2][2] = 1;
    r.m[3][3] = 1;
    return r;
}

Mat4d Mat4d::translation(const Vec3d& t) {
    Mat4d r = identity();
    r.m[0][3] = t.x;
    r.m[1][3] = t.y;
    r.m[2][3] = t.z;
    return r;
}

Mat4d Mat4d::scale(const Vec3d& s) {
    Mat4d r = identity();
    r.m[0][0] = s.x;
    r.m[1][1] = s.y;
    r.m[2][2] = s.z;
    return r;
}

Mat4d Mat4d::scale(float s) {
    Mat4d r = identity();
    r.m[0][0] = s;
    r.m[1][1] = s;
    r.m[2][2] = s;
    return r;
}

Mat4d Mat4d::rotationX(float rad) {
    Mat4d r = identity();
    r.m[1][1] = std::cos(rad);
    r.m[1][2] = -std::sin(rad);
    r.m[2][1] = std::sin(rad);
    r.m[2][2] = std::cos(rad);
    return r;
}

Mat4d Mat4d::rotationY(float rad) {
    Mat4d r = identity();
    r.m[2][2] = std::cos(rad);
    r.m[2][0] = -std::sin(rad);
    r.m[0][2] = std::sin(rad);
    r.m[0][0] = std::cos(rad);
    return r;
}

Mat4d Mat4d::rotationZ(float rad) {
    Mat4d r = identity();
    r.m[0][0] = std::cos(rad);
    r.m[0][1] = -std::sin(rad);
    r.m[1][0] = std::sin(rad);
    r.m[1][1] = std::cos(rad);
    return r;
}

Mat4d Mat4d::rotationAxis(const Vec3d& axis, float rad) {
    Vec3d k = axis.normalized();
    float c = std::cos(rad);
    float s = std::sin(rad);
    float t = 1.0f - c;

    Mat4d r = identity();
    r.m[0][0] = c + t * k.x * k.x;
    r.m[0][1] = t * k.x * k.y - s * k.z;
    r.m[0][2] = t * k.x * k.z + s * k.y;
    r.m[1][0] = t * k.x * k.y + s * k.z;
    r.m[1][1] = c + t * k.y * k.y;
    r.m[1][2] = t * k.y * k.z - s * k.x;
    r.m[2][0] = t * k.x * k.z - s * k.y;
    r.m[2][1] = t * k.y * k.z + s * k.x;
    r.m[2][2] = c + t * k.z * k.z;
    return r;
}

Mat4d Mat4d::perspective(float fovY, float aspect, float n, float f) {
    float t = std::tan(fovY / 2);

    Mat4d r = identity();
    r.m[0][0] = 1.0f / aspect / t;
    r.m[1][1] = 1.0f / t;
    r.m[2][2] = (n + f) / (n - f);
    r.m[2][3] = 2 * n * f / (n - f);
    r.m[3][2] = -1;
    r.m[3][3] = 0;
    return r;
}

Mat4d Mat4d::orthographic(float l, float r, float b, float t, float n, float f) {
    Mat4d res = identity();
    res.m[0][0] = 2.0f / (r - l);
    res.m[0][3] = -(r + l) / (r - l);
    res.m[1][1] = 2.0f / (t - b);
    res.m[1][3] = -(t + b) / (t - b);
    res.m[2][2] = -2.0f / (f - n);
    res.m[2][3] = -(f + n) / (f - n);
    return res;
}

Mat4d Mat4d::lookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up) {
    Vec3d f = (center - eye).normalized();
    Vec3d u = (up - (up.dot(f)) * f).normalized();
    Vec3d s = f.cross(u);

    Mat4d r = identity();
    r.m[0][0] = s.x;
    r.m[0][1] = s.y;
    r.m[0][2] = s.z;
    r.m[0][3] = -eye.dot(s);
    r.m[1][0] = u.x;
    r.m[1][1] = u.y;
    r.m[1][2] = u.z;
    r.m[1][3] = -eye.dot(u);
    r.m[2][0] = -f.x;
    r.m[2][1] = -f.y;
    r.m[2][2] = -f.z;
    r.m[2][3] = eye.dot(f);
    return r;
}

Mat4d Mat4d::operator*(const Mat4d& o) const {
    Mat4d r = identity();
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 4; ++j){
            r.m[i][j] = 0;
            for(int k = 0; k < 4; ++k){
                r.m[i][j] += (*this).m[i][k] * o.m[k][j];
            }
        }
    }
    return r;
}

Vec4d Mat4d::operator*(const Vec4d& v) const {
    return Vec4d(
        m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w,
        m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w,
        m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w,
        m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w
    );
}

Mat4d Mat4d::transposed() const {
    Mat4d r = identity();
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 4; ++j){
            r.m[i][j] = (*this).m[j][i];
        }
    }
    return r;
}

void Mat4d::print() const {
    for(int i = 0; i < 4; ++i){
        std::printf("(%.3f, %.3f, %.3f, %.3f)\n", (*this).m[i][0], (*this).m[i][1], (*this).m[i][2], (*this).m[i][3]);
    }
}
