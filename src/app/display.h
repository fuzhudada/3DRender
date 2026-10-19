#pragma once
#include <cstdint>
#include <vector>
#include "src/model/triangle.h"

// 弹出一个窗口显示 framebuffer 内容，阻塞直到用户关闭窗口。
// pixels 格式：0xAARRGGBB，长度 >= width * height
void showWindow(const std::vector<uint32_t>& pixels, int width, int height);

void updateWindow(const std::vector<uint32_t>& pixels, int width, int height);

void pollEvents();

bool isOpen();

bool getKeyDown(int vk);

int getWidth();

int getHeight();

void exit();

void getMouseDelta(float& dx, float& dy);

void setMouseCaptured(bool captured);

bool getMouseCaptured();
