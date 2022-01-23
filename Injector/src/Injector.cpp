#include "Injector.h"

#ifdef _WIN64
	LPCSTR DllPath = "C:\\Users\\PATH\\TO\\DLL_FILE";
#else
	LPCSTR DllPath = "C:\\Users\\PATH\\TO\\DLL_FILE";
#endif

// variables for Privilege Escalation
HANDLE hToken;
int dwRetVal = RTN_OK;

int main() {

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
	
	char szProc[80];

	printf("Target process name : ");
	scanf_s("%79s", szProc, 79);

	PROCESSENTRY32 PE32{ sizeof(PROCESSENTRY32) };
	PE32.dwSize = sizeof(PE32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE) {
		printf("CreateToolhelp32Snapshot failed!");
		printf("LastError : 0x%x\n", GetLastError());
		system("PAUSE");
		return 0;
	}

	DWORD PID = 0;
	BOOL bRet = Process32First(hSnap, &PE32);
	char yn[3];

	while (bRet) {

		//printf("process: %s\n", PE32.szExeFile);
		if (!strcmp((LPCSTR)szProc, PE32.szExeFile)) {

			PID = PE32.th32ProcessID;
			printf("PID found for this process name ---> %d\n", PID);
			printf("Is this OK ? [Input Y to continue with this PID] : ");


			scanf_s("%2s", yn, 2);

			if ( !strcmp((LPCSTR)yn, "y") || !strcmp((LPCSTR)yn, "Y") )
				break;

			printf("\n\n");

		}

		bRet = Process32Next(hSnap, &PE32);
	}

	CloseHandle(hSnap);

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
