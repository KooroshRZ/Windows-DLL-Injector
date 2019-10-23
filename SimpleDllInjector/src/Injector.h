#pragma once

#include <stdio.h>
#include <Windows.h>
#include <tlHelp32.h>


// tech 1 ---> CreateRemoteThread
bool CreateRemoteThread_Type1(LPCSTR DllPath, DWORD PID);


// tech 2 ---> NtCreateThreadEx
bool NtCreateThreadEx_Type2(LPCSTR DllPath, DWORD PID);


struct NtCreateThreadExBuffer {
	ULONG		Size;
	ULONG		Unknown1;
	ULONG		Unknown2;
	PULONG		Unknown3;
	ULONG		Unknown4;
	ULONG		Unknown5;
	ULONG		Unknown6;
	PULONG		Unknown7;
	ULONG		Unknown8;
};

typedef NTSTATUS(WINAPI* LPFUN_NtCreateThreadEx)(

	OUT		PHANDLE						hThread,
	IN		ACCESS_MASK					DesiredAccess,
	IN		LPVOID						ObjectAttributes,
	IN		HANDLE						ProcessHandle,
	IN		LPTHREAD_START_ROUTINE		lpStartAddress,
	IN		LPVOID						lpParameter,
	IN		BOOL						CreateSuspended,
	IN		ULONG						StackZeroBits,
	IN		ULONG						SizeOfStackCommit,
	IN		ULONG						SizeOfStackReserve,
	OUT		LPVOID						lpBytesBuffer

);
