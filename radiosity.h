/// \file
#pragma once

#include "common.h"
#include "triangle.h"
#include "raycast.h"

/// Recompute the radiosity values to a scene consisting of an array of
/// triangles.
/// \param trgs The array of triangles.
/// \param trgcount Number of triangles in \p trgs.
/// \param raycast_ctx The raycasting structure used for collision detection.
void compute_radiosity(
	triangle* trgs, size_t trgcount,
	raycast raycast_ctx
);
