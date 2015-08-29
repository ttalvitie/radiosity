#include "bmp.h"

#include <string.h>

bmp_canvas bmp_start(size_t width, size_t height) {
	bmp_canvas canvas;
	canvas.width = width;
	canvas.height = height;
	if(width > SIZE_MAX / 3) fail("bmp_start: Image too large.");
	
	canvas.pitch = 3 * canvas.width;
	while(canvas.pitch % 4) {
		++canvas.pitch;
		if(!canvas.pitch) fail("bmp_start: Image too large.");
	}
	
	if(canvas.pitch != 0 && height > SIZE_MAX / canvas.pitch) {
		fail("bmp_start: Image too large.");
	}
	
	canvas.size = canvas.pitch * height;
	canvas.data = checked_malloc(canvas.size);
	memset(canvas.data, 0, canvas.size);
	
	return canvas;
}

static inline void checked_fputc(int c, FILE* stream) {
	if(fputc(c, stream) == EOF) {
		fail("Could not write BMP file: checked_fputc() failed.");
	}
}

void bmp_write(bmp_canvas canvas, const char* filename) {
	FILE* fp = fopen(filename, "wb");
	if(!fp) fail("Failed to open output BMP file '%s' for writing.", filename);
	
	// Write header.
	
	// ID
	checked_fputc(0x42, fp);
	checked_fputc(0x4D, fp);
	
	// Size
	uint32_t tmp_u32 = canvas.size;
	if((size_t)tmp_u32 != canvas.size) fail("bmp_write: Image too large.");
	tmp_u32 += 54;
	if(tmp_u32 < 54) fail("bmp_write: Image too large.");
	for(int i = 0; i < 4; ++i) {
		checked_fputc(tmp_u32 & (uint32_t)0xFF, fp);
		tmp_u32 >>= 8;
	}
	
	// Unused
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// Pixel data offset (54 bytes = header size).
	checked_fputc(54, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// DIB header size (40 bytes)
	checked_fputc(40, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// Width
	tmp_u32 = canvas.width;
	for(int i = 0; i < 4; ++i) {
		checked_fputc(tmp_u32 & (uint32_t)0xFF, fp);
		tmp_u32 >>= 8;
	}
	
	// Height
	tmp_u32 = canvas.height;
	for(int i = 0; i < 4; ++i) {
		checked_fputc(tmp_u32 & (uint32_t)0xFF, fp);
		tmp_u32 >>= 8;
	}
	
	// Color plane count (1).
	checked_fputc(1, fp);
	checked_fputc(0, fp);
	
	// Bits per pixel (24).
	checked_fputc(24, fp);
	checked_fputc(0, fp);
	
	// No compression
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// Size of the bitmap data.
	tmp_u32 = canvas.size;
	for(int i = 0; i < 4; ++i) {
		checked_fputc(tmp_u32 & (uint32_t)0xFF, fp);
		tmp_u32 >>= 8;
	}
	
	// 72 DPI
	checked_fputc(0x13, fp);
	checked_fputc(0x0B, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0x13, fp);
	checked_fputc(0x0B, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// Palette size (0)
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// Number of important colors in palette (0)
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	checked_fputc(0, fp);
	
	// Image data
	if(fwrite(canvas.data, 1, canvas.size, fp) != canvas.size) {
		fail("Could not write image data to BMP output file.");
	}
	
	if(fclose(fp)) fail("Could not close BMP output file.");
	
	free(canvas.data);
}

size_t clamp_to_size_t(float x, size_t limit) {
	if(x < 0.0) return 0;
	if(x > (float)limit) return limit;
	size_t ret = (size_t)x;
	if(ret > limit) ret = limit;
	return ret;
}

void bmp_draw(
	bmp_canvas canvas,
	float x1, float y1, float x2, float y2, float x3, float y3,
	uint8_t r, uint8_t g, uint8_t b
) {
	typedef struct {
		float x;
		float y;
	} vec2;
	
	vec2 v[3];
	v[0].x = x1;
	v[0].y = y1;
	v[1].x = x2;
	v[1].y = y2;
	v[2].x = x3;
	v[2].y = y3;
	
	// Sort by y using insertion sort.
	for(int i = 1; i < 3; ++i) {
		for(int j = i - 1; j >= 0; --j) {
			if(v[j].y > v[j + 1].y) {
				vec2 tmp = v[j];
				v[j] = v[j + 1];
				v[j + 1] = tmp;
			}
		}
	}
	
	size_t y_lo = clamp_to_size_t(v[0].y, canvas.height);
	size_t y_hi = clamp_to_size_t(v[2].y + 1.0, canvas.height);
	
	for(size_t y = y_lo; y < y_hi; ++y) {
		float yd = (float)y;
		if(!(yd >= v[0].y && yd < v[2].y)) continue;
		
		float t = (y - v[0].y) / (v[2].y - v[0].y);
		float x1 = v[0].x + t * (v[2].x - v[0].x);
		
		float x2;
		if(yd < v[1].y) {
			t = (y - v[0].y) / (v[1].y - v[0].y);
			x2 = v[0].x + t * (v[1].x - v[0].x);
		} else {
			t = (y - v[1].y) / (v[2].y - v[1].y);
			x2 = v[1].x + t * (v[2].x - v[1].x);
		}
		if(x1 > x2) {
			float tmp = x1;
			x1 = x2;
			x2 = tmp;
		}
		
		size_t x_lo = clamp_to_size_t(x1, canvas.width);
		size_t x_hi = clamp_to_size_t(x2 + 1.0, canvas.width);
		
		size_t pos = y * canvas.pitch + 3 * x_lo;
		for(size_t x = x_lo; x < x_hi; ++x, pos += 3) {
			float xd = (float)x;
			if(!(xd >= x1 && xd < x2)) continue;
			canvas.data[pos + 2] = r;
			canvas.data[pos + 1] = g;
			canvas.data[pos + 0] = b;
		}
	}
}
