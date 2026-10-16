#include "src/render/3DRender.h"

#include <vector>
#include <cstdint>

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

int main() {
    Renderer renderer;
    renderer.getCamera().setEye({4,4,-2.5});
    renderer.getCamera().setFace({-1,-1,0});

    // 近面 (z = -3)，红色
    Triangle t0 (0,  {0,0,-3}, {2,0,-3}, {2,2,-3}, rgba(200, 50,  50));
    Triangle t1 (1,  {0,0,-3}, {2,2,-3}, {0,2,-3}, rgba(200, 50,  50));

    // 远面 (z = -5)，绿色
    Triangle t2 (2,  {0,0,-5}, {0,2,-5}, {2,2,-5}, rgba(50,  200, 50));
    Triangle t3 (3,  {0,0,-5}, {2,2,-5}, {2,0,-5}, rgba(50,  200, 50));

    // 左面 (x = 0)，蓝色
    Triangle t4 (4,  {0,0,-3}, {0,2,-3}, {0,2,-5}, rgba(50,  50,  200));
    Triangle t5 (5,  {0,0,-3}, {0,2,-5}, {0,0,-5}, rgba(50,  50,  200));

    // 右面 (x = 2)，黄色
    Triangle t6 (6,  {2,0,-5}, {2,2,-5}, {2,2,-3}, rgba(200, 200, 50));
    Triangle t7 (7,  {2,0,-5}, {2,2,-3}, {2,0,-3}, rgba(200, 200, 50));

    // 底面 (y = 0)，品红
    Triangle t8 (8,  {0,0,-5}, {2,0,-5}, {2,0,-3}, rgba(200, 50,  200));
    Triangle t9 (9,  {0,0,-5}, {2,0,-3}, {0,0,-3}, rgba(200, 50,  200));

    // 顶面 (y = 2)，青色
    Triangle t10(10, {0,2,-3}, {2,2,-3}, {2,2,-5}, rgba(50,  200, 200));
    Triangle t11(11, {0,2,-3}, {2,2,-5}, {0,2,-5}, rgba(50,  200, 200));

    // ---------- 加入渲染器 ----------
    renderer.addTriangle(t0);
    renderer.addTriangle(t1);
    renderer.addTriangle(t2);
    renderer.addTriangle(t3);
    renderer.addTriangle(t4);
    renderer.addTriangle(t5);
    renderer.addTriangle(t6);
    renderer.addTriangle(t7);
    renderer.addTriangle(t8);
    renderer.addTriangle(t9);
    renderer.addTriangle(t10);
    renderer.addTriangle(t11);

    renderer.paint();

    return 0;
}
