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

	
	CreateRemoteThread_Type1(DllPath, PID);
	system("PAUSE");
	//NtCreateThreadEx_Type2(DllPath, PID);*/

	return 0;
}


bool CreateRemoteThread_Type1(LPCSTR DllPath, DWORD PID) {

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (!hProc) {
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

	LPVOID pDllPath = VirtualAllocEx(hProc, NULL, strlen(DllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!pDllPath) {
		printf("Could not allocate Memory in target process\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory allocated at: 0X%x\n", pDllPath);
	Sleep(2000);

	BOOL Written = WriteProcessMemory(hProc, pDllPath, (LPVOID)DllPath, strlen(DllPath), NULL);

	if (!Written) {
		printf("Could not write into the allocated memory\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, pDllPath, 0, NULL);

	if (!hThread) {
		printf("Could not open Thread with CreatRemoteThread API\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}
	printf("Thread started with CreateRemoteThread\n");
	Sleep(2000);

	WaitForSingleObject(hThread, INFINITE);

	if (VirtualFreeEx(hProc, pDllPath, 0, MEM_RELEASE)) {
		//VirtualFreeEx(hProc, reinterpret_cast<int*>(pDllPath) + 0X010000, 0, MEM_RELEASE);
		printf("Memory was freed in target process\n");
		Sleep(2000);
	}

	return true;
}

bool NtCreateThreadEx_Type2(LPCSTR DllPath, DWORD PID) {
	return true;
}