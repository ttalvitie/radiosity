/// @file
#pragma once

#include "common.h"
#include "triangle.h"

/// Recompute the radiosity values to a scene consisting of an array of
/// triangles.
/// \param trgs The array of triangles.
/// \param trgcount Number of triangles in \p trgs.
/// \param collision_trgs The array of triangles used for collision detection.
/// \param collision_trgcount Number of triangles in \p collision_trgs.
void compute_radiosity(
	triangle* trgs, size_t trgcount,
	triangle* collision_trgs, size_t collision_trgcount
);
