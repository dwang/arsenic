#include "main.hpp"

namespace cheat::main {
	BOOL WINAPI detach() {
		if (remote::exists()) {
			remote::detach();
		}

		fclose(reinterpret_cast<FILE*>(stdin));
		fclose(reinterpret_cast<FILE*>(stdout));
		FreeConsole();

		return TRUE;
	}

	DWORD WINAPI attach(const LPVOID thread) {
		AllocConsole();
		freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
		freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

		printf("> arsenic\n\n");

		printf("> waiting for \"csgo.exe\"\n\n");

		while (!remote::attach(L"csgo.exe")) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		printf("> waiting for modules\n\n");

		while (remote::find_module(L"serverbrowser.dll") == 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		sdk::modules::initialize();

		printf("> success\n");

		while (!GetAsyncKeyState(VK_END)) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		detach();

		FreeLibraryAndExitThread(static_cast<HMODULE>(thread), EXIT_SUCCESS);
	}
}
