#include "src/app/application.h"

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <utility>

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

int main() {
    Application app;

    const float PI = 3.14159265358979f;

    // ---------- 整体缩放 & 深度锚点 ----------
    const float S  = 0.85f;
    const float AZ = -4.50f;

    auto tx = [&](float x) { return S * x; };
    auto ty = [&](float y) { return S * y; };
    auto tz = [&](float z) { return AZ + S * (z - AZ); };

    // ================================================================
    // 椭球生成器：每个顶点带解析外法线，支持平滑光照
    // ================================================================
    auto addEllipsoid = [&](float cx, float cy, float cz,
                            float rx, float ry, float rz,
                            int slices, int stacks,
                            uint32_t col) {
        float scx = tx(cx), scy = ty(cy), scz = tz(cz);
        float srx = rx * S, sry = ry * S, srz = rz * S;

        struct PN { float x, y, z, nx, ny, nz; };

        auto P = [&](int i, int j) -> PN {
            float phi   = -PI * 0.5f + PI * (float)i / (float)stacks;
            float theta = 2.0f * PI * (float)j / (float)slices;
            float cp = std::cos(phi), sp = std::sin(phi);
            float ct = std::cos(theta), st = std::sin(theta);
            PN p;
            p.x = scx + srx * cp * ct;
            p.y = scy + sry * sp;
            p.z = scz + srz * cp * st;

            // 椭球外法线（梯度方向，归一化）
            float nx = (p.x - scx) / (srx * srx);
            float ny = (p.y - scy) / (sry * sry);
            float nz = (p.z - scz) / (srz * srz);
            float nl = std::sqrt(nx*nx + ny*ny + nz*nz);
            if (nl < 1e-8f) nl = 1.0f;
            p.nx = nx / nl; p.ny = ny / nl; p.nz = nz / nl;
            return p;
        };

        auto emit = [&](PN a, PN b, PN c) {
            // 面法线仅用于判断绕序是否朝外
            float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z;
            float vx = c.x - a.x, vy = c.y - a.y, vz = c.z - a.z;
            float fnx = uy * vz - uz * vy;
            float fny = uz * vx - ux * vz;
            float fnz = ux * vy - uy * vx;

            float mx = (a.x + b.x + c.x) / 3.0f - scx;
            float my = (a.y + b.y + c.y) / 3.0f - scy;
            float mz = (a.z + b.z + c.z) / 3.0f - scz;

            if (fnx * mx + fny * my + fnz * mz < 0.0f) {
                std::swap(b, c);   // 同步交换位置和法线
            }

            app.addTriangle(Triangle(
                { a.x, a.y, a.z },
                { b.x, b.y, b.z },
                { c.x, c.y, c.z },
                { a.nx, a.ny, a.nz },
                { b.nx, b.ny, b.nz },
                { c.nx, c.ny, c.nz },
                col
            ));
        };

        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                PN a = P(i,     j);
                PN b = P(i,     j + 1);
                PN c = P(i + 1, j + 1);
                PN d = P(i + 1, j);
                if (i > 0)          emit(a, b, c);
                if (i < stacks - 1) emit(a, c, d);
            }
        }
    };

    // ================================================================
    // 头部几何参数（嘴巴贴花复用）
    // ================================================================
    const float HEAD_CX = 0.00f, HEAD_CY = 0.35f, HEAD_CZ = -4.55f;
    const float HEAD_RX = 1.30f, HEAD_RY = 0.95f, HEAD_RZ = 1.10f;

    auto headSurfZ = [&](float x, float y) {
        float dx = (x - HEAD_CX) / HEAD_RX;
        float dy = (y - HEAD_CY) / HEAD_RY;
        float s  = 1.0f - dx*dx - dy*dy;
        if (s < 0.01f) s = 0.01f;
        return HEAD_CZ + HEAD_RZ * std::sqrt(s);
    };
    auto headNormal = [&](float x, float y) -> Vec3d {
        float z = headSurfZ(x, y);
        float nx = (x - HEAD_CX) / (HEAD_RX * HEAD_RX);
        float ny = (y - HEAD_CY) / (HEAD_RY * HEAD_RY);
        float nz = (z - HEAD_CZ) / (HEAD_RZ * HEAD_RZ);
        float nl = std::sqrt(nx*nx + ny*ny + nz*nz);
        if (nl < 1e-8f) nl = 1.0f;
        return Vec3d{ nx/nl, ny/nl, nz/nl };
    };

    // ================================================================
    // 配色
    // ================================================================
    const uint32_t SKIN      = rgba(122, 200,  96);
    const uint32_t SKIN_DARK = rgba( 95, 170,  76);
    const uint32_t WHITE     = rgba(255, 255, 255);
    const uint32_t BLACK     = rgba( 32,  32,  32);
    const uint32_t MOUTH     = rgba( 62, 110,  52);
    const uint32_t NOSTRIL   = rgba( 72, 130,  58);
    const uint32_t BLUSH     = rgba(255, 158, 174);

    // ================================================================
    // 立体青蛙
    // ================================================================

    // 1) 身体
    addEllipsoid( 0.00f, -1.15f, -4.75f,  1.15f, 0.85f, 1.00f, 14, 10, SKIN);

    // 2) 头
    addEllipsoid(HEAD_CX, HEAD_CY, HEAD_CZ, HEAD_RX, HEAD_RY, HEAD_RZ, 16, 12, SKIN);

    // 3) 眼睛鼓包
    addEllipsoid(-0.62f,  1.08f, -4.05f,  0.46f, 0.46f, 0.46f, 12,  8, SKIN_DARK);
    addEllipsoid( 0.62f,  1.08f, -4.05f,  0.46f, 0.46f, 0.46f, 12,  8, SKIN_DARK);

    // 4) 眼白
    addEllipsoid(-0.62f,  1.10f, -3.82f,  0.34f, 0.34f, 0.34f, 12,  8, WHITE);
    addEllipsoid( 0.62f,  1.10f, -3.82f,  0.34f, 0.34f, 0.34f, 12,  8, WHITE);

    // 5) 瞳孔
    addEllipsoid(-0.66f,  1.08f, -3.60f,  0.20f, 0.20f, 0.20f, 10,  6, BLACK);
    addEllipsoid( 0.66f,  1.08f, -3.60f,  0.20f, 0.20f, 0.20f, 10,  6, BLACK);

    // 6) 高光
    addEllipsoid(-0.74f,  1.18f, -3.47f,  0.075f, 0.075f, 0.075f, 6, 4, WHITE);
    addEllipsoid( 0.58f,  1.18f, -3.47f,  0.075f, 0.075f, 0.075f, 6, 4, WHITE);

    // 7) 鼻孔
    addEllipsoid(-0.20f,  0.55f, -3.46f,  0.055f, 0.045f, 0.055f, 6, 4, NOSTRIL);
    addEllipsoid( 0.20f,  0.55f, -3.46f,  0.055f, 0.045f, 0.055f, 6, 4, NOSTRIL);

    // 8) 腮红
    addEllipsoid(-0.85f, -0.05f, -3.83f,  0.24f, 0.17f, 0.08f, 10, 6, BLUSH);
    addEllipsoid( 0.85f, -0.05f, -3.83f,  0.24f, 0.17f, 0.08f, 10, 6, BLUSH);

    // 9) 前脚
    addEllipsoid(-0.55f, -1.95f, -4.05f,  0.40f, 0.28f, 0.50f, 10, 6, SKIN_DARK);
    addEllipsoid( 0.55f, -1.95f, -4.05f,  0.40f, 0.28f, 0.50f, 10, 6, SKIN_DARK);

    // ================================================================
    // 10) 嘴巴：贴在头部前表面的弧形条带
    //     每个顶点使用头部椭球表面法线，跟随脸部光照
    // ================================================================
    {
        const int   SEG = 18;
        const float X0  = -0.50f;
        const float X1  =  0.50f;
        const float TH  =  0.06f;

        auto mouthTopY = [&](float x) {
            float t = x / 0.50f;
            return -0.10f - 0.28f * (1.0f - t * t);
        };

        for (int i = 0; i < SEG; ++i) {
            float xa = X0 + (X1 - X0) * (float)i       / (float)SEG;
            float xb = X0 + (X1 - X0) * (float)(i + 1) / (float)SEG;
            float ya = mouthTopY(xa);
            float yb = mouthTopY(xb);

            float za1 = headSurfZ(xa, ya)      + 0.03f;
            float zb1 = headSurfZ(xb, yb)      + 0.03f;
            float za2 = headSurfZ(xa, ya - TH) + 0.03f;
            float zb2 = headSurfZ(xb, yb - TH) + 0.03f;

            Vec3d nA1 = headNormal(xa, ya);
            Vec3d nB1 = headNormal(xb, yb);
            Vec3d nA2 = headNormal(xa, ya - TH);
            Vec3d nB2 = headNormal(xb, yb - TH);

            app.addTriangle(Triangle(
                { tx(xa), ty(ya),      tz(za1) },
                { tx(xb), ty(yb - TH), tz(zb2) },
                { tx(xb), ty(yb),      tz(zb1) },
                nA1, nB2, nB1,
                MOUTH));

            app.addTriangle(Triangle(
                { tx(xa), ty(ya),      tz(za1) },
                { tx(xa), ty(ya - TH), tz(za2) },
                { tx(xb), ty(yb - TH), tz(zb2) },
                nA1, nA2, nB2,
                MOUTH));
        }
    }

    app.paint();
    return 0;
}
