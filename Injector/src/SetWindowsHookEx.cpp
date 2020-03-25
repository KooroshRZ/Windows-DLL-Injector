#include "Injector.h"

bool SetWindowsHookEx_type4(DWORD processId, LPCSTR dllPath) {

	HMODULE hModDll = LoadLibrary(dllPath);

	if (!hModDll) {
		printf("Error in getting module handle for %s\n", dllPath);
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	HOOKPROC procAddress = (HOOKPROC)GetProcAddress(hModDll, "HookProcedure");

	if (!procAddress) {
		printf("Error in getting hook procedure address in %s\n", dllPath);
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take snaphsot of all running threads
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		printf("Could not get snapshot of threads\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	// set structure size
	te32.dwSize = sizeof(THREADENTRY32);
	DWORD threadId = 0;

	if (!Thread32First(hThreadSnap, &te32)) {
		printf("Could not get first thread\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	HWND hwnd = nullptr;
	hwnd = FindWindowA(NULL, "C:\\Users\\kourosh\\source\\repos\\WindowsDLLInjector\\TargetProgramBuild\\bin\\x64\\Debug\\TargetProgram.exe");
	GetWindowThreadProcessId(hwnd, &processId);

	if (!hwnd) {
		printf("Could not find window\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	do {

		if (te32.th32OwnerProcessID == processId) {

			threadId = te32.th32ThreadID;
			printf("%d", threadId);
			HANDLE hThread = OpenThread(READ_CONTROL, FALSE, te32.th32ThreadID);

			if (hThread) {


				printf("Setting hook in thread with ID %d\n", threadId);
				HHOOK hookHandle = SetWindowsHookExA(WH_KEYBOARD, procAddress, hModDll, (DWORD)threadId);

				if (!hookHandle) {
					printf("Error in Setting Hook in thread\n");
					printf("LastError : 0X%x\n", GetLastError());
					system("PAUSE");
					return false;
				}
				else
				{
					printf("Hook Installed successfully via SetWindowsHookEx in process %d\n", processId);
					system("PAUSE");
					UnhookWindowsHookEx(hookHandle);
					return true;
				}
			}
			else {
				printf("Could not find suitable Thread\n");
				system("PAUSE");
			}
				

		}

	} while (Thread32Next(hThreadSnap, &te32));

	if (!threadId)
		printf("No Thread for use in process %d\n", processId);

	return false;


}