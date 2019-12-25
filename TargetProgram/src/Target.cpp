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
		std::cout << "Doing something!\n";
		Sleep(1000);
	}

}