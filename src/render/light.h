#pragma once

#include "src/math/vec3d.h"
#include "src/model/triangle.h"

enum class LightMode {
    Directional,
    Point
};

class Light {
private:
    Vec3d source;
    LightMode mode;
    float ka, kd, ks, shininess;

public:
    Light();
    Light(const Vec3d& source, const LightMode& mode, float ka, float kd, float ks, float shininess);

    Vec3d getSource() const { return source; }
    LightMode getMode() const { return mode; }
    float getKa() const { return ka; }
    float getKd() const { return kd; }
    float getKs() const { return ks; }
    float getShininess() const { return shininess; }

    void setSource(const Vec3d& source) { this->source = source; }
    void setMode(const LightMode& mode) { this->mode = mode; }
    void setKa(float ka) { this->ka = ka; }
    void setKd(float kd) { this->kd = kd; }
    void setKs(float ks) { this->ks = ks; }
    void setShininess(float shininess) { this->shininess = shininess; }

    float getLightOnCenter(const Vec3d& n, const Vec3d& center, const Vec3d& eye) const;

    float FlatLambert(const Triangle& t) const;
    float FlatPhong(const Triangle& t, const Vec3d& eye) const;
    Vec3d GouraudPhong(const Triangle& t, const Vec3d& eye) const;
};
