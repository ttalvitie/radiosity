#pragma once

#include "common.h"
#include "vec3.h"

/// Structure representing a triangle in 3D, given by three points
typedef struct {
	/// The corner points of the triangle. Only the side from which the corner
	/// points are oriented counter-clockwise is active, i.e. it emits and
	/// reflects light.
	vec3 corners[3];
	
	/// The reflectivity of the triangle, i.e. how big portion of the light
	/// received by the triangle is emitted away.
	double reflectivity;
	
	/// The amount of light emitted by the triangle.
	double emitted_energy;
	
	/// The radiosity value, that is, the total lighting of the triangle.
	double radiosity;
} triangle;

/// Computes a normal of triangle \p trg towards the active side of the
/// triangle.
static inline vec3 triangle_normal(triangle trg) {
	return vec3_cross(
		vec3_sub(trg.corners[2], trg.corners[0]),
		vec3_sub(trg.corners[1], trg.corners[0])
	);
}
/// Computes the centroid of triangle \p trg.
static inline vec3 triangle_centroid(triangle trg) {
	vec3 ret = vec3_add(trg.corners[0], trg.corners[1]);
	ret = vec3_add(ret, trg.corners[2]);
	ret = vec3_mul(ret, 1.0 / 3.0);
	return ret;
}

/// Normalize the radiosity values of all triangles in an array to the range
/// [0, 1].
/// \param trgs The array of triangles.
/// \param trgcount Number of triangles in \p trgs.
void normalize_triangle_radiosities(triangle* trgs, size_t trgcount);

/// Reads a list of triangles from a text file. The triangles are read as tuples
/// of 11 floating point values: first the coordinates of the corners, then
/// reflectivity, and then emitted energy. Radiosity is set to 0.
/// \param filename The name of the file to read from.
/// \param edge_length_limit Limit for triangle edge lengths. If this value is
///                          positive, triangles with longer edges will be
///                          subdivided.
/// \param output Pointer to the pointer that will be set to the array of the
///               output triangles. The array must be freed with \p free.
/// \returns The number of triangles read.
size_t read_triangles_from_file(
	const char* filename,
	double edge_length_limit,
	triangle** output
);
