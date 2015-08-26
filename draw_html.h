#pragma once

#include "common.h"
#include "triangle.h"

/// Draw triangles colored by their radiosity values to a canvas on a HTML file.
/// \param filename The name of the HTML file to write to.
/// \param trgs The array of triangles.
/// \param trgcount The number of triangles in \p trgs.
void draw_to_html(const char* filename, triangle* trgs, size_t trgcount);
