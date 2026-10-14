#include "src/math/vec3d.h"
#include "src/math/vec4d.h"
#include "src/math/mat4.h"
#include "src/render/display.h"

#include <vector>
#include <cstdint>

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

int main() {
    const int W = 800;
    const int H = 600;
    std::vector<uint32_t> fb(W * H);

    // ºìÂÌ¶şÎ¬½¥±ä£¬²âÊÔÊä³ö
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int r = x * 255 / (W - 1);
            int g = y * 255 / (H - 1);
            int b = 128;
            fb[y * W + x] = rgba(r, g, b);
        }
    }

    showWindow(fb, W, H);
    return 0;
}
