#ifndef INJECTOR_H
#define INJECOTR_H

#include <stdio.h>
#include <Windows.h>
#include <tlHelp32.h>


bool GetOSInfo();

// tech 1 ---> CreateRemoteThread
bool CreateRemoteThread_Type1(LPCSTR DllPath, DWORD PID);


// tech 2 ---> NtCreateThreadEx
bool NtCreateThreadEx_Type2(LPCSTR DllPath, DWORD PID);


/*struct NtCreateThreadExBuffer {
	ULONG		Size;
	ULONG		Unknown1;
	ULONG		Unknown2;
	PULONG		Unknown3;
	ULONG		Unknown4;
	ULONG		Unknown5;
	ULONG		Unknown6;
	PULONG		Unknown7;
	ULONG		Unknown8;
};*/

struct N065C26D1 {
	char buf[64];
};

struct NtCreateThreadExBuffer
{
	__int64 cbSize; //0x0000 => 72(sizeof struct)
	__int64 Unknown; //0x0008 => 65539
	__int64 Unknown2; //0x0010 => 16
	N065C26D1* UnknownPtr; //0x0018 => pointer to zero memory.
	__int64 Unknown3; //0x0020 => zero
	__int64 Unknown4; //0x0028 => 65540
	__int64 Unknown5; //0x0030  => 8
	N065C26D1* UnknownPtr2; //0x0038 => pointer to zero memory(64b).
	__int64 Unknown6; //0x0040 => zero

};//Size=0x0048

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

#endif