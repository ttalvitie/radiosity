/// \file
#pragma once

#include "common.h"
#include "stdint.h"

/// A context for drawing a set of triangles and creating a BMP image out of
/// them. Create with bmp_start(), draw triangles with bmp_draw() and finalize
/// with bmp_write(). Do not access the structure directly.
typedef struct {
	size_t width;
	size_t height;
	size_t pitch;
	size_t size;
	uint8_t* data;
} bmp_canvas;

/// Creates a BMP context.
/// \param width Width of the image.
/// \param height Height of the image.
bmp_canvas bmp_start(size_t width, size_t height);

/// Writes a BMP containing everything drawn to a context. Destroys the context.
/// \param canvas The context to write and free.
/// \param filename The name of the file to write to.
void bmp_write(bmp_canvas canvas, const char* filename);

/// Draws triangle (\p x1, \p y1), (\p x2, \p y2), (\p x3, \p y3) to \p canvas
/// with colors (\p r, \p g, \p b).
void bmp_draw(
	bmp_canvas canvas,
	float x1, float y1, float x2, float y2, float x3, float y3,
	uint8_t r, uint8_t g, uint8_t b
);
