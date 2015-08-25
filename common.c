#include "common.h"

#include <stdarg.h>

void fail(const char *format, ...) {
	fprintf(stderr, "FATAL ERROR: ");
	
	va_list arglist;
	va_start(arglist, format);
	vfprintf(stderr, format, arglist);
	va_end(arglist);
	
	fprintf(stderr, "\n");
	
	abort();
}
