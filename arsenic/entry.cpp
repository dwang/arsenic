#include "main/main.hpp"

#include "dependencies/lazy_importer/lazy_importer.hpp"

BOOL WINAPI DllMain(const HINSTANCE instance, const DWORD reason, const LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH: {
		LI_FN(DisableThreadLibraryCalls).safe()(instance);

		const auto thread = LI_FN(CreateThread).safe()(nullptr, NULL, cheat::main::attach, instance, NULL, nullptr);

		if (thread != nullptr)
			LI_FN(CloseHandle).safe()(thread);
	} break;

	case DLL_PROCESS_DETACH: {
		if (!reserved)
			cheat::main::detach();
	} break;

	default: break;
	}

	return TRUE;
}
