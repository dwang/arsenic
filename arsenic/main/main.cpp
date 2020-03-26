#include "main.hpp"

#include "../sdk/convars.hpp"
#include "../sdk/interfaces.hpp"
#include "../sdk/modules.hpp"
#include "../sdk/netvars.hpp"
#include "../sdk/offsets.hpp"
#include "../utilities/remote.hpp"

#include "../dependencies/lazy_importer/lazy_importer.hpp"
#include "../dependencies/xorstr/xorstr.hpp"

#include <thread>

namespace cheat::main {
	BOOL WINAPI detach() {
		if (remote::exists()) {
			remote::detach();
		}

		fclose(reinterpret_cast<FILE*>(stdout));
		LI_FN(FreeConsole).safe()();

		return TRUE;
	}

	DWORD WINAPI attach(const LPVOID thread) {
		LI_FN(AllocConsole).safe()();
		freopen_s(reinterpret_cast<FILE**>(stdout), xorstr_("CONOUT$"), xorstr_("w"), stdout);

		printf(xorstr_("> arsenic\n\n"));

		printf(xorstr_("> waiting for \"csgo.exe\"\n\n"));

		while (!remote::attach(xorstr_(L"csgo.exe"))) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		printf(xorstr_("> waiting for modules\n\n"));

		while (remote::find_module(xorstr_(L"serverbrowser.dll")) == 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		sdk::modules::initialize();

		printf(xorstr_("> setting up interfaces\n\n"));

		sdk::interfaces::initialize();

		printf(xorstr_("> finding offsets\n\n"));

		sdk::offsets::initialize();

		printf(xorstr_("> finding netvars\n\n"));

		sdk::netvars::initialize();

		printf(xorstr_("> setting up convars\n\n"));

		sdk::cvar::initialize();

		printf(xorstr_("> success\n"));

		while (!LI_FN(GetAsyncKeyState).safe_cached()(VK_END) && remote::exists()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		detach();

		LI_FN(FreeLibraryAndExitThread).safe()(static_cast<HMODULE>(thread), EXIT_SUCCESS);

		return TRUE;
	}
}
