#include "pch.h"
#include "inject.h"


#include <TlHelp32.h>

DWORD GetProcId(const char* procName)

{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry))
        {
            do
            {
                char output[256] = "error"; // convert wchar* to char*
                sprintf(output, "%ws", procEntry.szExeFile);
                if (!_stricmp(output, procName))
                {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}



int performInjection(DWORD procId, const wchar_t* dllPath) {
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        WriteProcessMemory(hProc, loc, dllPath, wcslen(dllPath) * 2 + 2, 0); //length * 2 for bytes + 2 for end string

         HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, loc, 0, 0); // using LoadLibraryW instead of LoadLibraryA to allow wchar

        if (hThread)
        {
            CloseHandle(hThread);
        }
    }
    if (hProc)
    {
        CloseHandle(hProc);
    }
    return 0;
}
