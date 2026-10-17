#include "3DRender.h"
#include "text.h"

#include <vector>
#include <math.h>
#include <chrono>
#include <windows.h>
#include <iostream>

#define PI 3.1415926535

struct tempTriangle {
    Vec4d a, b, c;
};

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

static std::vector<tempTriangle> clip(const tempTriangle& t, float n) {
    std::vector<Vec4d> in, out, mid;
    std::vector<tempTriangle> res;
    if(t.a.z >= -n){
        in.push_back(t.a);
    }else{
        out.push_back(t.a);
    }

    if(t.b.z >= -n){
        in.push_back(t.b);
    }else{
        out.push_back(t.b);
    }

    if(t.c.z >= -n){
        in.push_back(t.c);
    }else{
        out.push_back(t.c);
    }

    for(int i=0;i<in.size();i++){
        for(int j=0;j<out.size();j++){
            float t = (n + out[j].z) / (out[j].z - in[i].z);
            float xp = in[i].x * t + out[j].x * (1 - t);
            float yp = in[i].y * t + out[j].y * (1 - t);
            mid.push_back({xp, yp, -n, n});
        }
    }

    if(in.size() == 1){
        res.push_back({in[0], mid[0], mid[1]});
    }else if(in.size() == 2){
        res.push_back({in[0], in[1], mid[0]});
        res.push_back({in[1], mid[0], mid[1]});
    }else if(in.size() == 3){
        res.push_back({in[0], in[1], in[2]});
    }
    return res;
}

static bool isIn(int x, int y, float a, const Triangle& t, float &w1, float &w2, float &w3) {
    if(std::fabs(a) < 1e-9) return false;

    float fx = x + 0.5f;
    float fy = y + 0.5f;

    w1 = ((t.c.x - t.b.x) * (fy - t.b.y) - (t.c.y - t.b.y) * (fx - t.b.x)) / a;
    w2 = ((t.a.x - t.c.x) * (fy - t.c.y) - (t.a.y - t.c.y) * (fx - t.c.x)) / a;
    w3 = ((t.b.x - t.a.x) * (fy - t.a.y) - (t.b.y - t.a.y) * (fx - t.a.x)) / a;
    return w1 >= 0 && w2 >= 0 && w3 >= 0;
}

static Vec3d forwardFromAngles(float yaw, float pitch) {
    float x = std::sin(yaw);
    float y = std::tan(pitch);
    float z = -std::cos(yaw);
    Vec3d v = {x, y, z};
    return v.normalized();
}

static std::pair<float, float> anglesFromForward(const Vec3d& v) {
    float x = v.x;
    float y = v.y;
    float z = v.z;
    float d = std::sqrt(x * x + z * z);

    float yaw;
    if(std::fabs(z) > 1e-9){
        yaw = std::atan(-x / z);
        if(z > 0) yaw += PI;
        if(yaw < 0) yaw += 2 * PI;
    }else{
        yaw = x > 0 ? PI / 2 : PI * 3 / 2;
    }

    float pitch = std::atan(y / d);
    return {yaw, pitch};
}

static std::string getFpsStr(int frame, float time) {
    return "FPS: " + std::to_string(1.0f * frame / time);
}

static void drawCrosshair(std::vector<uint32_t>& fb, int w, int h) {
    if(w < 25 || h < 25) return;
    int cx = w / 2, cy = h / 2;
    for(int i = cx - 11; i <= cx + 10; i++){
        fb[(cy - 1) * w + i] = rgba(255, 255, 255);
        fb[cy * w + i] = rgba(255, 255, 255);
    }
    for(int j = cy - 11; j <= cy + 10; j++){
        fb[j * w + cx - 1] = rgba(255, 255, 255);
        fb[j * w + cx] = rgba(255, 255, 255);
    }
}

Renderer::Renderer() : w(800), h(600), speed(3.0), sensitivity(0.003f), yaw(0), pitch(0) {}

Renderer::Renderer(const Camera& c, const Projection& p, float speed, float sensitivity) : camera(c), proj(p), w(p.getWidth()), h(p.getHeight()), speed(speed), sensitivity(sensitivity) {
    auto res = anglesFromForward(camera.getFace());
    yaw = res.first;
    pitch = res.second;
}

void Renderer::addTriangle(const Triangle& t) {
    triangles.push_back(t);
}

void Renderer::clearTriangles() {
    triangles.clear();
}

void Renderer::setCamera(const Camera& c) {
    this->camera = c;
    auto res = anglesFromForward(camera.getFace());
    yaw = res.first;
    pitch = res.second;
}

Vec4d Renderer::ndcToScreen(const Vec4d& v) const {
    return {(v.x + 1) * w / 2, (1 - v.y) * h / 2, v.z, v.w};
}

void Renderer::triangleTransform(const Triangle& t, std::vector<Triangle>& out) const {
    Vec4d v4a = {t.a.x, t.a.y, t.a.z, 1};
    Vec4d v4b = {t.b.x, t.b.y, t.b.z, 1};
    Vec4d v4c = {t.c.x, t.c.y, t.c.z, 1};

    v4a = proj * (camera * v4a);
    v4b = proj * (camera * v4b);
    v4c = proj * (camera * v4c);

    auto clipedTriangles = clip({v4a, v4b, v4c}, proj.getNear());

    for(int i=0;i<clipedTriangles.size();i++){
        v4a = (1 / clipedTriangles[i].a.w) * clipedTriangles[i].a;
        v4b = (1 / clipedTriangles[i].b.w) * clipedTriangles[i].b;
        v4c = (1 / clipedTriangles[i].c.w) * clipedTriangles[i].c;
        v4a = ndcToScreen(v4a);
        v4b = ndcToScreen(v4b);
        v4c = ndcToScreen(v4c);
        Triangle t1(v4a.xyz(), v4b.xyz(), v4c.xyz(), t.color);
        out.push_back(t1);
    }
}

void Renderer::getFb() {
    fb.assign(w * h, 0);
    z_buffer.assign(w * h, 2.0f);

    for(const auto& t1 : triangles) {
        std::vector<Triangle> res;
        triangleTransform(t1, res);

        for(int i=0;i<res.size();i++){
            Triangle t = res[i];
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

                        if(z_buffer[i * w + j] > depth) {
                            z_buffer[i * w + j] = depth;
                            fb[i * w + j] = t.color;
                        }
                    }
                }
            }
        }
    }
}

void Renderer::updateLook(float dt) {
    float dx, dy;
    getMouseDelta(dx, dy);

    yaw += dx * sensitivity;
    pitch -= dy * sensitivity;

    if(yaw < 0) yaw += 2 * PI;
    if(yaw > 2 * PI) yaw -= 2 * PI;

    if(pitch < -PI / 2) pitch = -PI / 2 + 0.01f;
    if(pitch > PI / 2) pitch = PI / 2 - 0.01f;

    camera.setFace(forwardFromAngles(yaw, pitch));
}

void Renderer::updateMove(float dt) {
    Vec3d eye = camera.getEye();

    float step = speed * dt;
    Vec3d forwardH = camera.getFace();
    Vec3d rightH = camera.getSide();

    forwardH = {forwardH.x, 0, forwardH.z};
    forwardH = forwardH.normalized();

    if (getKeyDown('W'))     eye += forwardH * step;
    if (getKeyDown('S'))     eye -= forwardH * step;
    if (getKeyDown('A'))     eye -= rightH   * step;
    if (getKeyDown('D'))     eye += rightH   * step;
    if (getKeyDown(VK_SPACE)) eye.y += step;
    if (getKeyDown(VK_SHIFT)) eye.y -= step;

    camera.setEye(eye);
}

void Renderer::mainLoop() {
    auto last = std::chrono::steady_clock::now();
    float cooldownE = 0;
    float cooldownFPS = 0;
    int frame = 0;
    std::string FpsStr;

    while(isOpen()) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;

        pollEvents();

        updateLook(dt);
        updateMove(dt);

        if (getKeyDown('E') && cooldownE <= 0){
            setMouseCaptured(!getMouseCaptured());
            cooldownE = 0.3f;
        }
        if(cooldownE > 0) cooldownE -= dt;

        if (getKeyDown(VK_ESCAPE)) exit();

        if(w != getWidth() || h != getHeight()){
            w = getWidth();
            h = getHeight();
            proj.setWidthAndHeight(w, h);
        }

        frame++;
        if(cooldownFPS <= 0){
            FpsStr = getFpsStr(frame, 1.0 - cooldownFPS);
            frame = 0;
            cooldownFPS = 1.0f;
        }
        cooldownFPS -= dt;

        getFb();
        drawText(fb, w, h, 20, 20, FpsStr, rgba(0, 255, 0), 3);
        drawCrosshair(fb, w, h);
        updateWindow(fb, w, h);
    }
}

void Renderer::paint() {
    getFb();
    showWindow(fb, w, h);

    mainLoop();
}
