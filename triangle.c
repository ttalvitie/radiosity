#include "triangle.h"

#include <errno.h>

void normalize_triangle_radiosities(triangle* trgs, size_t trgcount) {
	double minval = 0.0;
	double maxval = 0.0;
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

static void add_subdivided_triangle(
	triangle trg,
	double edge_length_limit2,
	triangle** output,
	size_t* count,
	size_t* allocated
) {
	if(edge_length_limit2 > 0.0) {
		// If the longest edge of the triangle has length larger than the limit,
		// then we split that edge.
		double maxval = -1.0;
		int maxval_i = 0;
		for(int i = 0; i < 3; ++i) {
			int j = (i + 1) % 3;
			double val = vec3_len2(vec3_sub(trg.corners[i], trg.corners[j]));
			if(val > maxval) {
				maxval = val;
				maxval_i = i;
			}
		}
		if(maxval > edge_length_limit2) {
			triangle trg1 = trg;
			triangle trg2 = trg;
			
			int i = maxval_i;
			int j = (i + 1) % 3;
			int k = (j + 1) % 3;
			
			vec3 split = vec3_add(trg.corners[i], trg.corners[j]);
			split = vec3_mul(split, 0.5);
			
			trg1.corners[0] = split;
			trg1.corners[1] = trg.corners[k];
			trg1.corners[2] = trg.corners[i];
			
			trg2.corners[0] = split;
			trg2.corners[1] = trg.corners[j];
			trg2.corners[2] = trg.corners[k];
			
			add_subdivided_triangle(
				trg1, edge_length_limit2, output, count, allocated
			);
			add_subdivided_triangle(
				trg2, edge_length_limit2, output, count, allocated
			);
			
			return;
		}
	}
	
	++(*count);
	while((*count) > (*allocated)) {
		(*allocated) *= 2;
		*output = checked_realloc2(*output, *allocated, sizeof(triangle));
	}
	(*output)[(*count) - 1] = trg;
}

size_t read_triangles_from_file(
	const char* filename,
	double edge_length_limit,
	triangle** output
) {
	if(edge_length_limit < 0.0) edge_length_limit = 0.0;
	double edge_length_limit2 = edge_length_limit * edge_length_limit;
	
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
		add_subdivided_triangle(
			trg, edge_length_limit2, output, &count, &allocated
		);
	}
	
	fclose(fp);
	
	return count;
}
