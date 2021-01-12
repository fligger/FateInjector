#pragma once
int performInjection(DWORD procId, const wchar_t* dllPath);
DWORD GetProcId(const char* procName);

