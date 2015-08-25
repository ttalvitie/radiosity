#pragma once

#include "common.h"
#include "vector.h"

/// Structure representing a triangle in 3D, given by three points
typedef struct {
	/// The corner points of the triangle. Only the side from which the corner
	/// points are oriented counter-clockwise is active, i.e. it emits and
	/// reflects light.
	vector corners[3];
	
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
static inline vector triangle_normal(triangle trg) {
	return vector_cross(
		vector_sub(trg.corners[2], trg.corners[0]),
		vector_sub(trg.corners[1], trg.corners[0])
	);
}

/// Normalize the radiosity values of all triangles in an array to the range
/// [0, 1].
/// @param trgs The array of triangles.
/// @param trgcount Number of triangles in \p trgs.
void normalize_triangle_radiosities(triangle* trgs, size_t trgcount);

/// Reads a list of triangles from a text file. The triangles are read as tuples
/// of 11 floating point values: first the coordinates of the corners, then
/// reflectivity, and then emitted energy. Radiosity is set to 0.
/// @param filename The name of the file to read from.
/// @param output Pointer to the pointer that will be set to the array of the
///               output triangles. The array must be freed with \p free.
/// @returns The number of triangles read.
size_t read_triangles_from_file(const char* filename, triangle** output);
