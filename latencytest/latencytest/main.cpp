#include <Windows.h>
#include <stdio.h>

unsigned frameNumber = 0;
int X = 0;
int Y = 0;
HBRUSH green, red;

DWORD previousSimulationTime;
DWORD currentSimulationTime;

void simulate() {
    ++frameNumber;

    previousSimulationTime = currentSimulationTime;
    currentSimulationTime = timeGetTime();

    const int S = 4;
    X += S * (GetKeyState(VK_RIGHT) < 0);
    X -= S * (GetKeyState(VK_LEFT) < 0);
    Y += S * (GetKeyState(VK_DOWN) < 0);
    Y -= S * (GetKeyState(VK_UP) < 0);
}

void CALLBACK postSimulate(UINT, UINT, DWORD user, DWORD, DWORD) {
    HWND window = (HWND)user;
    PostMessage(window, WM_TIMER, 0, 0);
}

LRESULT CALLBACK WndProc(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam
) {
    switch (message) {
    case WM_CLOSE:
        DestroyWindow(window);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        RECT rect;
        GetClientRect(window, &rect);
        
        int px = (rect.right - rect.left) / 2 + X;
        int py = (rect.bottom - rect.top) / 2 + Y;

        RECT fill;
        fill.left = px - 5;
        fill.right = px + 5;
        fill.top = py - 5;
        fill.bottom = py + 5;

        PAINTSTRUCT ps;
        HDC dc = BeginPaint(window, &ps);
        FillRect(dc, &fill, (GetKeyState(VK_SPACE) < 0) ? red : green);

        WCHAR text[100];
        _swprintf(text, L"Frame: %d\nSimTime: %d\n", frameNumber, currentSimulationTime - previousSimulationTime);
        DrawTextW(dc, text, -1, &rect, DT_LEFT);

        EndPaint(window, &ps);
        return 0;
    }
    case WM_TIMER:
        simulate();
        InvalidateRect(window, 0, TRUE);
        return 0;
    default:
        return DefWindowProc(window, message, wparam, lparam);
    }
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    timeBeginPeriod(1);

    WNDCLASSW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
    wc.hCursor = LoadCursorW(0, MAKEINTRESOURCE(IDC_ARROW));
    wc.lpszClassName = L"LatencyTest";
    RegisterClassW(&wc);

    HWND mainWindow = CreateWindowW(
        L"LatencyTest",
        L"Input Latency Test",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        hInstance,
        0);
    ShowWindow(mainWindow, SW_SHOW);
    UpdateWindow(mainWindow);

    green = CreateSolidBrush(RGB(0, 255, 0));
    red = CreateSolidBrush(RGB(255, 0, 0));

    timeSetEvent(
        16,
        0,
        postSimulate,
        (DWORD_PTR)mainWindow,
        TIME_PERIODIC);

    MSG msg;
    for (;;) {
        BOOL b = GetMessageW(&msg, 0, 0, 0);
        if (b == -1) {
            return -1;
        }
        if (b == 0) {
            return msg.wParam;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
