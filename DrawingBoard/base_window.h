#include <Windows.h>
#include <WinUser.h>

template <class DERIVED_TYPE>
class BaseWindow {
  public:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        DERIVED_TYPE *pThis = NULL;

        if (uMsg == WM_NCCREATE) {
            // @see https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-nccreate

            CREATESTRUCT *pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
            pThis->m_hWnd = hWnd;
        } else {
            pThis = (DERIVED_TYPE*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
        }

        if (pThis) {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        } else {
            return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hWnd(NULL) {}
    virtual ~BaseWindow() = default;

    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = 1200,
        int nHeight = 800,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    ) {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = ::GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();

        ::RegisterClass(&wc);

        m_hWnd = ::CreateWindowEx(
                dwExStyle,                    // Optional window styles.
                ClassName(),                  // Window class
                lpWindowName,                 // Window text
                dwStyle,                      // Window style

                // Size and position
                x, y, nWidth, nHeight,

                hWndParent,               // Parent window
                hMenu,                    // Menu
                ::GetModuleHandle(NULL),  // Instance handle
                this                      // Additional application data
            );

        return (m_hWnd ? TRUE : FALSE);
    }

    HWND Window() const {
        return m_hWnd;
    }

  protected:

    virtual PCWSTR  ClassName() const = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hWnd;
};

