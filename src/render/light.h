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
    float ka;

public:
    Light();
    Light(const Vec3d& source, const LightMode& mode, float ka);

    Vec3d getSource() const { return source; }
    LightMode getMode() const { return mode; }
    float getKa() const { return ka; }

    void setSource(const Vec3d& source) { this->source = source; }
    void setMode(const LightMode& mode) { this->mode = mode; }
    void setKa(float ka) { this->ka = ka; }

    float FlatLambert(const Triangle& t) const;
};
