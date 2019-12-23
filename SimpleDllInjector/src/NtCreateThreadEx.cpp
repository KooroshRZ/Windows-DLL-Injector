#include "Injector.h"

bool NtCreateThreadEx_Type2(LPCSTR DllPath, HANDLE hProcess) {


	LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	if (!LoadLibraryAddr) {
		printf("Could note get real address of LoadLibraryA from kernel32.dll!\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("LoadLibraryA is located at real address: 0X%p\n", (void*)LoadLibraryAddr);
	Sleep(2000);
	//system("PAUSE");
	

	LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath) {
		printf("Could not allocate Memory in target process\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory allocated at: 0X%p\n", (void*)pDllPath);
	Sleep(2000);
	//system("PAUSE");
	

	BOOL Written = WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath), NULL);


	if (!Written) {
		printf("Could not write into the allocated memory\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory was written at address : 0x%p\n", (void*)pDllPath);
	Sleep(2000);
	//system("PAUSE");

	HMODULE modNtDll = GetModuleHandle("ntdll.dll");

	if (!modNtDll) {
		printf("Failed to get moduke handle for ntdll.dll");
		printf("LastError : 0X%x\n", GetLastError());
		system("PASUE");
		return false;
	}

	LPFUN_NtCreateThreadEx funNtCreateThreadEx = (LPFUN_NtCreateThreadEx)GetProcAddress(modNtDll, "NtCreateThreadEx");

	if (!funNtCreateThreadEx) {
		printf("Failed to get NtCreateThreadEx function address from ntdll.dll\n");
		printf("LastError: 0X%x\n", GetLastError());
		system("PASUE");
		return false;
	}

	NtCreateThreadExBuffer ntBuffer;

	memset(&ntBuffer, 0, sizeof(NtCreateThreadExBuffer));
	LARGE_INTEGER  temp1 = { 0 };
	LARGE_INTEGER  temp2 = { 0 };
	HANDLE hThread = NULL;

	// set function arguements 32 bit
	
	ntBuffer.Size = sizeof(NtCreateThreadExBuffer);
	ntBuffer.Unknown1 = 0x10003;
	ntBuffer.Unknown2 = 0x8;
	ntBuffer.Unknown3 = (DWORD*)&temp2;
	ntBuffer.Unknown4 = 0;
	ntBuffer.Unknown5 = 0x10004;
	ntBuffer.Unknown6 = 4;
	ntBuffer.Unknown7 = (DWORD*)&temp1;
	ntBuffer.Unknown8 = 0;
	


	/*ntBuffer.cbSize = sizeof(NtCreateThreadExBuffer);
	ntBuffer.Unknown = 65539;
	ntBuffer.Unknown2 = 16;
	ntBuffer.UnknownPtr = (N065C26D1*)&temp1;
	ntBuffer.Unknown3 = 0;
	ntBuffer.Unknown4 = 65540;
	ntBuffer.Unknown5 = 8;
	ntBuffer.UnknownPtr2 = (N065C26D1*)&temp1;;
	ntBuffer.Unknown6 = 0;
	*/

	NTSTATUS status = funNtCreateThreadEx(
		&hThread,
		0x1FFFFF,
		NULL,
		hProcess,
		(LPTHREAD_START_ROUTINE)LoadLibraryAddr,
		pDllPath,
		FALSE, //start instantly
		NULL,
		NULL,
		NULL,
		&ntBuffer
	);

	if (!hThread) {
		printf("\n NtCreateThreadEx failed\n");
		printf("LastError: 0X%x", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Thread started with NtCreateThread\n");
	Sleep(2000);
	//system("PAUSE");

	WaitForSingleObject(hThread, INFINITE);

	if (VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE)) {
		//VirtualFreeEx(hProc, reinterpret_cast<int*>(pDllPath) + 0X010000, 0, MEM_RELEASE);
		printf("Memory was freed in target process\n");
		Sleep(2000);
	}

	CloseHandle(hThread);

	CloseHandle(hProcess);

	return true;
}