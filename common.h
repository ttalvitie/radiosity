/// @file
#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define PI 3.14159265358979323846

/// Simple error handler that outputs an error message given in the style of
/// printf() and aborts.
void fail(const char *format, ...);

/// Calls malloc, but if it fails, calls fail().
static inline void* checked_malloc(size_t size) {
	void* ret = malloc(size);
	if(!ret && size) fail("Could not allocate memory: call to malloc failed.");
	return ret;
}
/// Calls malloc with value \p a * \p b, but if the product overflows or malloc
/// fails, calls fail().
static inline void* checked_malloc2(size_t a, size_t b) {
	if(a != 0 && b > SIZE_MAX / a) {
		fail("Could not allocate memory: size does not fit in size_t.");
	}
	return checked_malloc(a * b);
}
/// Calls malloc with value \p a * \p b * \p c, but if the product overflows or
/// malloc fails, calls fail().
static inline void* checked_malloc3(size_t a, size_t b, size_t c) {
	if(a != 0 && b > SIZE_MAX / a) {
		fail("Could not allocate memory: size does not fit in size_t.");
	}
	return checked_malloc2(a * b, c);
}

/// Calls realloc, but if it fails, calls fail().
static inline void* checked_realloc(void* ptr, size_t size) {
	void* ret = realloc(ptr, size);
	if(!ret && size) fail("Could not allocate memory: call to realloc failed.");
	return ret;
}
/// Calls realloc with parameters (\p ptr, \p a * \p b), but if the product
/// overflows or realloc fails, calls fail().
static inline void* checked_realloc2(void* ptr, size_t a, size_t b) {
	if(a != 0 && b > SIZE_MAX / a) {
		fail("Could not allocate memory: size does not fit in size_t.");
	}
	return checked_realloc(ptr, a * b);
}
