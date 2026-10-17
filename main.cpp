#include "src/render/3DRender.h"

#include <cmath>
#include <cstdint>
#include <algorithm>
#include <utility>

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

static uint32_t shadeColor(uint32_t c, float k) {
    int a = int((c >> 24) & 0xFF);
    int r = int((c >> 16) & 0xFF);
    int g = int((c >>  8) & 0xFF);
    int b = int( c        & 0xFF);
    r = std::min(255, int(r * k + 0.5f));
    g = std::min(255, int(g * k + 0.5f));
    b = std::min(255, int(b * k + 0.5f));
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

int main() {
    Renderer renderer;

    const float PI = 3.14159265358979f;

    const float S  = 0.80f;
    const float AZ = -4.50f;

    auto tx = [&](float x) { return S * x; };
    auto ty = [&](float y) { return S * y; };
    auto tz = [&](float z) { return AZ + S * (z - AZ); };

    float Lx = 0.45f, Ly = 0.80f, Lz = 0.55f;
    {
        float l = std::sqrt(Lx*Lx + Ly*Ly + Lz*Lz);
        Lx /= l; Ly /= l; Lz /= l;
    }

    // ---------- 椭球生成器 ----------
    auto addEllipsoid = [&](float cx, float cy, float cz,
                            float rx, float ry, float rz,
                            int slices, int stacks,
                            uint32_t col) {
        cx = tx(cx); cy = ty(cy); cz = tz(cz);
        rx *= S;     ry *= S;     rz *= S;

        struct P3 { float x, y, z; };
        auto P = [&](int i, int j) -> P3 {
            float phi   = -PI * 0.5f + PI * (float)i / (float)stacks;
            float theta = 2.0f * PI * (float)j / (float)slices;
            float cp = std::cos(phi), sp = std::sin(phi);
            float ct = std::cos(theta), st = std::sin(theta);
            return { cx + rx * cp * ct,
                     cy + ry * sp,
                     cz + rz * cp * st };
        };

        auto emit = [&](P3 a, P3 b, P3 c) {
            float ux = b.x - a.x, uy = b.y - a.y, uz = b.z - a.z;
            float vx = c.x - a.x, vy = c.y - a.y, vz = c.z - a.z;
            float nx = uy * vz - uz * vy;
            float ny = uz * vx - ux * vz;
            float nz = ux * vy - uy * vx;

            float mx = (a.x + b.x + c.x) / 3.0f - cx;
            float my = (a.y + b.y + c.y) / 3.0f - cy;
            float mz = (a.z + b.z + c.z) / 3.0f - cz;

            if (nx * mx + ny * my + nz * mz < 0.0f) {
                std::swap(b, c);
                nx = -nx; ny = -ny; nz = -nz;
            }
            float len = std::sqrt(nx*nx + ny*ny + nz*nz);
            if (len < 1e-8f) return;
            nx /= len; ny /= len; nz /= len;

            float d = nx * Lx + ny * Ly + nz * Lz;
            if (d < 0.0f) d = 0.0f;
            float k = 0.55f + 0.65f * d;
            renderer.addTriangle(Triangle(
                { a.x, a.y, a.z },
                { b.x, b.y, b.z },
                { c.x, c.y, c.z },
                shadeColor(col, k)
            ));
        };

        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                P3 a = P(i,     j);
                P3 b = P(i,     j + 1);
                P3 c = P(i + 1, j + 1);
                P3 d = P(i + 1, j);
                if (i > 0)          emit(a, b, c);
                if (i < stacks - 1) emit(a, c, d);
            }
        }
    };

    // ---------- 头部几何参数（统一用常量，方便日后微调） ----------
    const float HEAD_CY = 0.40f;    // 头中心 y（原 0.55，下移 0.15）
    const float HEAD_CZ = -4.60f;
    const float HEAD_RX = 1.05f;
    const float HEAD_RY = 0.90f;
    const float HEAD_RZ = 1.00f;

    // 头部表面 z（脸部贴花用）
    auto headSurfZ = [&](float x, float y) {
        float dx = x / HEAD_RX;
        float dy = (y - HEAD_CY) / HEAD_RY;
        float s  = 1.0f - dx * dx - dy * dy;
        if (s < 0.01f) s = 0.01f;
        return HEAD_CZ + HEAD_RZ * std::sqrt(s);
    };

    auto addFaceDecal = [&](float cx, float cy,
                            float rx, float ry, int seg,
                            float eps, uint32_t col) {
        float zc = headSurfZ(cx, cy) + eps;
        for (int i = 0; i < seg; ++i) {
            float a0 =  2.0f * PI * (float)i       / (float)seg;
            float a1 =  2.0f * PI * (float)(i + 1) / (float)seg;
            float x0 = cx + rx * std::cos(a0), y0 = cy + ry * std::sin(a0);
            float x1 = cx + rx * std::cos(a1), y1 = cy + ry * std::sin(a1);
            float z0 = headSurfZ(x0, y0) + eps;
            float z1 = headSurfZ(x1, y1) + eps;
            renderer.addTriangle(Triangle(
                { tx(cx), ty(cy), tz(zc) },
                { tx(x0), ty(y0), tz(z0) },
                { tx(x1), ty(y1), tz(z1) },
                col));
        }
    };

    // 配色
    const uint32_t SKIN      = rgba(255, 216, 112);
    const uint32_t BELLY     = rgba(255, 243, 200);
    const uint32_t WHITE     = rgba(255, 255, 255);
    const uint32_t EYE_GREEN = rgba( 80, 180,  90);
    const uint32_t BLACK     = rgba( 35,  35,  35);
    const uint32_t MOUTH     = rgba(120,  85,  50);
    const uint32_t SPIKE     = rgba(235, 170,  65);
    const uint32_t CLAW      = rgba( 45,  40,  35);

    // ================================================================
    // 身体（宽:高 = 1:1.1）
    // ================================================================
    const float BODY_CX = 0.00f, BODY_CY = -1.23f, BODY_CZ = -4.85f;
    const float BODY_RX = 1.20f, BODY_RY = 1.32f, BODY_RZ = 1.05f;

    addEllipsoid(BODY_CX, BODY_CY, BODY_CZ, BODY_RX, BODY_RY, BODY_RZ,
                 48, 32, SKIN);

    // ================================================================
    // 肚皮贴花：贴合身体前表面，占身体高度的下方 2/3 略少一点
    //   phi_c = -0.45, phi_r = 0.65 → 上边 y≈-0.97, 下边 y≈-2.41
    //   theta_r = 1.00 → 前方约 115° 覆盖
    // ================================================================
    {
        float bCx = tx(BODY_CX), bCy = ty(BODY_CY), bCz = tz(BODY_CZ);
        float bRx = BODY_RX * S, bRy = BODY_RY * S, bRz = BODY_RZ * S;

        auto bodySurfPt = [&](float phi, float theta, float eps) {
            float cp = std::cos(phi), sp = std::sin(phi);
            float ct = std::cos(theta), st = std::sin(theta);
            float lx = bRx * cp * ct;
            float ly = bRy * sp;
            float lz = bRz * cp * st;
            float nx = lx / (bRx * bRx);
            float ny = ly / (bRy * bRy);
            float nz = lz / (bRz * bRz);
            float nl = std::sqrt(nx*nx + ny*ny + nz*nz);
            if (nl < 1e-8f) nl = 1.0f;
            return Vec3d{ bCx + lx + eps*nx/nl,
                                   bCy + ly + eps*ny/nl,
                                   bCz + lz + eps*nz/nl };
        };

        const float phi_c   = -0.45f;
        const float phi_r   =  0.65f;
        const float theta_c =  1.5708f;
        const float theta_r =  1.00f;
        const int   rings   =  20;
        const int   sectors =  48;
        const float EPS     =  0.02f;

        for (int j = 0; j < rings; ++j) {
            for (int i = 0; i < sectors; ++i) {
                float r0 = (float)j       / (float)rings;
                float r1 = (float)(j + 1) / (float)rings;
                float a0 = 2.0f * PI * (float)i       / (float)sectors;
                float a1 = 2.0f * PI * (float)(i + 1) / (float)sectors;

                auto v00 = bodySurfPt(phi_c + phi_r * r0 * std::cos(a0),
                                      theta_c + theta_r * r0 * std::sin(a0), EPS);
                auto v01 = bodySurfPt(phi_c + phi_r * r0 * std::cos(a1),
                                      theta_c + theta_r * r0 * std::sin(a1), EPS);
                auto v10 = bodySurfPt(phi_c + phi_r * r1 * std::cos(a0),
                                      theta_c + theta_r * r1 * std::sin(a0), EPS);
                auto v11 = bodySurfPt(phi_c + phi_r * r1 * std::cos(a1),
                                      theta_c + theta_r * r1 * std::sin(a1), EPS);

                if (j > 0) {
                    renderer.addTriangle(Triangle(v00, v10, v11, BELLY));
                    renderer.addTriangle(Triangle(v00, v11, v01, BELLY));
                } else {
                    renderer.addTriangle(Triangle(v00, v10, v11, BELLY));
                }
            }
        }
    }

    // ================================================================
    // 头（中心下移到 0.40，靠重叠自然填补脖子凹陷，不再需要肩部椭球）
    // ================================================================
    addEllipsoid( 0.00f, HEAD_CY, HEAD_CZ, HEAD_RX, HEAD_RY, HEAD_RZ,
                  48, 32, SKIN);

    // ================================================================
    // 眼睛（贴花）
    // ================================================================
    const float EYE_LX = -0.45f;
    const float EYE_RX =  0.45f;
    const float EYE_Y  =  0.70f;   // 原 0.85，随头下移 0.15

    addFaceDecal(EYE_LX, EYE_Y, 0.22f, 0.22f, 40, 0.030f, WHITE);
    addFaceDecal(EYE_RX, EYE_Y, 0.22f, 0.22f, 40, 0.030f, WHITE);

    addFaceDecal(EYE_LX, EYE_Y, 0.15f, 0.15f, 32, 0.045f, EYE_GREEN);
    addFaceDecal(EYE_RX, EYE_Y, 0.15f, 0.15f, 32, 0.045f, EYE_GREEN);

    addFaceDecal(EYE_LX, EYE_Y, 0.07f, 0.07f, 24, 0.060f, BLACK);
    addFaceDecal(EYE_RX, EYE_Y, 0.07f, 0.07f, 24, 0.060f, BLACK);

    addFaceDecal(EYE_LX - 0.03f, EYE_Y + 0.03f, 0.035f, 0.035f, 20, 0.075f, WHITE);
    addFaceDecal(EYE_RX - 0.03f, EYE_Y + 0.03f, 0.035f, 0.035f, 20, 0.075f, WHITE);

    // ================================================================
    // 手臂：平滑球链 + 手掌捂肚子 + 三根黑爪
    // ================================================================
    auto buildArm = [&](float sign) {
        struct Node { float x, y, z, r; };
        const Node chain[] = {
            { 1.06f, -0.95f, -4.38f, 0.30f },
            { 1.10f, -1.08f, -4.24f, 0.29f },
            { 1.04f, -1.20f, -4.10f, 0.28f },
            { 0.88f, -1.30f, -3.97f, 0.27f },
            { 0.68f, -1.35f, -3.85f, 0.26f },
        };
        for (auto& n : chain) {
            addEllipsoid(sign * n.x, n.y, n.z,
                         n.r, n.r * 0.94f, n.r * 0.98f,
                         20, 14, SKIN);
        }

        struct Claw { float x, y, z, rx, ry, rz; };
        const Claw claws[] = {
            { 0.50f, -1.55f, -3.92f, 0.055f, 0.11f, 0.055f },
            { 0.65f, -1.62f, -3.95f, 0.055f, 0.11f, 0.055f },
            { 0.80f, -1.55f, -3.92f, 0.055f, 0.11f, 0.055f },
        };
        for (auto& c : claws) {
            addEllipsoid(sign * c.x, c.y, c.z,
                         c.rx, c.ry, c.rz,
                         8, 6, CLAW);
        }
    };
    buildArm(-1.0f);
    buildArm( 1.0f);

    // ================================================================
    // 脚：本体 + 三根朝前的黑爪
    // ================================================================
    auto buildFoot = [&](float footX) {
        addEllipsoid(footX, -2.50f, -4.50f,
                     0.35f, 0.22f, 0.42f,
                     28, 18, SKIN);

        struct Claw { float dx, y, z, rz; };
        const Claw claws[] = {
            { -0.15f, -2.55f, -3.92f, 0.09f },
            {  0.00f, -2.57f, -3.88f, 0.10f },
            {  0.15f, -2.55f, -3.92f, 0.09f },
        };
        for (auto& c : claws) {
            addEllipsoid(footX + c.dx, c.y, c.z,
                         0.05f, 0.05f, c.rz,
                         8, 6, CLAW);
        }
    };
    buildFoot(-0.45f);
    buildFoot( 0.45f);

    // ================================================================
    // 尾巴
    // ================================================================
    addEllipsoid( 0.00f, -1.40f, -5.85f,  0.28f, 0.22f, 0.55f, 28, 18, SKIN);
    addEllipsoid( 0.00f, -1.15f, -6.15f,  0.18f, 0.15f, 0.30f, 24, 16, SKIN);

    // ================================================================
    // 背刺（第一个略微下移贴近新头后表面）
    // ================================================================
    addEllipsoid( 0.00f,  0.55f, -5.65f,  0.10f, 0.18f, 0.12f, 16, 12, SPIKE);
    addEllipsoid( 0.00f,  0.00f, -5.60f,  0.12f, 0.22f, 0.14f, 16, 12, SPIKE);
    addEllipsoid( 0.00f, -0.50f, -5.70f,  0.13f, 0.24f, 0.15f, 16, 12, SPIKE);
    addEllipsoid( 0.00f, -1.00f, -5.95f,  0.13f, 0.24f, 0.15f, 16, 12, SPIKE);
    addEllipsoid( 0.00f, -1.55f, -5.96f,  0.12f, 0.22f, 0.14f, 16, 12, SPIKE);
    addEllipsoid( 0.00f, -2.10f, -5.74f,  0.10f, 0.18f, 0.12f, 16, 12, SPIKE);

    // ================================================================
    // 嘴巴（整体下移 0.15）
    // ================================================================
    {
        const int   SEG = 32;
        const float X0  = -0.35f;
        const float X1  =  0.35f;
        const float TH  =  0.05f;

        auto mouthTopY = [&](float x) {
            float t = x / 0.35f;
            return 0.13f - 0.15f * (1.0f - t * t);   // 原 0.28，下移 0.15
        };

        for (int i = 0; i < SEG; ++i) {
            float xa = X0 + (X1 - X0) * (float)i       / (float)SEG;
            float xb = X0 + (X1 - X0) * (float)(i + 1) / (float)SEG;
            float ya = mouthTopY(xa);
            float yb = mouthTopY(xb);

            float za1 = headSurfZ(xa, ya)      + 0.02f;
            float zb1 = headSurfZ(xb, yb)      + 0.02f;
            float za2 = headSurfZ(xa, ya - TH) + 0.02f;
            float zb2 = headSurfZ(xb, yb - TH) + 0.02f;

            renderer.addTriangle(Triangle(
                { tx(xa), ty(ya),      tz(za1) },
                { tx(xb), ty(yb - TH), tz(zb2) },
                { tx(xb), ty(yb),      tz(zb1) },
                MOUTH));

            renderer.addTriangle(Triangle(
                { tx(xa), ty(ya),      tz(za1) },
                { tx(xa), ty(ya - TH), tz(za2) },
                { tx(xb), ty(yb - TH), tz(zb2) },
                MOUTH));
        }
    }

    renderer.paint();
    return 0;
}
