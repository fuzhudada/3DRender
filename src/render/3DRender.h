#pragma once
#include <unordered_map>
#include <utility>

#include "src/model/triangle.h"
#include "camera.h"
#include "projection.h"
#include "src/render/display.h"

class Renderer {
private:
    std::unordered_map<int, Triangle> triangles;
    Camera camera;
    Projection proj;
    int w, h;

    Vec4d ndcToScreen(const Vec4d& v) const;
    Vec3d pointTransform(const Vec3d& v) const;
    void triangleTransform(Triangle& t) const;

    bool isIn(int x, int y, const Triangle& t, float &w1, float &w2, float &w3) const;

public:
    Renderer();
    Renderer(const Camera& c, const Projection& p);

    void addTriangle(const Triangle& t);
    void delTriangle(int id);
    void clearTriangles();

    Camera & getCamera() { return camera; }
    Projection & getProjection() { return proj; }

    void setCamera(const Camera& c) { this->camera = c; }
    void setProjection(const Projection& p) { this->proj = p; }

    void paint();
};
