#include "triangle.h"

#include <errno.h>

void normalize_triangle_radiosities(triangle* trgs, size_t trgcount) {
	double minval = 1.0/0.0;
	double maxval = -1.0/0.0;
	for(size_t i = 0; i < trgcount; ++i) {
		double val = trgs[i].radiosity;
		if(val < minval) minval = val;
		if(val > maxval) maxval = val;
	}
	if(minval == maxval) {
		minval -= 1.0;
		maxval += 1.0;
	}
	for(size_t i = 0; i < trgcount; ++i) {
		double t = (trgs[i].radiosity - minval) / (maxval - minval);
		if(t < 0.0) t = 0.0;
		if(t > 1.0) t = 1.0;
		trgs[i].radiosity = t;
	}
}

size_t read_triangles_from_file(const char* filename, triangle** output) {
	FILE* fp = fopen(filename, "r");
	if(!fp) fail("Could not open triangle input file '%s'.", filename);
	
	size_t count = 0;
	
	size_t allocated = 1;
	*output = checked_malloc2(allocated, sizeof(triangle));
	
	while(1) {
		triangle trg;
		trg.radiosity = 0.0;
		errno = 0;
		int read = fscanf(
			fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
			&trg.corners[0].x, &trg.corners[0].y, &trg.corners[0].z,
			&trg.corners[1].x, &trg.corners[1].y, &trg.corners[1].z,
			&trg.corners[2].x, &trg.corners[2].y, &trg.corners[2].z,
			&trg.reflectivity, &trg.emitted_energy
		);
		if(read == EOF && !errno) break;
		if(read != 11) {
			fail("Error reading input file.");
		}
		++count;
		while(count > allocated) {
			allocated *= 2;
			*output = checked_realloc2(*output, allocated, sizeof(triangle));
		}
		(*output)[count - 1] = trg;
	}
	
	fclose(fp);
	
	return count;
}
