#pragma once
#include <unordered_map>
#include <utility>

#include "src/model/triangle.h"
#include "camera.h"
#include "projection.h"
#include "src/render/display.h"

class Renderer {
private:
    std::vector<Triangle> triangles;
    std::vector<uint32_t> fb;
    std::vector<float> z_buffer;
    Camera camera;
    Projection proj;
    int w, h;
    float speed, sensitivity;
    float yaw, pitch;

    Vec4d ndcToScreen(const Vec4d& v) const;
    void triangleTransform(const Triangle& t, std::vector<Triangle>& out) const;

    void getFb();

    void mainLoop();
    void updateLook(float dt);
    void updateMove(float dt);

public:
    Renderer();
    Renderer(const Camera& c, const Projection& p, float speed, float sensitivity);

    void addTriangle(const Triangle& t);
    void clearTriangles();

    Camera getCamera() const { return camera; }
    Projection getProjection() const { return proj; }

    void setCamera(const Camera& c);
    void setProjection(const Projection& p) { this->proj = p; }

    float getSpeed() const { return speed; }
    float getSensitivity() const { return sensitivity; }

    void setSpeed(float speed) { this->speed = speed; }
    void setSensitivity(float sensitivity) { this->sensitivity = sensitivity; }

    void paint();
};
