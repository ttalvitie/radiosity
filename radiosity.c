#include "radiosity.h"

#include "matrix.h"

static inline float radiosity_matrix_element(
	triangle* trgs, size_t trgcount,
	triangle* collision_trgs, size_t collision_trgcount,
	size_t i, size_t j
) {
	if(i == j) {
		return 0.0;
	}
	vec3 ci = triangle_centroid(trgs[i]);
	vec3 cj = triangle_centroid(trgs[j]);
	
	// If the centroids do not see each other, no light propagation.
	for(size_t k = 0; k < collision_trgcount; ++k) {
		if(segment_intersects_triangle(ci, cj, collision_trgs[k])) {
			return 0.0;
		}
	}
	
	vec3 diff = vec3_sub(cj, ci);
	float difflen = vec3_len(diff);
	
	vec3 ni = triangle_normal(trgs[i]);
	vec3 nj = triangle_normal(trgs[j]);
	
	float cosi = vec3_dot(ni, diff) / (difflen * vec3_len(ni));
	float cosj = -vec3_dot(nj, diff) / (difflen * vec3_len(nj));
	
	// If the triangles do not face each other, no light propagation.
	if(cosi <= 0.0 || cosj <= 0.0) return 0.0;
	
	// Use a conservative distance estimate: average of
	// distances between corners and distance between centroids.
	float dist = 0.0;
	for(int a = 0; a < 3; ++a) {
		for(int b = 0; b < 3; ++b) {
			vec3 ci = trgs[i].corners[a];
			vec3 cj = trgs[j].corners[b];
			dist += vec3_len(vec3_sub(ci, cj));
		}
	}
	dist /= 9.0;
	dist = 0.25 * dist + 0.75 * difflen;
	
	float val = trgs[i].reflectivity * cosi * cosj;
	val *= triangle_area(trgs[j]);
	val /= PI * dist * dist;
	return val;
}

void compute_radiosity(
	triangle* trgs, size_t trgcount,
	triangle* collision_trgs, size_t collision_trgcount
) {
	// We solve radiosity B from the equation B = E + X B, where E is the
	// emitted energy and X is the radiosity matrix. By augmenting the matrix
	// B with one element 1, we can write this in augmented form B = Y B, and
	// solve it by iteration.
	vector B = create_vector(trgcount + 1);
	vector B2 = create_vector(trgcount + 1);
	matrix Y = create_matrix(trgcount + 1);
	
	for(size_t i = 0; i < trgcount; ++i) {
		B.data[i] = 0.0;
	}
	B.data[trgcount] = 1.0;
	
	size_t Ypos = 0;
	size_t next_percent = 0;
	for(size_t i = 0; i <= trgcount; ++i) {
		if(100 * i >= next_percent * trgcount) {
			printf("Populating radiosity matrix: %zu%%.\n", next_percent++);
		}
		for(size_t j = 0; j <= trgcount; ++j) {
			float val;
			
			if(j == trgcount) {
				if(i == trgcount) {
					val = 1.0;
				} else {
					val = trgs[i].emitted_energy;
				}
			} else if(i == trgcount) {
				val = 0.0;
			} else {
				val = radiosity_matrix_element(
					trgs, trgcount,
					collision_trgs, collision_trgcount,
					i, j
				);
			}
			
			if(isnan(val)) {
				fail(
					"NaN value created in radiosity matrix. "
					"Possibly caused by degenerate triangles."
				);
			}
			
			Y.data[Ypos] = val;
			
			++Ypos;
		}
	}
	
	// Iterate assignment B <- Y B.
	for(int i = 0; i < 100; ++i) {
		matrix_vector_mul(Y, B, B2);
		
		vector tmp = B;
		B = B2;
		B2 = tmp;
		
		float asd = 0.0;
		for(size_t i = 0; i < trgcount; ++i) {
			float d = fabs(B.data[i] - B2.data[i]);
			if(d > asd) asd = d;
		}
		printf("Iteration #%d: maximum matrix element difference %f\n", i + 1, asd);
	}
	
	// Read the result from B.
	for(size_t i = 0; i < trgcount; ++i) {
		trgs[i].radiosity = B.data[i];
	}
	
	free_matrix(Y);
	free_vector(B2);
	free_vector(B);
}
