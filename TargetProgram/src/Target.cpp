#include <iostream>
#include <Windows.h>


int main() {

	/*LPCSTR DllPath = "C:\\Users\\k.rajabzadeh\\source\\repos\\SimpleDllInjector\\DLLFileBuild\\bin\Win32\\Debug\\DLLFile.dll";
	HINSTANCE h = LoadLibrary(DllPath);

	

	if (h) {
		printf("success, 0x%x\n", GetLastError());
		LogMessage("Helllllllloooooooooooo from DLL");
	}
	else {
		printf("failure, 0x%x\n", GetLastError());
	}*/

	while (true) {
		printf("My PID is %d\n", GetCurrentProcessId());
		SleepEx(1000, true);// Call Sleep function with alertable flag to put the thread in suspended state for using in QueueUserAPC method
	}

}