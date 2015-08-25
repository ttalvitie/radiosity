#pragma once

#include "common.h"

/// Structure representing one vector in 3D.
typedef struct {
	/// The coordinates of the vector.
	double x, y, z;
} vector;

/// Returns the sum of vectors \p a and \p b.
static inline vector vector_add(vector a, vector b) {
	vector ret;
	ret.x = a.x + b.x;
	ret.y = a.y + b.y;
	ret.z = a.z + b.z;
	return ret;
}

/// Returns the difference of vectors \p a and \p b.
static inline vector vector_sub(vector a, vector b) {
	vector ret;
	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;
	return ret;
}

/// Returns the difference of vector \p v.
static inline vector vector_neg(vector v) {
	vector ret;
	ret.x = -v.x;
	ret.y = -v.y;
	ret.z = -v.z;
	return ret;
}

/// Returns the vector \p v multiplied by scalar \p c.
static inline vector vector_mul(vector v, double c) {
	v.x *= c;
	v.y *= c;
	v.z *= c;
	return v;
}

/// Computes the cross product of vectors \p a and \p b.
static inline vector vector_cross(vector a, vector b) {
	vector ret;
	ret.x = a.y * b.z - a.z * b.y;
	ret.y = a.z * b.x - a.x * b.z;
	ret.z = a.x * b.y - a.y * b.x;
	return ret;
}

/// Computes the dot product of vectors \p a and \p b.
static inline double vector_dot(vector a, vector b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
