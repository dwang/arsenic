#include "math.hpp"

#include <cmath>

namespace cheat::math {
	void sincos(float radians, float* sine, float* cosine) {
		*sine = static_cast<float>(sin(radians));
		*cosine = static_cast<float>(cos(radians));
	}

	void angle_vec(sdk::vec3 angles, sdk::vec3* forward) {
		float sp, sy, cp, cy;

		sincos(DEG2RAD(angles.x), &sp, &cp);
		sincos(DEG2RAD(angles.y), &sy, &cy);
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	float vec_dot(sdk::vec3 v0, sdk::vec3 v1) {
		return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z);
	}

	float vec_length(sdk::vec3 v) {
		return (v.x * v.x + v.y * v.y + v.z * v.z);
	}

	sdk::vec3 vec_sub(sdk::vec3 p0, sdk::vec3 p1) {
		sdk::vec3 r;

		r.x = p0.x - p1.x;
		r.y = p0.y - p1.y;
		r.z = p0.z - p1.z;

		return r;
	}

	float vec_length_sqrt(sdk::vec3 p0) {
		return static_cast<float>(sqrt(p0.x * p0.x + p0.y * p0.y + p0.z * p0.z));
	}

	sdk::vec3 vec_delta(sdk::vec3 p0, sdk::vec3 p1) {
		sdk::vec3 d;
		float l;

		d = vec_sub(p0, p1);
		l = static_cast<float>(vec_length_sqrt(d));
		d.x /= l; d.y /= l; d.z /= l;

		return d;
	}

	float vec_distance(sdk::vec3 p0, sdk::vec3 p1) {
		return vec_length_sqrt(vec_sub(p0, p1));
	}

	void vec_clamp(sdk::vec3* v) {
		if (v->x > 89.0f && v->x <= 180.0f) {
			v->x = 89.0f;
		}

		if (v->x > 180.0f) {
			v->x = v->x - 360.0f;
		}

		if (v->x < -89.0f) {
			v->x = -89.0f;
		}

		v->y = fmodf(v->y + 180, 360) - 180;
		v->z = 0;
	}

	void vec_normalize(sdk::vec3* vec) {
		float radius;

		radius = 1.f / static_cast<float>(sqrt(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z) + 1.192092896e-07f);
		vec->x *= radius, vec->y *= radius, vec->z *= radius;
	}

	sdk::vec3 vec_transform(sdk::vec3 p0, sdk::matrix3x4_t p1) {
		sdk::vec3 v;

		v.x = (p0.x * p1[0][0] + p0.y * p1[0][1] + p0.z * p1[0][2]) + p1[0][3];
		v.y = (p0.x * p1[1][0] + p0.y * p1[1][1] + p0.z * p1[1][2]) + p1[1][3];
		v.z = (p0.x * p1[2][0] + p0.y * p1[2][1] + p0.z * p1[2][2]) + p1[2][3];

		return v;
	}

	sdk::vec3 vec_atd(sdk::vec3 vangle) {
		double y[2], p[2];

		vangle.x *= (3.14159265358979323846f / 180.0f);
		vangle.y *= (3.14159265358979323846f / 180.0f);
		y[0] = sin(vangle.y), y[1] = cos(vangle.y);
		p[0] = sin(vangle.x), p[1] = cos(vangle.x);
		vangle.x = static_cast<float>(p[1] * y[1]);
		vangle.y = static_cast<float>(p[1] * y[0]);
		vangle.z = static_cast<float>(-p[0]);

		return vangle;
	}

	bool vec_min_max(sdk::vec3 eye, sdk::vec3 dir, sdk::vec3 min, sdk::vec3 max, float radius) {
		sdk::vec3 delta;
		std::uint32_t i;
		sdk::vec3 q;
		float v;

		delta = vec_delta(max, min);

		for (i = 0; i < vec_distance(min, max); i++) {
			q.x = min.x + delta.x * static_cast<float>(i) - eye.x;
			q.y = min.y + delta.y * static_cast<float>(i) - eye.y;
			q.z = min.z + delta.z * static_cast<float>(i) - eye.z;
			if ((v = vec_dot(q, dir)) < 1.0f) {
				return false;
			}

			v = radius * radius - (vec_length(q) - v * v);

			if (v <= -100.f) {
				return false;
			}

			if (v >= 1.19209290E-07F) {
				return true;
			}
		}

		return false;
	}

	void vec_angles(sdk::vec3 forward, sdk::vec3* angles) {
		float tmp, yaw, pitch;

		if (forward.y == 0.f && forward.x == 0.f) {
			yaw = 0;

			if (forward.z > 0) {
				pitch = 270;
			}
			else {
				pitch = 90.f;
			}
		}
		else {
			yaw = static_cast<float>(atan2(forward.y, forward.x) * 180.f / 3.14159265358979323846f);

			if (yaw < 0) {
				yaw += 360.f;
			}

			tmp = static_cast<float>(sqrt(forward.x * forward.x + forward.y * forward.y));
			pitch = static_cast<float>(atan2(-forward.z, tmp) * 180.f / 3.14159265358979323846f);

			if (pitch < 0) {
				pitch += 360.f;
			}
		}

		angles->x = pitch;
		angles->y = yaw;
		angles->z = 0.f;
	}
}
