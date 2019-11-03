#include "Injector.h"

#ifdef _WIN64
	LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\SimpleDllInjector\\PayloadDLLBuild\\bin\\x64\\Debug\\PayloadDLL.dll";
#else
	LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\SimpleDllInjector\\PayloadDLLBuild\\bin\\Win32\\Debug\\PayloadDLL.dll";
#endif

int main() {


	if (!GetOSInfo()) {
		printf("Failed to get Windows NT version\n");
		printf("LastError: 0x%x\n", GetLastError());
	}


	//LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\SimpleDllInjector\\PayloadDLLBuild\\bin\\Win32\\Debug\\PayloadDLL.dll";
	
	char szProc[80];

	printf("Target process name : ");
	scanf_s("%79s", szProc, 79);

	PROCESSENTRY32 PE32{ sizeof(PROCESSENTRY32) };
	PE32.dwSize = sizeof(PE32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot failed!");
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
