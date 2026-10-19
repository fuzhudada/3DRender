#pragma once

#include "src/model/triangle.h"
#include "src/render/camera.h"
#include "src/render/projection.h"
#include "src/render/light.h"

#include <vector>

class Renderer {
private:
    std::vector<Triangle>& triangles;
    std::vector<float> z_buffer;
    Camera& camera;
    Projection& proj;
    Light& light;
    int w, h;

    Vec4d ndcToScreen(const Vec4d& v) const;
    void triangleTransform(const Triangle& t, std::vector<Triangle>& out) const;

public:
    Renderer(std::vector<Triangle>& triangles, Camera& camera, Projection& proj, Light& light);

    void resize(int w, int h);

    void paint(std::vector<uint32_t>& fb);
};
