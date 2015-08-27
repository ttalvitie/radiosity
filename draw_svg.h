/// @file
#pragma once

#include "common.h"
#include "triangle.h"

/// Draw triangles colored by their radiosity values to a canvas on a HTML file.
/// The camera faces from (0, 0, 0) towards the positive z-axis.
/// \param filename The name of the HTML file to write to.
/// \param trgs The array of triangles.
/// \param trgcount The number of triangles in \p trgs.
void draw_to_svg(const char* filename, triangle* trgs, size_t trgcount);
