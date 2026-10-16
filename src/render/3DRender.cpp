#include "3DRender.h"
#include <vector>
#include <math.h>

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

Vec4d Renderer::ndcToScreen(const Vec4d& v) const {
    return {(v.x + 1) * w / 2, (1 - v.y) * h / 2, v.z, v.w};
}

Vec3d Renderer::pointTransform(const Vec3d& v) const {
    Vec4d v4 = {v.x, v.y, v.z, 1};
    v4 = proj * (camera * v4);
    v4 = (1.0f / v4.w) * v4;
    v4 = ndcToScreen(v4);
    return v4.xyz();
}

void Renderer::triangleTransform(Triangle& t) const {
    t.a = pointTransform(t.a);
    t.b = pointTransform(t.b);
    t.c = pointTransform(t.c);
}

bool Renderer::isIn(int x, int y, const Triangle& t, float &w1, float &w2, float &w3) const {
    float fx = x + 0.5f;
    float fy = y + 0.5f;

    float a = ((t.b.x - t.a.x) * (t.c.y - t.a.y) - (t.b.y - t.a.y) * (t.c.x - t.a.x));
    if(std::fabs(a) < 1e-9) return false;

    w1 = ((t.c.x - t.b.x) * (fy - t.b.y) - (t.c.y - t.b.y) * (fx - t.b.x)) / a;
    w2 = ((t.a.x - t.c.x) * (fy - t.c.y) - (t.a.y - t.c.y) * (fx - t.c.x)) / a;
    w3 = ((t.b.x - t.a.x) * (fy - t.a.y) - (t.b.y - t.a.y) * (fx - t.a.x)) / a;
    return w1 >= 0 && w2 >= 0 && w3 >= 0;
}

void Renderer::paint() {
    std::vector<uint32_t> fb(w * h);
    std::vector<float> z_buffer(w * h, 2.0f);

    for(auto p : triangles) {
        Triangle t = p.second;
        triangleTransform(t);

        float minx = std::min(std::min(t.a.x, t.b.x), t.c.x);
        float maxx = std::max(std::max(t.a.x, t.b.x), t.c.x);
        float miny = std::min(std::min(t.a.y, t.b.y), t.c.y);
        float maxy = std::max(std::max(t.a.y, t.b.y), t.c.y);

        minx = std::max(0.0f, minx);
        maxx = std::min((float)(w - 1), maxx);
        miny = std::max(0.0f, miny);
        maxy = std::min((float)(h - 1), maxy);

        for(int i = miny; i <= maxy; ++i) {
            for(int j = minx; j <= maxx; ++j) {
                float w1, w2, w3;
                if(isIn(j, i, t, w1, w2, w3)) {
                    float depth = w1 * t.a.z + w2 * t.b.z + w3 * t.c.z;

                    if(z_buffer[i * w + j] > depth) {
                        z_buffer[i * w + j] = depth;
                        fb[i * w + j] = t.color;
                    }
                }
            }
        }
    }

    showWindow(fb, w, h);
}
