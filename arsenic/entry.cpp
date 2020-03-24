#include "main/main.hpp"

BOOL WINAPI DllMain(const HINSTANCE instance, const DWORD reason, const LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls(instance);

		const auto thread = CreateThread(nullptr, NULL, cheat::main::attach, instance, NULL, nullptr);

		if (thread != nullptr)
			CloseHandle(thread);
	} break;

	case DLL_PROCESS_DETACH: {
		if (!reserved)
			cheat::main::detach();
	} break;

	default: break;
	}

	return TRUE;
}
