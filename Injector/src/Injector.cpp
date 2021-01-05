#include "Injector.h"

#define ProcType_specified

DWORD GetProcId(const char* procName)
{
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_stricmp(procEntry.szExeFile, procName))
				{
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	else
	{
		printf("CreateToolhelp32Snapshot failed!");
		printf("LastError : 0x%x\n", GetLastError());
		system("PAUSE");
		return 0;
	}
	CloseHandle(hSnap);
	return procId;
}

// variables for Privilege Escalation
HANDLE hToken;
int dwRetVal = RTN_OK;

int main() {

#ifdef _WIN64
	LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\Windows-DLL-Injector\\PayloadDLLBuild\\bin\\Debug\\Win32\\PayloadDLL.dll";
	#ifdef ProcType_specified
	const char* szProc = "some-exe.exe";
	#else
	char szProc[80];

	printf("Enter 64bit Target process name : ");
	scanf_s("%79s", szProc, 79);
	#endif
#else
	LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\Windows-DLL-Injector\\PayloadDLLBuild\\bin\\Debug\\Win32\\PayloadDLL.dll";
	#ifdef ProcType_specified
	const char* szProc = "some-exe.exe";
	#else
	char szProc[80];

	printf("Enter Target process name : ");
	scanf_s("%79s", szProc, 79);
	#endif
#endif

	if (!GetOSInfo()) {
		printf("Failed to get Windows NT version\n");
		printf("LastError: 0x%x\n", GetLastError());
	}
	
	printf("escalating Privileges...\n");
	Sleep(2000);
	int epResult = EscalatePrivilege();
	printf("Result of Privilege Escalation : %d\n", epResult);

	if (epResult == RTN_OK)
		printf("Successfully Escalated privileges to SYSTEM level...\n");

	DWORD PID = 0;
	while (!PID)
	{
		PID = GetProcId(szProc);
		Sleep(30);
	}

	printf("Target Program PID: %d\n\n", PID);

	int InjectionMethod = -1;

	printf("\n\n");
	printf("   1) CreateRemoteThread\n");
	printf("   2) NtCreateThread\n");
	printf("   3) QueueUserAPC\n");
	printf("   4) SetWindowsHookEx\n");
	printf("   5) RtlCreateUserThread\n");
	printf("\n\n\n");
	printf("Enter the Injection method: ");
	scanf("%d", &InjectionMethod);

	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_CREATE_THREAD |
		PROCESS_VM_OPERATION |
		PROCESS_VM_WRITE,
		FALSE, PID);	

	//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	if (!hProcess) {
		printf("Could not open Process for PID %d\n", PID);
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}
	
	// disable SeDebugPrivilege
	SetPrivilege(hToken, SE_DEBUG_NAME, FALSE);

	// close handles
	CloseHandle(hToken);

	switch (InjectionMethod)
	{
		case 1:
			CreateRemoteThread_Type1(DllPath, hProcess);
			break;
		case 2:
			NtCreateThreadEx_Type2(DllPath, hProcess);
			break;
		case 3:
			QueueUserAPC_Type3(DllPath, hProcess, PID);
			break;
		case 4:
			SetWindowsHookEx_type4(PID, DllPath);
			break;
		case 5:
			RtlCreateUsreThread_type5(hProcess, DllPath);
			break;
		default:
			printf("Choose a valid mathod\n");
			break;
	}
	

	CloseHandle(hProcess);

	if (!TerminateProcess(hProcess, 0xffffffff))
	{
		DisplayError("TerminateProcess");
		dwRetVal = RTN_ERROR;
	}

	return 0;
}
