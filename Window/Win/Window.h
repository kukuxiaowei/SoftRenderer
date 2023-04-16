#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <memory>
#include "Buffer.h"

typedef void (*ButtonCallback)(bool);
typedef void (*MouseCallback)(double, double);

class Window
{
private:
    HWND m_Handle;
    HDC m_MemoryDC;
    int m_Width;
    int m_Height;
    bool m_ShouldClose;

    ButtonCallback m_LeftButtonCallback;
    MouseCallback m_CursorPosCallback;
    MouseCallback m_ScrollCallback;
public:
    void Create(int width, int height, const char* title);
    void Terminate();
    bool ShouldClose();
    void SetShouldClose(bool value);

    void MakeContextCurrent(std::shared_ptr<Buffer> buffer);
    void SwapBuffer();

    void PollEvents();
    int Keys[512];

    void SetLeftButtonCallback(ButtonCallback callback);
    void LeftButton(bool down);
    void SetCursorPosCallback(MouseCallback callback);
    void CursorPos(int x, int y);
    void SetScrollCallback(MouseCallback callback);
    void Scroll(int x, int y);
};

#endif