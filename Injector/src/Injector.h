#ifndef INJECTOR_H
#define INJECOTR_H

#define RTN_OK		0
#define RTN_USAGE	1
#define RTN_ERROR	13

#include <stdio.h>
#include <Windows.h>
#include <tlHelp32.h>


#define DEBUG_NTBUFFER

bool GetOSInfo();
bool SetPrivilege(HANDLE hToken, LPCTSTR Privilege, BOOL bEnablePrivilege);
void DisplayError(LPCSTR szAPI);
int EscalatePrivilege();

// tech 1 ---> CreateRemoteThread
bool CreateRemoteThread_Type1(LPCSTR DllPath, HANDLE hProcess);


// tech 2 ---> NtCreateThreadEx
bool NtCreateThreadEx_Type2(LPCSTR DllPath, HANDLE hProcess);

#ifdef DEBUG_NTBUFFER

struct NtCreateThreadExBuffer
{
	SIZE_T	Size;
	SIZE_T	Unknown1;
	SIZE_T	Unknown2;
	PULONG	Unknown3;
	SIZE_T	Unknown4;
	SIZE_T	Unknown5;
	SIZE_T	Unknown6;
	PULONG	Unknown7;
	SIZE_T	Unknown8;
};

#endif

typedef NTSTATUS(WINAPI* LPFUN_NtCreateThreadEx)(

	OUT		PHANDLE				hThread,
	IN		ACCESS_MASK			DesiredAccess,
	IN		LPVOID				ObjectAttributes,
	IN		HANDLE				ProcessHandle,
	IN		LPVOID				lpStartAddress,
	IN		LPVOID				lpParameter,
	IN		ULONG				CreateSuspended,
	IN		SIZE_T				StackZeroBits,
	IN		SIZE_T				SizeOfStackCommit,
	IN		SIZE_T				SizeOfStackReserve,
	OUT		LPVOID				lpBytesBuffer
);

// tech 3 ---> QueueUserAPC
bool QueueUserAPC_Type3(LPCSTR DllPath, HANDLE hProcess, DWORD processId);

#endif