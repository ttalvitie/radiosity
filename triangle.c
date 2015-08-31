#include "triangle.h"

#include <errno.h>

void normalize_triangle_radiosities(triangle* trgs, size_t trgcount) {
	float maxval = 0.0f;
	for(size_t i = 0; i < trgcount; ++i) {
		// Only take into account the triangles that are not lights, i.e. do not
		// emit light.
		if(trgs[i].emitted_energy > 0.0f) continue;
		
		float val = trgs[i].radiosity;
		if(val > maxval) maxval = val;
	}
	if(maxval == 0.0f) maxval = 1.0f;
	
	for(size_t i = 0; i < trgcount; ++i) {
		float t = 0.9f * trgs[i].radiosity / maxval;
		if(t < 0.0f) t = 0.0f;
		if(t > 1.0f) t = 1.0f;
		trgs[i].radiosity = t;
	}
}

static void add_subdivided_triangle(
	triangle trg,
	float edge_length_limit2,
	triangle** output,
	size_t* count,
	size_t* allocated
) {
	if(edge_length_limit2 > 0.0f) {
		// If the longest edge of the triangle has length larger than the limit,
		// then we split that edge.
		float maxval = -1.0f;
		int maxval_i = 0;
		for(int i = 0; i < 3; ++i) {
			int j = (i + 1) % 3;
			float val = vec3_len2(vec3_sub(trg.corners[i], trg.corners[j]));
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
			
			// We don't split exactly at the center to have some nice randomness
			// in the triangles.
			double t = 0.4f + 0.2f * (double)rand() / (double)RAND_MAX;
			vec3 split = vec3_add(
				vec3_mul(trg.corners[i], 1.0f - t),
				vec3_mul(trg.corners[j], t)
			);
			
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

size_t subdivide_triangles(
	triangle* trgs,
	size_t trgcount,
	float edge_length_limit,
	triangle** output
) {
	float edge_length_limit2 = edge_length_limit * edge_length_limit;
	
	size_t count = 0;
	size_t allocated = 1;
	*output = checked_malloc2(allocated, sizeof(triangle));
	
	for(size_t i = 0; i < trgcount; ++i) {
		add_subdivided_triangle(
			trgs[i], edge_length_limit2, output, &count, &allocated
		);
	}
	
	return count;
}

size_t read_triangles_from_file(const char* filename, triangle** output) {
	FILE* fp = fopen(filename, "r");
	if(!fp) fail("Could not open triangle input file '%s'.", filename);
	
	size_t count = 0;
	
	size_t allocated = 1;
	*output = checked_malloc2(allocated, sizeof(triangle));
	
	while(1) {
		triangle trg;
		trg.radiosity = 0.0f;
		errno = 0;
		int read = fscanf(
			fp, "%f %f %f %f %f %f %f %f %f %f %f",
			&trg.corners[0].x, &trg.corners[0].y, &trg.corners[0].z,
			&trg.corners[1].x, &trg.corners[1].y, &trg.corners[1].z,
			&trg.corners[2].x, &trg.corners[2].y, &trg.corners[2].z,
			&trg.reflectivity, &trg.emitted_energy
		);
		trg.group = count;
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
