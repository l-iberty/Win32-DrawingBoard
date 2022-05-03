// https://docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window
//
// https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/Win7Samples/begin/LearnWin32/DrawCircle

#define NOMINMAX
#include <Windows.h>
#include <vector>
#include "base_window.h"
#include "shape.h"
#include "painter.h"
#include "dragger.h"
#include "factory.h"
#include "plugin_loader.h"

typedef const char* (*PluginNameFn)();
typedef ShapeFactory* (*CreateShapeFactoryFn)();
typedef PainterFactory* (*CreatePainterFactoryFn)();

PluginLoader g_pluginLoader("*");


class MainWindow : public BaseWindow<MainWindow> {
  public:
    MainWindow();
    virtual ~MainWindow();

    PCWSTR ClassName() const override {
        return L"Drawing Board";
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

  private:
    void OnPaint();
    void OnMenuCommand( WPARAM wParam, LPARAM lParam);
    void OnLButtonDown(int x, int y, DWORD flags);
    void OnRButtonDown(int x, int y, DWORD flags);
    void OnMouseMove(int x, int y, DWORD flags);
    void DoubleBufferingPaint(HDC hdc, PPAINTSTRUCT ps);
    void Repaint();

    Shape *FindShapeContainsPoint(const POINT &pt);

    void SetCursorStyle(LPCWSTR lpCursorName);

    BOOL m_drawing, m_dragging, m_drawMode, m_dragMode;
    Shape *m_shape;
    Painter *m_painter;
    Dragger *m_dragger;
    std::vector<Shape*> m_shapes;
    std::vector<Painter*> m_painters;
    std::vector<ShapeFactory*> m_shapeFactories;
    std::vector<PainterFactory*> m_painterFactories;
};

MainWindow::MainWindow(): m_drawing(false), m_dragging(false), m_drawMode(false),
    m_dragMode(false), m_shape(nullptr), m_painter(nullptr), m_dragger(new Dragger) {

    const std::vector<HMODULE> &hModules = g_pluginLoader.GetModules();
    for (HMODULE hMod : hModules) {
        CreateShapeFactoryFn pfnCreateShapeFactory = (CreateShapeFactoryFn)::GetProcAddress(hMod, "CreateShapeFactory");
        CreatePainterFactoryFn pfnCreatePainterFactory = (CreatePainterFactoryFn)::GetProcAddress(hMod, "CreatePainterFactory");
        m_shapeFactories.push_back(pfnCreateShapeFactory());
        m_painterFactories.push_back(pfnCreatePainterFactory());
    }
}

MainWindow::~MainWindow() {
    delete m_shape;
    delete m_painter;
    delete m_dragger;
    m_shapes.erase(m_shapes.begin(), m_shapes.end());
    m_painters.erase(m_painters.begin(), m_painters.end());
    m_shapeFactories.erase(m_shapeFactories.begin(), m_shapeFactories.end());
    m_painterFactories.erase(m_painterFactories.begin(), m_painterFactories.end());
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            OnPaint();
            return 0;

        case WM_MENUCOMMAND:
            OnMenuCommand(wParam, lParam);
            return 0;

        case WM_LBUTTONDOWN:
            OnLButtonDown(LOWORD(lParam), HIWORD(lParam), (DWORD)wParam);
            return 0;

        case WM_RBUTTONDOWN:
            OnRButtonDown(LOWORD(lParam), HIWORD(lParam),  (DWORD)wParam);
            return 0;

        case WM_MOUSEMOVE:
            OnMouseMove(LOWORD(lParam), HIWORD(lParam),  (DWORD)wParam);
            return 0;
    }
    return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

void MainWindow::DoubleBufferingPaint(HDC hdc, PPAINTSTRUCT ps) {
    RECT rect;
    HDC hdcMemDC;
    HBITMAP hBitmap;

    ::GetClientRect(m_hWnd, &rect);
    int nWidth = rect.right - rect.left;
    int nHeight = rect.bottom - rect.top;

    hdcMemDC = ::CreateCompatibleDC(hdc);
    hBitmap = ::CreateCompatibleBitmap(hdc, nWidth, nHeight);

    ::SelectObject(hdcMemDC, hBitmap);

    ::FillRect(hdcMemDC, &ps->rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

    for (size_t i = 0; i < m_shapes.size(); i++) {
        m_painters[i]->Draw(hdcMemDC, m_shapes[i]->GetPoints(), m_shapes[i]->GetBrushColor());
    }

    if (m_shape && m_painter) {
        if (m_drawing) {
            m_painter->Draw(hdcMemDC, m_shape->GetPoints(), m_shape->GetBrushColor());
        }
    }

    ::BitBlt(hdc, 0, 0, nWidth, nHeight, hdcMemDC, 0, 0, SRCCOPY);

    ::DeleteObject(hBitmap);
    ::DeleteDC(hdcMemDC);
}

void MainWindow::Repaint() {
    ::InvalidateRect(m_hWnd, NULL, FALSE);
}

void MainWindow::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint(m_hWnd, &ps);

    DoubleBufferingPaint(hdc, &ps);

    ::EndPaint(m_hWnd, &ps);
}

void MainWindow::OnMenuCommand(WPARAM wParam, LPARAM lParam) {
    HMENU hMenu = (HMENU)lParam;
    if (hMenu == ::GetMenu(m_hWnd)) {
        int index = (int)wParam;
        if (index == 0) { /* move */
            m_drawMode = FALSE;
            m_dragMode = TRUE;
            SetCursorStyle(IDC_HAND);
            return;
        }

        m_drawMode = TRUE;
        m_dragMode = FALSE;
        SetCursorStyle(IDC_CROSS);

        m_shape = m_shapeFactories[index - 1]->CreateShape();
        m_painter = m_painterFactories[index - 1]->CreatePainter();
    }
}

void MainWindow::OnLButtonDown(int x, int y, DWORD flags) {
    POINT pt = { x, y };
    m_drawing = m_drawMode;
    m_dragging = m_dragMode;

    if (m_drawing) {
        if (m_shape && m_painter) {
            m_painter->StartDrawing(m_shape, pt);
        }
    } else if (m_dragging) {
        m_shape = FindShapeContainsPoint(pt);
        if (m_shape) {
            m_shape->SetBrushColor(RGB(255, 0, 0));
            m_dragger->Start(pt);
        }
    }
}

void MainWindow::OnRButtonDown(int x, int y, DWORD flags) {
    if (m_shape && m_painter) {
        if (m_drawing) {
            m_shapes.push_back(m_shape);
            m_painters.push_back(m_painter);
            m_shape = m_shape->Reset();
        }
        m_shape->SetBrushColor(RGB(255, 255, 255));
    }
    m_drawing = FALSE;
    m_dragging = FALSE;
}

void MainWindow::OnMouseMove(int x, int y, DWORD flags) {
    POINT pt = { x, y };
    if (m_shape && m_painter) {
        if (m_drawing) {
            m_painter->Update(m_shape, pt);
        } else if (m_dragging) {
            m_dragger->Drag(m_shape, pt);
        }
        Repaint();
    }
}

Shape *MainWindow::FindShapeContainsPoint(const POINT &pt) {
    Shape *shape = nullptr;
    for (Shape *s : m_shapes) {
        if (s->Contains(pt)) {
            shape = s;
        }
    }
    return shape;
}

void MainWindow::SetCursorStyle(LPCWSTR lpCursorName) {
    HCURSOR hCursor = ::LoadCursor(0, lpCursorName);
    ::SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)hCursor);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    MainWindow win;

    if (!win.Create(L"Drawing Board", WS_OVERLAPPEDWINDOW)) {
        return 0;
    }

    std::vector<const char*> items = { "move" };
    const std::vector<HMODULE> &hModules = g_pluginLoader.GetModules();
    for (HMODULE hMod : hModules) {
        PluginNameFn pfnPluginName = (PluginNameFn)::GetProcAddress(hMod, "PluginName");
        items.push_back(pfnPluginName());
    }

    HMENU hMenu = CreateMenu();
    for (auto &item : items) {
        ::AppendMenuA(hMenu, MF_STRING, (UINT_PTR)CreateMenu(), item);
    }

    // https://www.codenong.com/7541750/
    MENUINFO mi;
    memset(&mi, 0, sizeof(mi));
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_STYLE;
    mi.dwStyle = MNS_NOTIFYBYPOS;
    ::SetMenuInfo(hMenu, &mi);

    ::SetMenu(win.Window(), hMenu);

    ::ShowWindow(win.Window(), nCmdShow);

    // Run the message loop.

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return 0;
}

