#include "light.h"

#include <algorithm>

float Light::getLightOnCenter(const Vec3d& n, const Vec3d& center, const Vec3d& eye) const {
    Vec3d L;
    if(mode == LightMode::Directional){
        L = source.normalized();
    }else{
        L = (source - center).normalized();
    }
    Vec3d R = 2 * (n.dot(L)) * n - L;
    Vec3d V = (eye - center).normalized();
    float l = ka + (1.0f - ka) * (kd * std::max(0.0f, n.dot(L)) + ks * std::pow(std::max(0.0f, R.dot(V)), shininess));
    return std::min(l, 1.0f);
}

Light::Light() : source(1, 1, 1), mode(LightMode::Directional), ka(0.2f), kd(0.8f), ks(0.5f), shininess(32.0f) {}

Light::Light(const Vec3d& source, const LightMode& mode, float ka, float kd, float ks, float shininess) : source(source), mode(mode), ka(ka), kd(kd), ks(ks), shininess(shininess) {}

float Light::FlatLambert(const Triangle& t) const {
    Vec3d n = (t.b - t.a).cross(t.c - t.a).normalized();
    Vec3d L;
    if(mode == LightMode::Directional){
        L = source.normalized();
    }else{
        Vec3d center = (1.0f / 3.0f) * (t.a + t.b + t.c);
        L = (source - center).normalized();
    }
    float l = ka + (1.0f - ka) * kd * std::max(0.0f, n.dot(L));
    return std::min(l, 1.0f);
}

float Light::FlatPhong(const Triangle& t, const Vec3d& eye) const {
    Vec3d n = (t.b - t.a).cross(t.c - t.a).normalized();
    Vec3d center = (1.0f / 3.0f) * (t.a + t.b + t.c);
    float l = getLightOnCenter(n, center, eye);
    return l;
}

Vec3d Light::GouraudPhong(const Triangle& t, const Vec3d& eye) const {
    float l1 = getLightOnCenter(t.na.normalized(), t.a, eye);
    float l2 = getLightOnCenter(t.nb.normalized(), t.b, eye);
    float l3 = getLightOnCenter(t.nc.normalized(), t.c, eye);
    return {l1, l2, l3};
}
