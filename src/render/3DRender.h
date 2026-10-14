#pragma once
#include <unordered_map>
#include <utility>

#include "src/model/triangle.h"
#include "camera.h"
#include "projection.h"

class Renderer {
private:
    std::unordered_map<int, Triangle> triangles;
    Camera camera;
    Projection proj;
    int w, h;

    std::pair<int, int> ndcToScreen(float x, float y) const;

public:
    Renderer();
    Renderer(const Camera& c, const Projection& p);

    void addTriangle(const Triangle& t);
    void delTriangle(int id);
    void clearTriangles();

    Camera & getCamera() { return camera; }
    Projection & getProjection() { return proj; }

    void setCamera(const Camera& c) { this->camera = camera; }
    void setProjection(const Projection& p) { this->proj = proj; }

    void paint() const;
};
