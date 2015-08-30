/// \file
#pragma once

#include "common.h"
#include "triangle.h"

/// Draw triangles colored by their radiosity values to a BMP file.
/// The camera faces from (0, 0, 0) towards the positive z-axis.
/// \param filename The name of the BMP file to write to.
/// \param trgs The array of triangles.
/// \param trgcount The number of triangles in \p trgs.
void draw_to_bmp(const char* filename, triangle* trgs, size_t trgcount);
