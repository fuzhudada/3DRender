#include "text.h"
#include "overlay.h"

static uint32_t rgba(int r, int g, int b, int a = 255) {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

void drawFpsStr(std::vector<uint32_t>& fb, int w, int h, float fps) {
    drawText(fb, w, h, 20, 20, "FPS: " + std::to_string(fps), rgba(0, 255, 0), 3);
}

void drawCrosshair(std::vector<uint32_t>& fb, int w, int h) {
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
