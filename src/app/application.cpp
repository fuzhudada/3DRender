#include "application.h"
#include "src/app/overlay.h"

#include <chrono>
#include <iostream>

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

Application::Application() : w(800), h(600), renderer(triangles, camera, proj, light), con(camera) {}

Application::Application(const Camera& c, const Projection& p, float speed, float sensitivity) : camera(c), proj(p), w(p.getWidth()), h(p.getHeight()), renderer(triangles, camera, proj, light), con(camera) {
    renderer.resize(w, h);
    con.setSpeed(speed);
    con.setSensitivity(sensitivity);
    con.setFace(camera.getFace());
}

void Application::addTriangle(const Triangle& t) {
    triangles.push_back(t);
}

void Application::clearTriangles() {
    triangles.clear();
}

void Application::setCamera(const Camera& c) {
    this->camera = c;
    con.setFace(camera.getFace());
}

void Application::mainLoop() {
    auto last = std::chrono::steady_clock::now();
    float cooldownE = 0;
    float cooldownR = 0;
    float cooldownF = 0;
    float cooldownFPS = 1.0f;
    int frame = 0;
    float fps = 0;

    while(isOpen()) {
        auto now = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;

        pollEvents();

        con.updateLook();
        con.updateMove(dt);

        if (getKeyDown('E') && cooldownE <= 0){
            setMouseCaptured(!getMouseCaptured());
            cooldownE = 0.3f;
        }
        if(cooldownE > 0) cooldownE -= dt;

        if (getKeyDown('R') && cooldownR <= 0){
            light.setMode(light.getMode() == LightMode::Directional ? LightMode::Point : LightMode::Directional);
            cooldownR = 0.3f;
        }
        if(cooldownR > 0) cooldownR -= dt;

        if (getKeyDown('F') && cooldownF <= 0){
            lightFollowsCamera = !lightFollowsCamera;
            cooldownF = 0.3f;
        }
        if(cooldownF > 0) cooldownF -= dt;

        if(lightFollowsCamera){
            if(light.getMode() == LightMode::Directional){
                light.setSource(-camera.getFace());
            }else{
                light.setSource(camera.getEye());
            }
        }

        if (getKeyDown(VK_ESCAPE)) exit();

        if(w != getWidth() || h != getHeight()){
            w = getWidth();
            h = getHeight();
            proj.setWidthAndHeight(w, h);
            renderer.resize(w, h);
        }

        frame++;
        if(cooldownFPS <= 0){
            fps = 1.0f * frame / (1.0f - cooldownFPS);
            frame = 0;
            cooldownFPS = 1.0f;
        }
        cooldownFPS -= dt;

        renderer.paint(fb);
        drawFpsStr(fb, w, h, fps);
        drawCrosshair(fb, w, h);
        updateWindow(fb, w, h);
    }
}

void Application::paint() {
    showWindow(fb, w, h);
    light.setShininess(32);
    mainLoop();
}
