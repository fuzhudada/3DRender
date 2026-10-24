#include "renderer.h"

#include <iostream>

struct tempTriangleInfo {
    Vec4d a, b, c;
    Vec3d wa, wb, wc;
    Vec3d na, nb, nc;
};

struct tempPoint {
    Vec4d a;
    Vec3d wa, na;
};

static void clip(const tempTriangleInfo& t, std::vector<tempTriangleInfo>& res, float n) {
    std::vector<tempPoint> in, out, mid;
    if(t.a.z >= -n){
        in.push_back({t.a, t.wa, t.na});
    }else{
        out.push_back({t.a, t.wa, t.na});
    }

    if(t.b.z >= -n){
        in.push_back({t.b, t.wb, t.nb});
    }else{
        out.push_back({t.b, t.wb, t.nb});
    }

    if(t.c.z >= -n){
        in.push_back({t.c, t.wc, t.nc});
    }else{
        out.push_back({t.c, t.wc, t.nc});
    }

    for(int i=0;i<in.size();i++){
        for(int j=0;j<out.size();j++){
            float t = (n + out[j].a.z) / (out[j].a.z - in[i].a.z);
            float xp = in[i].a.x * t + out[j].a.x * (1 - t);
            float yp = in[i].a.y * t + out[j].a.y * (1 - t);
            Vec3d wp = in[i].wa * t + out[j].wa * (1 - t);
            Vec3d np = in[i].na * t + out[j].na * (1 - t);
            np = np.normalized();
            mid.push_back({{xp, yp, -n, n}, wp, np});
        }
    }

    if(in.size() == 1){
        res.push_back({in[0].a, mid[0].a, mid[1].a, in[0].wa, mid[0].wa, mid[1].wa, in[0].na, mid[0].na, mid[1].na});
    }else if(in.size() == 2){
        res.push_back({in[0].a, in[1].a, mid[0].a, in[0].wa, in[1].wa, mid[0].wa, in[0].na, in[1].na, mid[0].na});
        res.push_back({in[1].a, mid[0].a, mid[1].a, in[1].wa, mid[0].wa, mid[1].wa, in[1].na, mid[0].na, mid[1].na});
    }else if(in.size() == 3){
        res.push_back({in[0].a, in[1].a, in[2].a, in[0].wa, in[1].wa, in[2].wa, in[0].na, in[1].na, in[2].na});
    }
}

static bool isIn(int x, int y, float a, const TriangleInfo& t, float &w1, float &w2, float &w3) {
    if(std::fabs(a) < 1e-9) return false;

    float fx = x + 0.5f;
    float fy = y + 0.5f;

    w1 = ((t.c.x - t.b.x) * (fy - t.b.y) - (t.c.y - t.b.y) * (fx - t.b.x)) / a;
    w2 = ((t.a.x - t.c.x) * (fy - t.c.y) - (t.a.y - t.c.y) * (fx - t.c.x)) / a;
    w3 = ((t.b.x - t.a.x) * (fy - t.a.y) - (t.b.y - t.a.y) * (fx - t.a.x)) / a;
    return w1 >= 0 && w2 >= 0 && w3 >= 0;
}

static uint32_t shade(uint32_t color, float l) {
    int a = 255;
    int r = (color >> 16) & a;
    int g = (color >> 8) & a;
    int b = color & a;
    r = (int)(l * r);
    g = (int)(l * g);
    b = (int)(l * b);
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

Vec4d Renderer::ndcToScreen(const Vec4d& v) const {
    return {(v.x + 1) * w / 2, (1 - v.y) * h / 2, v.z, v.w};
}

void Renderer::triangleTransform(const Triangle& t, std::vector<TriangleInfo>& out) const {
    Vec4d v4a = {t.a.x, t.a.y, t.a.z, 1};
    Vec4d v4b = {t.b.x, t.b.y, t.b.z, 1};
    Vec4d v4c = {t.c.x, t.c.y, t.c.z, 1};

    v4a = proj * (camera * v4a);
    v4b = proj * (camera * v4b);
    v4c = proj * (camera * v4c);

    tempTriangleInfo tti = {v4a, v4b, v4c, t.a, t.b, t.c, t.na, t.nb, t.nc};
    std::vector<tempTriangleInfo> clipedTriangles;
    clip(tti, clipedTriangles, proj.getNear());

    for(int i=0;i<clipedTriangles.size();i++){
        tempTriangleInfo tti = clipedTriangles[i];
        v4a = (1 / tti.a.w) * tti.a;
        v4b = (1 / tti.b.w) * tti.b;
        v4c = (1 / tti.c.w) * tti.c;
        v4a = ndcToScreen(v4a);
        v4b = ndcToScreen(v4b);
        v4c = ndcToScreen(v4c);
        TriangleInfo t1 = {v4a.xyz(), v4b.xyz(), v4c.xyz(), tti.wa, tti.wb, tti.wc, tti.na, tti.nb, tti.nc, t.color};
        out.push_back(t1);
    }
}

Renderer::Renderer(std::vector<Triangle>& triangles, Camera& camera, Projection& proj, Light& light) : triangles(triangles), camera(camera), proj(proj), light(light) {}

void Renderer::resize(int w, int h) {
    this->w = w;
    this->h = h;
}

void Renderer::paint(std::vector<uint32_t>& fb) {
    fb.assign(w * h, 0);
    z_buffer.assign(w * h, 2.0f);

    Vec3d eye = camera.getEye();
    for(const auto& t1 : triangles) {
        //float l = light.FlatLambert(t1);
        //float l = light.FlatPhong(t1, eye);
        //Vec3d l1 = light.GouraudPhong(t1, eye);
        std::vector<TriangleInfo> res;
        triangleTransform(t1, res);

        for(int i=0;i<res.size();i++){
            TriangleInfo t = res[i];
            float minx = std::min(std::min(t.a.x, t.b.x), t.c.x);
            float maxx = std::max(std::max(t.a.x, t.b.x), t.c.x);
            float miny = std::min(std::min(t.a.y, t.b.y), t.c.y);
            float maxy = std::max(std::max(t.a.y, t.b.y), t.c.y);

            minx = std::max(0.0f, minx);
            maxx = std::min((float)(w - 1), maxx);
            miny = std::max(0.0f, miny);
            maxy = std::min((float)(h - 1), maxy);

            float a = (t.b.x - t.a.x) * (t.c.y - t.a.y) - (t.b.y - t.a.y) * (t.c.x - t.a.x);
            if(a >= 0) continue;
            for(int i = miny; i <= maxy; ++i) {
                for(int j = minx; j <= maxx; ++j) {
                    float w1, w2, w3;
                    if(isIn(j, i, a, t, w1, w2, w3)) {
                        float depth = w1 * t.a.z + w2 * t.b.z + w3 * t.c.z;
                        Vec3d wp = w1 * t.wa + w2 * t.wb + w3 * t.wc;
                        Vec3d np = w1 * t.na + w2 * t.nb + w3 * t.nc;
                        np = np.normalized();
                        float l = light.getLightOnCenter(np, wp, camera.getEye());

                        if(z_buffer[i * w + j] > depth) {
                            z_buffer[i * w + j] = depth;
                            fb[i * w + j] = shade(t.color, l);
                        }
                    }
                }
            }
        }
    }
}
