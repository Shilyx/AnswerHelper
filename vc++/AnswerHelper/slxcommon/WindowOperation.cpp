#include "slxcommon.h"
#include "WindowOperation.h"
#include <CommCtrl.h>
#include <Shlwapi.h>

using namespace std;

DWORD GetWindowStyle(HWND hWindow)
{
    if (IsWindow(hWindow))
    {
        return (DWORD)GetWindowLongPtr(hWindow, GWL_STYLE);
    }

    return 0;
}

DWORD GetWindowExStyle(HWND hWindow)
{
    if (IsWindow(hWindow))
    {
        return (DWORD)GetWindowLongPtr(hWindow, GWL_EXSTYLE);
    }

    return 0;
}

DWORD_PTR GetWindowUserData(HWND hWindow)
{
    if (IsWindow(hWindow))
    {
        return GetWindowLongPtr(hWindow, GWLP_USERDATA);
    }

    return 0;
}

void ModifyStyle(HWND hWindow, DWORD dwAdd, DWORD dwRemove)
{
    DWORD_PTR dwStyle = GetWindowLongPtr(hWindow, GWL_STYLE);

    dwStyle |= dwAdd;
    dwStyle &= ~dwRemove;

    SetWindowLongPtr(hWindow, GWL_STYLE, dwStyle);
}

void ModifyExStyle(HWND hWindow, DWORD dwAdd, DWORD dwRemove)
{
    DWORD_PTR dwStyle = GetWindowLongPtr(hWindow, GWL_EXSTYLE);

    dwStyle |= dwAdd;
    dwStyle &= ~dwRemove;

    SetWindowLongPtr(hWindow, GWL_EXSTYLE, dwStyle);
}

std::set<HWND> GetAllWnds()
{
    set<HWND> setWnds;
    HWND hWindow = FindWindowExW(NULL, NULL, NULL, NULL);

    while (IsWindow(hWindow))
    {
        if (IsWindowVisible(hWindow))
        {
            setWnds.insert(hWindow);
        }

        hWindow = FindWindowExW(NULL, hWindow, NULL, NULL);
    }

    return setWnds;
}

static BOOL CALLBACK EnumChildWndProc(HWND hwnd, LPARAM lParam)
{
    ((set<HWND> *)lParam)->insert(hwnd);
    return TRUE;
}

std::set<HWND> GetAllChildWnds(HWND hWindow)
{
    set<HWND> setChildWnds;

    EnumChildWindows(hWindow, EnumChildWndProc, (LPARAM)&setChildWnds);

    return setChildWnds;
}

static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
    ((set<HWND> *)lParam)->insert(hwnd);
    return TRUE;
}

std::set<HWND> GetThreadWnds(DWORD dwThreadId)
{
    set<HWND> setThreadWnds;

    EnumThreadWindows(dwThreadId, EnumThreadWndProc, (LPARAM)&setThreadWnds);

    return setThreadWnds;
}

HWND GetShellDllDefViewWnd()
{
    HWND hShellDllDefView = NULL;
    vector<HWND> vectorDesktopWnds;
    HWND hWindow = FindWindowExW(NULL, NULL, L"Progman", L"Program Manager");

    while (IsWindow(hWindow))
    {
        vectorDesktopWnds.push_back(hWindow);
        hWindow = FindWindowExW(NULL, hWindow, L"Progman", L"Program Manager");
    }

    hWindow = FindWindowExW(NULL, NULL, L"WorkerW", L"");

    while (IsWindow(hWindow))
    {
        vectorDesktopWnds.push_back(hWindow);
        hWindow = FindWindowExW(NULL, hWindow, L"WorkerW", L"");
    }

    for (vector<HWND>::const_iterator it = vectorDesktopWnds.begin(); it != vectorDesktopWnds.end(); ++it)
    {
        HWND hWindow = *it;
        hShellDllDefView = FindWindowExW(hWindow, NULL, L"SHELLDLL_DefView", NULL);

        if (IsWindow(hShellDllDefView))
        {
            break;
        }
    }

    return hShellDllDefView;
}

HWND GetDesktopListViewWnd()
{
    HWND hShellDllDefView = GetShellDllDefViewWnd();
    HWND hSysListView32 = FindWindowExW(hShellDllDefView, NULL, L"SysListView32", L"FolderView");

    if (!IsWindow(hSysListView32))
    {
        hSysListView32 = FindWindowExW(hShellDllDefView, NULL, L"SysListView32", NULL);
    }

    return hSysListView32;
}

bool ReloadIcons()
{
    HWND hListView = GetDesktopListViewWnd();

    if (IsWindow(hListView))
    {
        bool b1 = !!PostMessageW(hListView, LVM_SETWORKAREAS, 0, NULL);
        bool b2 = !!PostMessageW(hListView, LVM_ARRANGE, LVA_DEFAULT, 0);
        bool b3 = !!PostMessageW(hListView, WM_KEYDOWN, VK_F5, 0);

        return b1 && b2 && b3;
    }

    return false;
}

std::string GetWindowClassNameA(HWND hWindow)
{
    char szText[4096] = "";

    GetClassNameA(hWindow, szText, RTL_NUMBER_OF(szText));

    return szText;
}

std::wstring GetWindowClassNameW(HWND hWindow)
{
    WCHAR szText[4096] = L"";

    GetClassNameW(hWindow, szText, RTL_NUMBER_OF(szText));

    return szText;
}

std::string GetWindowCaptionA(HWND hWindow)
{
    char szText[4096] = "";

    GetWindowTextA(hWindow, szText, RTL_NUMBER_OF(szText));

    return szText;
}

std::wstring GetWindowCaptionW(HWND hWindow)
{
    WCHAR szText[4096] = L"";

    GetWindowTextW(hWindow, szText, RTL_NUMBER_OF(szText));

    return szText;
}

RECT GetWindowRectHelper(HWND hWindow)
{
    RECT rect = {0};

    GetWindowRect(hWindow, &rect);

    return rect;
}

RECT GetWindowClientRectHelper(HWND hWindow)
{
    RECT rect = {0};

    GetClientRect(hWindow, &rect);

    return rect;
}

int GetWindowWidth(HWND hWindow)
{
    RECT rect = GetWindowRectHelper(hWindow);

    return rect.right - rect.left;
}

int GetWindowHeight(HWND hWindow)
{
    RECT rect = GetWindowRectHelper(hWindow);

    return rect.bottom - rect.top;
}

int GetWindowLeft(HWND hWindow)
{
    return GetWindowRectHelper(hWindow).left;
}

int GetWindowTop(HWND hWindow)
{
    return GetWindowRectHelper(hWindow).top;
}

int GetWindowRight(HWND hWindow)
{
    return GetWindowRectHelper(hWindow).right;
}

int GetWindowBottom(HWND hWindow)
{
    return GetWindowRectHelper(hWindow).bottom;
}

int GetWindowClientWidth(HWND hWindow)
{
    RECT rect = GetWindowClientRectHelper(hWindow);

    return rect.right - rect.left;
}

int GetWindowClientHeight(HWND hWindow)
{
    RECT rect = GetWindowClientRectHelper(hWindow);

    return rect.bottom - rect.top;
}

int GetWindowClientLeft(HWND hWindow)
{
    return GetWindowClientRectHelper(hWindow).left;
}

int GetWindowClientTop(HWND hWindow)
{
    return GetWindowClientRectHelper(hWindow).top;
}

int GetWindowClientRight(HWND hWindow)
{
    return GetWindowClientRectHelper(hWindow).right;
}

int GetWindowClientBottom(HWND hWindow)
{
    return GetWindowClientRectHelper(hWindow).bottom;
}

DWORD GetWindowThreadId(HWND hWindow)
{
    return GetWindowThreadProcessId(hWindow, NULL);
}

DWORD GetWindowProcessId(HWND hWindow)
{
    DWORD dwProcessId = 0;

    GetWindowThreadProcessId(hWindow, &dwProcessId);

    return dwProcessId;
}

//////////////////////////////////////////////////////////////////////////
class CForegroundWindowNotify;
static CKeyValueStore<HWINEVENTHOOK, CForegroundWindowNotify *> g_kvHooks;

class CForegroundWindowNotify
{
public:
    CForegroundWindowNotify(void (CALLBACK *pNotify)(HWND, void *), void *lpUserData)
        : m_pNotify(pNotify)
        , m_lpUserData(lpUserData)
    {
        m_hHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_FOREGROUND,
            NULL,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
            );
        g_kvHooks.set(m_hHook, this);
    }

    ~CForegroundWindowNotify()
    {
        g_kvHooks.remove(m_hHook);
        UnhookWinEvent(m_hHook);
    }

private:
    static VOID CALLBACK WinEventProc(
        HWINEVENTHOOK hWinEventHook,
        DWORD event,
        HWND hwnd,
        LONG idObject,
        LONG idChild,
        DWORD dwEventThread,
        DWORD dwmsEventTime
        )
    {
        if (IsWindow(hwnd) && EVENT_SYSTEM_FOREGROUND == event)
        {
            CForegroundWindowNotify *pForegroundWindowNotify = g_kvHooks.get(hWinEventHook);

            if (pForegroundWindowNotify != NULL)
            {
                pForegroundWindowNotify->m_pNotify(hwnd, pForegroundWindowNotify->m_lpUserData);
            }
        }
    }

private:
    HWINEVENTHOOK m_hHook;
    void (CALLBACK *m_pNotify)(HWND, void *);
    void *m_lpUserData;
};

HANDLE SetForegroundWindowNotify(void (CALLBACK *pNotify)(HWND, void *), void *lpUserData)
{
    return (HANDLE) new CForegroundWindowNotify(pNotify, lpUserData);
}

BOOL CloseForgroundWindowNotify(HANDLE hNotify)
{
    delete (CForegroundWindowNotify *)hNotify;
    return TRUE;
}

class CWndPosRecoder
{
    class CRegHelper
    {
    public:
        CRegHelper(HKEY hRootKey, LPCWSTR lpRegPath)
            : m_hKey(NULL)
        {
            RegCreateKeyExW(hRootKey, lpRegPath, 0, NULL, 0, KEY_SET_VALUE | KEY_QUERY_VALUE, NULL, &m_hKey, NULL);
        }

        ~CRegHelper()
        {
            if (m_hKey != NULL)
            {
                RegCloseKey(m_hKey);
            }
        }

        BOOL SetValue(LPCWSTR lpValueName, DWORD dwValue)
        {
            return ERROR_SUCCESS == RegSetValueExW(m_hKey, lpValueName, 0, REG_DWORD, (const BYTE *)&dwValue, sizeof(dwValue));
        }

        DWORD GetValue(LPCWSTR lpValueName, DWORD dwDefault = 0) const
        {
            DWORD dwValue;
            DWORD dwSize = sizeof(dwValue);

            if (ERROR_SUCCESS == RegQueryValueExW(m_hKey, lpValueName, NULL, NULL, (BYTE *)&dwValue, &dwSize))
            {
                return dwValue;
            }

            return dwDefault;
        }

    private:
        HKEY m_hKey;
    };

    static void BuildRegPath(LPCTSTR lpProcessMark, LPCTSTR lpWindowMark, LPWSTR lpRegPath, DWORD dwBufferSize)
    {
        WCHAR szProcessPath[MAX_PATH] = {0};

        if (lpProcessMark == NULL || *lpProcessMark == L'\0')
        {
            GetModuleFileNameW(GetModuleHandle(NULL), szProcessPath, RTL_NUMBER_OF(szProcessPath));
            lpProcessMark = szProcessPath;

            for (int i = 0; i < lstrlen(szProcessPath); ++i)
            {
                if (szProcessPath[i] == L'\\')
                {
                    szProcessPath[i] = L'/';
                }
            }
        }

        if (lpWindowMark == NULL || *lpWindowMark == L'\0')
        {
            lpWindowMark = L"_default";
        }

        wnsprintfW(lpRegPath, dwBufferSize, L"software\\Shilyx Studio\\WindowPlacementRecorder Module\\%ls\\%ls", lpProcessMark, lpWindowMark);
    }

public:
    static BOOL SaveWindowPlacement(HWND hWindow, LPCWSTR lpProcMark, LPCWSTR lpWndMark)
    {
        if (!IsWindow(hWindow))
        {
            return FALSE;
        }

        WCHAR szRegPath[1024];

        BuildRegPath(lpProcMark, lpWndMark, szRegPath, RTL_NUMBER_OF(szRegPath));

        RECT rect;
        WINDOWPLACEMENT wp = {sizeof(wp)};
        CRegHelper reg(HKEY_CURRENT_USER, szRegPath);

        if (IsIconic(hWindow))
        {
            reg.SetValue(L"min-1normal0max1", -1);
            GetWindowPlacement(hWindow, &wp);
            rect = wp.rcNormalPosition;
        }
        else if (IsZoomed(hWindow))
        {
            reg.SetValue(L"min-1normal0max1", 1);
            GetWindowPlacement(hWindow, &wp);
            rect = wp.rcNormalPosition;
        }
        else
        {
            reg.SetValue(L"min-1normal0max1", 0);
            GetWindowRect(hWindow, &rect);
        }

        reg.SetValue(L"left", rect.left);
        reg.SetValue(L"right", rect.right);
        reg.SetValue(L"top", rect.top);
        reg.SetValue(L"bottom", rect.bottom);

        return true;
    }

    static BOOL LoadWindowPlacement(HWND hWindow, LPCWSTR lpProcMark, LPCWSTR lpWndMark, BOOL bIgnoreMaximize, BOOL bIgnoreMinimize)
    {
        if (!IsWindow(hWindow))
        {
            return FALSE;
        }

        WCHAR szRegPath[1024];

        BuildRegPath(lpProcMark, lpWndMark, szRegPath, RTL_NUMBER_OF(szRegPath));

        RECT rect;
        CRegHelper reg(HKEY_CURRENT_USER, szRegPath);
        DWORD dwMinMax = reg.GetValue(L"min-1normal0max1", 1987);

        if (dwMinMax != 1 && dwMinMax != -1 && dwMinMax != 0)
        {
            return FALSE;
        }

        rect.left = reg.GetValue(L"left", 0);
        rect.right = reg.GetValue(L"right", 0);
        rect.top = reg.GetValue(L"top", 110);
        rect.bottom = reg.GetValue(L"bottom", 110);

        SetWindowPos(hWindow, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);

        if (dwMinMax == -1)
        {
            ShowWindowAsync(hWindow, SW_MINIMIZE);
        }
        else if (dwMinMax == 1)
        {
            ShowWindowAsync(hWindow, SW_MAXIMIZE);
        }

        return TRUE;
    }
};

BOOL SaveWindowPlacement(HWND hWindow, LPCWSTR lpProcMark, LPCWSTR lpWndMark)
{
    return CWndPosRecoder::SaveWindowPlacement(hWindow, lpProcMark, lpWndMark);
}

BOOL LoadWindowPlacement(HWND hWindow, LPCWSTR lpProcMark, LPCWSTR lpWndMark, BOOL bIgnoreMaximize, BOOL bIgnoreMinimize)
{
    return CWndPosRecoder::LoadWindowPlacement(hWindow, lpProcMark, lpWndMark, bIgnoreMaximize, bIgnoreMinimize);
}