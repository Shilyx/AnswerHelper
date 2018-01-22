#include "slxcommon.h"
#include <winternl.h>
#include <Shlwapi.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <ShlObj.h>
#include <sstream>
#include <iomanip>
#include <list>
#include <iterator>

#pragma warning(disable: 4996)

using namespace std;

std::wstring CreateGuidW()
{
    GUID guid;

    UuidCreate(&guid);

    return fmtW(
        L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        guid.Data1,
        guid.Data2,
        guid.Data3,
        guid.Data4[0],
        guid.Data4[1],
        guid.Data4[2],
        guid.Data4[3],
        guid.Data4[4],
        guid.Data4[5],
        guid.Data4[6],
        guid.Data4[7]);
}

std::string fmtA(const char *fmt, ...)
{
    char szText[1024 * 10];
    va_list val;

    szText[0] = '\0';
    va_start(val, fmt);
    wvnsprintfA(szText, RTL_NUMBER_OF(szText), fmt, val);
    va_end(val);

    return szText;
}

std::wstring fmtW(const wchar_t *fmt, ...)
{
    wchar_t szText[1024 * 10];
    va_list val;

    szText[0] = L'\0';
    va_start(val, fmt);
    wvnsprintfW(szText, RTL_NUMBER_OF(szText), fmt, val);
    va_end(val);

    return szText;
}

std::wstring GetCurrentDirectoryStringW()
{
    WCHAR szDir[MAX_PATH] = L"";

    GetCurrentDirectoryW(RTL_NUMBER_OF(szDir), szDir);

    return szDir;
}

HANDLE CreateClassThread(std::tr1::function<DWORD()> func, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
    class Local
    {
    public:
        static DWORD CALLBACK ThreadProc(LPVOID lpParam)
        {
            std::tr1::function<DWORD()> *pFunc = (std::tr1::function<DWORD()> *)lpParam;
            std::tr1::function<DWORD()> func(*pFunc);

            delete pFunc;

            return func();
        }
    };

    try
    {
        return CreateThread(NULL, 0, Local::ThreadProc, new std::tr1::function<DWORD()>(func), dwCreationFlags, lpThreadId);
    }
    catch (std::bad_alloc &)
    {
    }

    return NULL;
}

HANDLE ShellExecuteReturnProcess(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd)
{
    SHELLEXECUTEINFO sei = {sizeof(sei)};

    sei.hwnd = hwnd;
    sei.lpVerb = lpOperation;
    sei.lpFile = lpFile;
    sei.lpParameters = lpParameters;
    sei.lpDirectory = lpDirectory;
    sei.nShow = nShowCmd;
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;

    ShellExecuteEx(&sei);

    return sei.hProcess;
}

DWORD GetParentProcessId(HANDLE hProcess)
{
    DWORD dwParentProcessId = 0;
    NTSTATUS (WINAPI *pNtQueryInformationProcess)(
        __in       HANDLE ProcessHandle,
        __in       PROCESSINFOCLASS ProcessInformationClass,
        __out      PVOID ProcessInformation,
        __in       ULONG ProcessInformationLength,
        __out_opt  PULONG ReturnLength
        ) = NULL;

    (PROC &)pNtQueryInformationProcess = GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtQueryInformationProcess");

    if (pNtQueryInformationProcess != NULL)
    {
        PROCESS_BASIC_INFORMATION pbi;
        ULONG uRetLength = 0;

        ZeroMemory(&pbi, sizeof(pbi));

        pNtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &uRetLength);

        if (uRetLength == sizeof(pbi))
        {
            dwParentProcessId = (DWORD&)pbi.Reserved3;
        }
    }

    return dwParentProcessId;
}

DWORD GetParentProcessId(DWORD dwProcessId)
{
    DWORD dwParentProcessId = -1;
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId());

    if (hProcess != NULL)
    {
        dwParentProcessId = GetParentProcessId(hProcess);
        CloseHandle(hProcess);
    }

    return dwParentProcessId;
}

BOOL JoinOneKillOnCloseJob(BOOL bAllowBreakway)
{
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli;

    ZeroMemory(&jeli, sizeof(jeli));
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    if (bAllowBreakway)
    {
        jeli.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_BREAKAWAY_OK;
    }

    HANDLE hJob = CreateJobObject(NULL, NULL);

    return hJob != NULL &&
        SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli)) &&
        AssignProcessToJobObject(hJob, GetCurrentProcess());
}
