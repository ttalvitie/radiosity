/// @file
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
	float reflectivity;
	
	/// The amount of light emitted by the triangle.
	float emitted_energy;
	
	/// The radiosity value, that is, the total lighting of the triangle.
	float radiosity;
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
/// Computes the area of triangle \p trg.
static inline float triangle_area(triangle trg) {
	return 0.5 * vec3_len(vec3_cross(
		vec3_sub(trg.corners[2], trg.corners[0]),
		vec3_sub(trg.corners[1], trg.corners[0])
	));
}

/// Checks whether the segment from \p a to \p b intersects triangle \p trg.
static inline int segment_intersects_triangle(vec3 a, vec3 b, triangle trg) {
	vec3 n = triangle_normal(trg);
	vec3 c = triangle_centroid(trg);
	
	float da = vec3_dot(vec3_sub(a, c), n);
	float db = vec3_dot(vec3_sub(b, c), n);
	if((da > 0.0) == (db > 0.0)) return 0;
	
	float t = da / (da - db);
	if(t < 1e-4 || t > 1.0 - 1e-4) return 0;
	
	vec3 v = vec3_add(vec3_mul(a, 1.0 - t), vec3_mul(b, t));
	
	float whole_area = triangle_area(trg);
	float subs_area = 0.0;
	
	triangle subtrg = trg;
	
	for(int i = 0; i < 3; ++i) {
		subtrg.corners[i] = v;
		subs_area += triangle_area(subtrg);
		subtrg.corners[i] = trg.corners[i];
	}
	return subs_area / whole_area < 1.0 + 1e-4;
}

/// Normalize the radiosity values of all triangles in an array to the range
/// [0, 1].
/// \param trgs The array of triangles.
/// \param trgcount Number of triangles in \p trgs.
void normalize_triangle_radiosities(triangle* trgs, size_t trgcount);

/// Subdivides all triangles in of \p trgs such that they do not have too long
/// edges.
/// \param trgs The array of input triangles.
/// \param trgcount Number of triangles in \p trgs.
/// \param edge_length_limit Limit for triangle edge lengths. If this value is
///                          positive, triangles with longer edges will be
///                          subdivided.
/// \param output Pointer to the pointer that will be set to the array of the
///               output triangles. The array must be freed with \p free.
/// \returns The number of triangles created in \p output.
size_t subdivide_triangles(
	triangle* trgs,
	size_t trgcount,
	float edge_length_limit,
	triangle** output
);

/// Reads a list of triangles from a text file. The triangles are read as tuples
/// of 11 floating point values: first the coordinates of the corners, then
/// reflectivity, and then emitted energy. Radiosity is set to 0.
/// \param filename The name of the file to read from.
/// \param output Pointer to the pointer that will be set to the array of the
///               output triangles. The array must be freed with \p free.
/// \returns The number of triangles read.
size_t read_triangles_from_file(const char* filename, triangle** output);
