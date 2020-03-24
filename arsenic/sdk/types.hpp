#pragma once

#include <cstdint>

namespace cheat::sdk {
#ifdef _WIN32
    typedef unsigned int ptr_t;
#else
    typedef std::uintptr_t ptr_t;
#endif

    typedef struct {
        char value[120];
    } SHORT_STRING;

    typedef struct {
        char value[260];
    } LONG_STRING;

    typedef struct {
        float x, y, z;
    } vec3;

    typedef struct {
        int x, y;
    } vec2i;

    typedef float matrix3x4_t[3][4];
}
