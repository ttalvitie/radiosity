#pragma once

#include "common.h"

/// Structure representing one vector in 3D.
typedef struct {
	/// The coordinates of the vector.
	double x, y, z;
} vec3;

/// Returns the sum of vectors \p a and \p b.
static inline vec3 vec3_add(vec3 a, vec3 b) {
	vec3 ret;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	ret.z = a.z + b.z;
	return ret;
}

/// Returns the difference of vectors \p a and \p b.
static inline vec3 vec3_sub(vec3 a, vec3 b) {
	vec3 ret;
	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;
	return ret;
}

/// Returns the difference of vector \p v.
static inline vec3 vec3_neg(vec3 v) {
	vec3 ret;
	ret.x = -v.x;
	ret.y = -v.y;
	ret.z = -v.z;
	return ret;
}

/// Returns the vector \p v multiplied by scalar \p c.
static inline vec3 vec3_mul(vec3 v, double c) {
	v.x *= c;
	v.y *= c;
	v.z *= c;
	return v;
}

/// Computes the cross product of vectors \p a and \p b.
static inline vec3 vec3_cross(vec3 a, vec3 b) {
	vec3 ret;
	ret.x = a.y * b.z - a.z * b.y;
	ret.y = a.z * b.x - a.x * b.z;
	ret.z = a.x * b.y - a.y * b.x;
	return ret;
}

/// Computes the dot product of vectors \p a and \p b.
static inline double vec3_dot(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

/// Computes the square of the length of vector \p v.
static inline double vec3_len2(vec3 v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}
/// Computes the length of vector \p v.
static inline double vec3_len(vec3 v) {
	return sqrt(vec3_len2(v));
}
