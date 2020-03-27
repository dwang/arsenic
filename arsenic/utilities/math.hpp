#pragma once

#include "../sdk/types.hpp"

#define RAD2DEG(x) (static_cast<float>(x) * static_cast<float>(180.0f / 3.14159265358979323846f))
#define DEG2RAD(x) (static_cast<float>(x) * static_cast<float>(3.14159265358979323846f / 180.0f))

namespace cheat::math {
	extern float sincos(float radians, float* sine, float* cosine);
	extern void angle_vec(sdk::vec3 angles, sdk::vec3* forward);
	extern float vec_dot(sdk::vec3 v0, sdk::vec3 v1);
	extern float vec_length(sdk::vec3 v);
	extern sdk::vec3 vec_sub(sdk::vec3 p0, sdk::vec3 p1);
	extern float vec_length_sqrt(sdk::vec3 p0);
	extern sdk::vec3 vec_delta(sdk::vec3 p0, sdk::vec3 p1);
	extern float vec_distance(sdk::vec3 p0, sdk::vec3 p1);
	extern void vec_clamp(sdk::vec3* v);
	extern void vec_normalize(sdk::vec3* vec);
	extern sdk::vec3 vec_transform(sdk::vec3 p0, sdk::matrix3x4_t p1);
	extern sdk::vec3 vec_atd(sdk::vec3 vangle);
	extern bool vec_min_max(sdk::vec3 eye, sdk::vec3 dir, sdk::vec3 min, sdk::vec3 max, float radius);
	extern void vec_angles(sdk::vec3 forward, sdk::vec3* angles);
}
