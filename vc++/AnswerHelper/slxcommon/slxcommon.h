#ifndef _SLXCOMMON_H
#define _SLXCOMMON_H

#ifdef _MSC_VER

#include <WinSock2.h>
#include <Windows.h>
#include <TlHelp32.h>

HANDLE ShellExecuteReturnProcess(
    HWND hwnd,
    LPCTSTR lpOperation,
    LPCTSTR lpFile,
    LPCTSTR lpParameters,
    LPCTSTR lpDirectory,
    INT nShowCmd);
DWORD GetParentProcessId(HANDLE hProcess);
DWORD GetParentProcessId(DWORD dwProcessId);
BOOL JoinOneKillOnCloseJob(BOOL bAllowBreakway);

#else /* _MSC_VER */

#endif /* _MSC_VER */

#ifdef __cplusplus

#include <stdexcept>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <functional>

#ifdef _MSC_VER

#include "WindowOperation.h"

std::string CreateGuidA();
std::wstring CreateGuidW();

std::string fmtA(const char *fmt, ...);
std::wstring fmtW(const wchar_t *fmt, ...);

HANDLE CreateClassThread(std::tr1::function<DWORD()> func, DWORD dwCreationFlags, LPDWORD lpThreadId);

class CLockable
{
public:
    virtual void Lock() const = 0;
    virtual void Unlock() const = 0;
};

class CLockableCriticalSection : public CLockable
{
public:
    CLockableCriticalSection()
    {
        InitializeCriticalSection(&m_cs);
    }

    virtual ~CLockableCriticalSection()
    {
        DeleteCriticalSection(&m_cs);
    }

public:
    virtual void Lock() const
    {
        EnterCriticalSection(&m_cs);
    }

    virtual void Unlock() const
    {
        LeaveCriticalSection(&m_cs);
    }

private:
    mutable CRITICAL_SECTION m_cs;
};

class CScopedLocker
{
public:
    CScopedLocker(const CLockable *pLockable)
        : m_pLockable(pLockable)
    {
        m_pLockable->Lock();
    }

    ~CScopedLocker()
    {
        m_pLockable->Unlock();
    }

private:
    const CLockable *m_pLockable;
};

template <class Key, class Value>
class CKeyValueStore : CLockableCriticalSection
{
public:
    void set(const Key &key, const Value &value)
    {
        CScopedLocker lock(this);
        m_mapStore[key] = value;
    }

    Value get(const Key &key) const
    {
        CScopedLocker lock(this);
        std::map<Key, Value>::const_iterator it = m_mapStore.find(key);

        if (it == m_mapStore.end())
        {
            return Value();
        }
        else
        {
            return it->second;
        }
    }

    void remove(const Key &key)
    {
        CScopedLocker lock(this);
        m_mapStore.erase(key);
    }

    void clear()
    {
        CScopedLocker lock(this);
        m_mapStore.clear();
    }

private:
    std::map<Key, Value> m_mapStore;
};

#else /* _MSC_VER */

#endif /* _MSC_VER */

#endif /* __cplusplus */

#endif /* _SLXCOMMON_H */