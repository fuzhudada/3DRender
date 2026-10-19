#pragma once

#include "src/render/renderer.h"
#include "src/render/light.h"
#include "cameraController.h"

class Application {
private:
    std::vector<Triangle> triangles;
    std::vector<uint32_t> fb;
    Camera camera;
    Projection proj;
    Renderer renderer;
    CameraController con;
    Light light;
    int w, h;
    bool lightFollowsCamera = false;

    void mainLoop();

public:
    Application();
    Application(const Camera& c, const Projection& p, float speed, float sensitivity);

    void addTriangle(const Triangle& t);
    void clearTriangles();

    Camera getCamera() const { return camera; }
    Projection getProjection() const { return proj; }

    void setCamera(const Camera& c);
    void setProjection(const Projection& p) { this->proj = p; }

    float getSpeed() const { return con.getSpeed(); }
    float getSensitivity() const { return con.getSensitivity(); }

    void setSpeed(float speed) { con.setSpeed(speed); }
    void setSensitivity(float sensitivity) { con.setSensitivity(sensitivity); }

    void paint();
};
