#pragma once

#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>


bool CreateRemoteThread_Type1(LPCSTR DllPath, DWORD PID);
bool NtCreateThreadEx_Type2(LPCSTR DllPath, DWORD PID);