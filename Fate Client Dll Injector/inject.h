#pragma once
#include "framework.h"
int performInjection(DWORD procId, const char* dllPath);
DWORD GetProcId(const char* procName);

