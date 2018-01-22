#ifndef _WINDOWOPERATION_H
#define _WINDOWOPERATION_H

#include <Windows.h>
#include <set>
#include <string>

#ifdef _UNICODE
#   define GetWindowClassName GetWindowClassNameW
#   define GetWindowCaption GetWindowCaptionW
#else
#   define GetWindowClassName GetWindowClassNameA
#   define GetWindowCaption GetWindowCaptionA
#endif

DWORD GetWindowStyle(HWND hWindow);
DWORD GetWindowExStyle(HWND hWindow);
DWORD_PTR GetWindowUserData(HWND hWindow);
void ModifyStyle(HWND hWindow, DWORD dwAdd, DWORD dwRemove);
void ModifyExStyle(HWND hWindow, DWORD dwAdd, DWORD dwRemove);
std::set<HWND> GetAllWnds();
std::set<HWND> GetAllChildWnds(HWND hWindow);
std::set<HWND> GetThreadWnds(DWORD dwThreadId);
HWND GetShellDllDefViewWnd();
HWND GetDesktopListViewWnd();
bool ReloadIcons();
std::string GetWindowClassNameA(HWND hWindow);
std::wstring GetWindowClassNameW(HWND hWindow);
std::string GetWindowCaptionA(HWND hWindow);
std::wstring GetWindowCaptionW(HWND hWindow);
RECT GetWindowRectHelper(HWND hWindow);
int GetWindowWidth(HWND hWindow);
int GetWindowHeight(HWND hWindow);
int GetWindowLeft(HWND hWindow);
int GetWindowTop(HWND hWindow);
int GetWindowRight(HWND hWindow);
int GetWindowBottom(HWND hWindow);
RECT GetWindowClientRectHelper(HWND hWindow);
int GetWindowClientWidth(HWND hWindow);
int GetWindowClientHeight(HWND hWindow);
int GetWindowClientLeft(HWND hWindow);
int GetWindowClientTop(HWND hWindow);
int GetWindowClientRight(HWND hWindow);
int GetWindowClientBottom(HWND hWindow);
DWORD GetWindowThreadId(HWND hWindow);
DWORD GetWindowProcessId(HWND hWindow);
HANDLE SetForegroundWindowNotify(void (CALLBACK *pNotify)(HWND, void *), void *lpUserData);
BOOL CloseForgroundWindowNotify(HANDLE hNotify);
BOOL SaveWindowPlacement(HWND hWindow, LPCWSTR lpProcMark = NULL, LPCWSTR lpWndMark = NULL);
BOOL LoadWindowPlacement(HWND hWindow, LPCWSTR lpProcMark = NULL, LPCWSTR lpWndMark = NULL, BOOL bIgnoreMaximize = FALSE, BOOL bIgnoreMinimize = TRUE);

#endif /* _WINDOWOPERATION_H */
