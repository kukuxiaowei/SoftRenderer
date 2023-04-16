#include "Window.h"

#ifdef UNICODE
static const wchar_t *WINDOW_CLASS_NAME = L"Class";
static const wchar_t *WINDOW_ENTRY_NAME = L"Entry";
#else
static const char *WINDOW_CLASS_NAME = "Class";
static const char *WINDOW_ENTRY_NAME = "Entry";
#endif

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window *window = (Window*)GetProp(hWnd, WINDOW_ENTRY_NAME);
    if (window == NULL)
        return DefWindowProc(hWnd, message, wParam, lParam);
    
    int x,y;
    switch (message)
    {
    case WM_CLOSE:
        window->SetShouldClose(true);
        break;
    case WM_KEYDOWN:
        window->Keys[wParam & 511] = 1;
        break;
    case WM_KEYUP:
        window->Keys[wParam & 511] = 0;
        break;
    case WM_LBUTTONDOWN:
        window->LeftButton(true);
        break;
    case WM_LBUTTONUP:
        window->LeftButton(false);
        break;
    case WM_MOUSEMOVE:
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        window->CursorPos(x, y);
        break;
    case WM_MOUSEWHEEL:
        x = LOWORD(wParam);
        y = HIWORD(wParam);
        window->Scroll(x, y);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

void Window::Create(int width, int height, const char *title)
{
    m_Width = width;
    m_Height = height;

    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = GetModuleHandle(NULL);
    wcex.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(WHITE_BRUSH);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = WINDOW_CLASS_NAME;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        return;
    }

#ifdef UNICODE
    wchar_t windowName[128];
    mbstowcs(windowName, title, 128);
#else
    const char *windowName = title;
#endif

    m_Handle = CreateWindow(
    WINDOW_CLASS_NAME,
    windowName,
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT,
    width, height,
    NULL,
    NULL,
    GetModuleHandle(NULL),
    NULL
    );

    SetProp(m_Handle, WINDOW_ENTRY_NAME, this);
    ShowWindow(m_Handle, SW_SHOW);

    memset(Keys, 0, sizeof(int) * 512);
}

void Window::Terminate()
{
    ShowWindow(m_Handle, SW_HIDE);
    RemoveProp(m_Handle, WINDOW_ENTRY_NAME);

    DeleteDC(m_MemoryDC);
    DestroyWindow(m_Handle);
}

bool Window::ShouldClose()
{
    return m_ShouldClose;
}

void Window::SetShouldClose(bool value)
{
    m_ShouldClose = value;
}

void Window::MakeContextCurrent(std::shared_ptr<Buffer> buffer)
{
    void* data;

    HDC windowDC = GetDC(m_Handle);
    m_MemoryDC = CreateCompatibleDC(windowDC);
    ReleaseDC(m_Handle, windowDC);

    BITMAPINFOHEADER biHeader;
    memset(&biHeader, 0, sizeof(BITMAPINFOHEADER));
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = m_Width;
    biHeader.biHeight = -m_Height;  /* top-down */
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 32;
    biHeader.biCompression = BI_RGB;
    HBITMAP dibBitmap = CreateDIBSection(m_MemoryDC, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS, &data, NULL, 0);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(m_MemoryDC, dibBitmap);
    DeleteObject(oldBitmap);

    buffer->SetData(m_Width * m_Height * 4, data);
}

void Window::SwapBuffer()
{
    HDC windowDC = GetDC(m_Handle);
    BitBlt(windowDC, 0, 0, m_Width, m_Height, m_MemoryDC, 0, 0, SRCCOPY);
    ReleaseDC(m_Handle, windowDC);
}

void Window::PollEvents()
{
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

void Window::SetCursorPosCallback(MouseCallback callback)
{
    m_CursorPosCallback = callback;
}

void Window::CursorPos(int x, int y)
{
    if(m_CursorPosCallback != NULL)
    {
        m_CursorPosCallback((double)x, (double)y);
    }
}

void Window::SetScrollCallback(MouseCallback callback)
{
    m_ScrollCallback = callback;
}

void Window::Scroll(int x, int y)
{
    if(m_ScrollCallback != NULL)
    {
        m_ScrollCallback((double)x, (double)y);
    }
}

void Window::SetLeftButtonCallback(ButtonCallback callback)
{
    m_LeftButtonCallback = callback;
}

void Window::LeftButton(bool down)
{
    if(m_LeftButtonCallback != NULL)
    {
        m_LeftButtonCallback(down);
    }
}