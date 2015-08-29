#pragma once

#include "common.h"
#include "triangle.h"

struct raycast_tree_node_;
typedef struct raycast_tree_node_ raycast_tree_node;

/// Precomputed data structure for querying whether a segment intersects a set
/// of triangles. Initialize with raycast_init(), query segments with
/// raycast_query() and destroy with raycast_free(). Do not access the
/// structure directly.
typedef struct {
	triangle* trgs;
	raycast_tree_node* tree;
} raycast;

/// Initialize a raycasting data structure.
/// @param trgs The array of triangles.
/// @param trgcount The number of triangles in \p trgs.
/// @returns The raycasting data structure.
raycast raycast_init(triangle* trgs, size_t trgcount);

/// Query whether segment \p a \p b intersects a triangle in the raycasting
/// data structure \p ctx. Does not change the structure, can be called from
/// multiple threads at the same time.
int raycast_query(raycast ctx, vec3 a, vec3 b);

/// Destroys the raycast data structure.
/// @param ctx The raycast data structure to free.
void raycast_free(raycast ctx);
