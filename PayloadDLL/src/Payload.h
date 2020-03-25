#pragma once

#include <stdio.h>
#include <Windows.h>

__declspec(dllexport) void LogMessage(const char* message);
extern "C" __declspec(dllexport) void HookProcedure();