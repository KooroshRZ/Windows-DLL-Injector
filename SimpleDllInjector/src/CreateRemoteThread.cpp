#include "Injector.h"

bool CreateRemoteThread_Type1(LPCSTR DllPath, HANDLE hProcess) {

	

	LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	if (!LoadLibAddr) {
		printf("Could note locate real address of LoadLibraryA!\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("LoadLibraryA is located at real address: 0X%p\n", (void*)LoadLibAddr);
	Sleep(2000);

	LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(DllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath) {
		printf("Could not allocate Memory in target process\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory allocated at: 0X%p\n", (void*)pDllPath);
	Sleep(2000);

	BOOL Written = WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath), NULL);

	if (!Written) {
		printf("Could not write into the allocated memory\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory was written at address : 0x%p\n", (void*)pDllPath);
	Sleep(2000);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, pDllPath, 0, NULL);

	if (!hThread) {
		printf("Could not open Thread with CreatRemoteThread API\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Thread started with CreateRemoteThread\n");
	Sleep(2000);

	WaitForSingleObject(hThread, INFINITE);

	if (VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE)) {
		//VirtualFreeEx(hProc, reinterpret_cast<int*>(pDllPath) + 0X010000, 0, MEM_RELEASE);
		printf("Memory was freed in target process\n");
		Sleep(2000);
	}

	CloseHandle(hThread);

	return true;
}