#include "cameraController.h"

#include <utility>

#define PI 3.1415926535

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

CameraController::CameraController(Camera& camera) : speed(3.0), sensitivity(0.003f), yaw(0), pitch(0), camera(camera) {}

void CameraController::setFace(const Vec3d& face) {
    auto res = anglesFromForward(face);
    yaw = res.first;
    pitch = res.second;
}

void CameraController::updateLook() {
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

void CameraController::updateMove(float dt) {
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
