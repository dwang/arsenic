#pragma once

#include <cstdint>
#include <cstddef>

typedef void* HANDLE, * PVOID;
typedef long NTSTATUS;

namespace cheat::remote {
	bool attach(const wchar_t*);
	void detach();
	bool exists();

	std::uintptr_t find_module(const wchar_t*);
	std::uintptr_t find_export(std::uintptr_t, const char*);

	NTSTATUS read(std::uintptr_t, void*, std::size_t);
	NTSTATUS write(std::uintptr_t, void*, std::size_t);

	template<typename t>
	inline t read(std::uintptr_t address) {
		t v;
		read(address, &v, sizeof(t));
		return v;
	}

	template<typename t>
	inline t read_ptr(std::uintptr_t address) {
		return read<t>(read<std::uintptr_t>(address));
	}

	template<typename t>
	inline t read_ptr32(std::uintptr_t address) {
		return read<t>(read<std::uint32_t>(address));
	}

	template<typename t>
	inline NTSTATUS write(std::uintptr_t address, t value) {
		return write(address, &value, sizeof(t));
	}

	template <typename t>
	inline NTSTATUS write_ptr(std::uintptr_t address, t value) {
		return write<t>(read<std::uintptr_t>(address), value);
	}

	template <typename t>
	inline NTSTATUS write_ptr32(std::uintptr_t address, t value) {
		return write<t>(read<std::uint32_t>(address), value);
	}
}
