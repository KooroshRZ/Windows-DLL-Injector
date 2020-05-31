#include "injector.h"

bool SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege) {

	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid)) return false;

	// First pass. get current privilege settings
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious);

	if (GetLastError() != ERROR_SUCCESS) return false;

	// second pass. set privileges based on previous settings

	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

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

			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken)) {
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