#pragma once

#include "src/math/vec3d.h"
#include "display.h"
#include "src/render/camera.h"

#include <windows.h>

class CameraController {
private:
    Camera& camera;

    float speed, sensitivity;
    float yaw, pitch;

public:
    CameraController(Camera& camera);

    float getSpeed() const { return speed; }
    float getSensitivity() const { return sensitivity; }

    void setSpeed(float speed) { this->speed = speed; }
    void setSensitivity(float sensitivity) { this->sensitivity = sensitivity; }

    void setFace(const Vec3d& face);

    void updateLook();
    void updateMove(float dt);
};
