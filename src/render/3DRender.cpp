#include "3DRender.h"
#include <vector>

Renderer::Renderer() : w(800), h(600) {}

Renderer::Renderer(const Camera& c, const Projection& p) : camera(c), proj(p), w(p.getWidth()), h(p.getHeight()) {}

void Renderer::addTriangle(const Triangle& t) {
    triangles[t.id] = t;
}

void Renderer::delTriangle(int id) {
    triangles.erase(id);
}

void Renderer::clearTriangles() {
    triangles.clear();
}

std::pair<int, int> Renderer::ndcToScreen(float x, float y) const {
    return {(x + 1) * w / 2, (1 - x) * h / 2};
}

void Renderer::paint() const {
    std::vector<uint32_t> fb(w * h);
    std::vector<Triangle> temp;

    for(auto p : triangles) {
        Triangle t = p.second;

        temp.push_back(new Triangle(t.id, ));
    }
}
