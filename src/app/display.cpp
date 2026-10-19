#include <windows.h>
#include "display.h"

// 全局，供窗口过程访问
static const std::vector<uint32_t>* g_pixels = nullptr;
static int g_width = 0;
static int g_height = 0;
static HWND g_hwnd = nullptr;
static bool g_open = false;
static bool g_mouseCaptured = false;

static bool g_keys[256] = {};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_pixels && g_width > 0 && g_height > 0) {
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = g_width;
            bmi.bmiHeader.biHeight = -g_height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            StretchDIBits(hdc, 0, 0, g_width, g_height, 0, 0, g_width, g_height,
                          g_pixels->data(), &bmi, DIB_RGB_COLORS, SRCCOPY);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wp < 256) g_keys[wp] = true;
        return 0;
    case WM_KEYUP:
        if (wp < 256) g_keys[wp] = false;
        return 0;
    case WM_SIZE:
        g_width = LOWORD(lp);
        g_height = HIWORD(lp);
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

void showWindow(const std::vector<uint32_t>& pixels, int width, int height) {
    g_pixels = &pixels;
    g_width  = width;
    g_height = height;

    HINSTANCE hInst = GetModuleHandle(nullptr);

    const wchar_t* CLASS_NAME = L"RendererWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassW(&wc);

    // 计算窗口外框大小，让客户区正好是 width x height
    RECT rect = { 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, nullptr, hInst, nullptr
    );

    ShowWindow(g_hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(g_hwnd);

    g_open = true;
    setMouseCaptured(true);
}

void updateWindow(const std::vector<uint32_t>& pixels, int width, int height) {
    g_pixels = &pixels;
    g_width  = width;
    g_height = height;

    InvalidateRect(g_hwnd, nullptr, false);
    UpdateWindow(g_hwnd);
}

void pollEvents() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            g_open = false;
            setMouseCaptured(false);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

bool isOpen() {
    return g_open;
}

bool getKeyDown(int vk) {
    if (vk < 0 || vk >= 256) return false;
    return g_keys[vk];
}

int getWidth() {
    return g_width;
}

int getHeight() {
    return g_height;
}

void exit() {
    DestroyWindow(g_hwnd);
}

void getMouseDelta(float& dx, float& dy) {
    dx = dy = 0;
    if(!g_mouseCaptured || !g_hwnd) return;

    RECT rect;
    GetClientRect(g_hwnd, &rect);
    int cx = (rect.right - rect.left) / 2;
    int cy = (rect.bottom - rect.top) / 2;

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(g_hwnd, &p);

    dx = (float)(p.x - cx);
    dy = (float)(p.y - cy);

    POINT center = {cx, cy};
    ClientToScreen(g_hwnd, &center);
    SetCursorPos(center.x, center.y);
}

void setMouseCaptured(bool captured) {
    g_mouseCaptured = captured;
    if (captured) {
        ShowCursor(FALSE);
        if (g_hwnd) {
            RECT rect;
            GetClientRect(g_hwnd, &rect);
            POINT center = {(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2};
            ClientToScreen(g_hwnd, &center);
            SetCursorPos(center.x, center.y);
        }
    } else {
        ShowCursor(TRUE);
    }
}

bool getMouseCaptured() {
    return g_mouseCaptured;
}
