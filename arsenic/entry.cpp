#define WIN32_LEAN_AND_MEAN


#include <thread>
#include <Windows.h>

DWORD WINAPI detach() {
	fclose(reinterpret_cast<FILE*>(stdin));
	fclose(reinterpret_cast<FILE*>(stdout));
	FreeConsole();

	return TRUE;
}


DWORD WINAPI attach(const LPVOID thread) {
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	detach();

	FreeLibraryAndExitThread(static_cast<HMODULE>(thread), EXIT_SUCCESS);
}


BOOL WINAPI DllMain(const HINSTANCE instance, const DWORD reason, const LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(instance);

		const auto thread = CreateThread(nullptr, NULL, attach, instance, NULL, nullptr);

		if (thread != nullptr)
			CloseHandle(thread);
	} break;

	case DLL_PROCESS_DETACH: {
		if (!reserved)
			detach();
	} break;

	default: break;
	}

	return TRUE;
}
