#include "light.h"

#include <algorithm>

Light::Light() : source(1, 1, 1), mode(LightMode::Directional), ka(0.2f) {}

Light::Light(const Vec3d& source, const LightMode& mode, float ka) : source(source), mode(mode), ka(ka) {}

float Light::FlatLambert(const Triangle& t) const {
    Vec3d n = (t.b - t.a).cross(t.c - t.a).normalized();
    Vec3d L;
    if(mode == LightMode::Directional){
        L = source.normalized();
    }else{
        Vec3d center = (1.0f / 3.0f) * (t.a + t.b + t.c);
        L = (source - center).normalized();
    }
    return ka + (1.0f - ka) * std::max(0.0f, n.dot(L));
}
