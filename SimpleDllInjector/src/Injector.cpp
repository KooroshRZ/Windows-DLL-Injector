#include "Injector.h"

int main() {

	LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\SimpleDllInjector\\PayloadDLLBuild\\bin\\Win32\\Debug\\PayloadDLL.dll";
	
	char szProc[80];

	printf("Target process name : ");
	scanf_s("%79s", szProc, 79);

	PROCESSENTRY32 PE32{ 0 };
	PE32.dwSize = sizeof(PE32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot failed: 0X%x\n", GetLastError());
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return 0;
	}

	DWORD PID = 0;
	BOOL bRet = Process32First(hSnap, &PE32);

	while (bRet) {

		if (!strcmp((LPCSTR)szProc, PE32.szExeFile)) {
			PID = PE32.th32ProcessID;
			break;
		}
		bRet = Process32Next(hSnap, &PE32);
	}

	CloseHandle(hSnap);

	printf("Target Program PID: %d\n", PID);
	Sleep(2000);

	
	//CreateRemoteThread_Type1(DllPath, PID);
	NtCreateThreadEx_Type2(DllPath, PID);
	system("PAUSE");
	return 0;
}


bool CreateRemoteThread_Type1(LPCSTR DllPath, DWORD PID) {

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (!hProcess) {
		printf("Could not open Process for PID %d\n", PID);
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	if (!LoadLibAddr) {
		printf("Could note locate real address of LoadLibraryA!\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("LoadLibraryA is located at real address: 0X%x\n", LoadLibAddr);
	Sleep(2000);

	LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(DllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath) {
		printf("Could not allocate Memory in target process\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory allocated at: 0X%x\n", pDllPath);
	Sleep(2000);

	BOOL Written = WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath), NULL);

	if (!Written) {
		printf("Could not write into the allocated memory\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

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

	CloseHandle(hProcess);

	return true;
}

bool NtCreateThreadEx_Type2(LPCSTR DllPath, DWORD PID) {

	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_CREATE_THREAD |
		PROCESS_VM_OPERATION |
		PROCESS_VM_WRITE,
		FALSE, PID);

	LPVOID LoadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	if (!LoadLibraryAddr) {
		printf("Could note get real address of LoadLibraryA from kernel32.dll!\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("LoadLibraryA is located at real address: 0X%x\n", LoadLibraryAddr);
	Sleep(2000);

	LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(DllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath) {
		printf("Could not allocate Memory in target process\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory allocated at: 0X%x\n", pDllPath);
	Sleep(2000);

	BOOL Written = WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath, strlen(DllPath), NULL);

	if (!Written) {
		printf("Could not write into the allocated memory\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

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
	HANDLE hThread;

	// set function arguements
	ntBuffer.Size = sizeof(NtCreateThreadExBuffer);
	ntBuffer.Unknown1 = 0x10003;
	ntBuffer.Unknown2 = 0x8;
	ntBuffer.Unknown3 = (DWORD*)&temp2;
	ntBuffer.Unknown4 = 0;
	ntBuffer.Unknown5 = 0x10004;
	ntBuffer.Unknown6 = 4;
	ntBuffer.Unknown7 = (DWORD*)&temp1;
	ntBuffer.Unknown8 = 0;

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

	WaitForSingleObject(hThread, INFINITE);

	if (VirtualFreeEx(hProcess, pDllPath, 0, MEM_RELEASE)) {
		//VirtualFreeEx(hProc, reinterpret_cast<int*>(pDllPath) + 0X010000, 0, MEM_RELEASE);
		printf("Memory was freed in target process\n");
		Sleep(2000);
	}

	//CloseHandle(hThread);

	//CloseHandle(hProcess);

	return true;
}
