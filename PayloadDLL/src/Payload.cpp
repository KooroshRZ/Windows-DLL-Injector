#include "Payload.h"

void LogMessage(const char* message) {

	std::cout << message << std::endl;

}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
) {
	//if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		//MessageBox(0, "Hello I'm inside you GOOGLE CHROME", ":)))", MB_ICONINFORMATION);
		//system("PAUSE");
		//exit(1);
		//LogMessage("Hey I'm injected inside you :)");
	//}

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			//MessageBox(0, "Hello I'm DLL injected inside you !!!", "DLL_PROCESS_ATTACH", MB_ICONINFORMATION);
			std::cout << "Hello I'm injected inside you in DLL_PROCESS_ATTACH mode" << std::endl;
			break;
		case DLL_THREAD_ATTACH:
			//MessageBox(0, "Hello I'm DLL injected inside you GOOGLE CHROME !!!", "DLL_THREAD_ATTACH", MB_ICONINFORMATION);
			std::cout << "Hello I'm injected inside you in DLL_THREAD_ATTACH mode" << std::endl;
			break;
		case DLL_THREAD_DETACH:
			//MessageBox(0, "Hello I'm DLL injected inside you GOOGLE CHROME !!!", "DLL_THREAD_DETACH", MB_ICONINFORMATION);
			std::cout << "Hello I'm injected inside you in DLL_THREAD_DETACH mode" << std::endl;
			break;
		case DLL_PROCESS_DETACH:
			//MessageBox(0, "Hello I'm DLL injected inside you !!!", "DLL_PROCESS_DETACH", MB_ICONINFORMATION);
			std::cout << "Hello I'm injected inside you in DLL_PROCESS_DETACH mode" << std::endl;

			break;
	}

	return TRUE;
}