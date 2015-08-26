#include "radiosity.h"

#include "matrix.h"

void compute_radiosity(triangle* trgs, size_t trgcount) {
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
	for(size_t i = 0; i <= trgcount; ++i) {
		for(size_t j = 0; j <= trgcount; ++j) {
			double val;
			
			if(j == trgcount) {
				if(i == trgcount) {
					val = 1.0;
				} else {
					val = trgs[i].emitted_energy;
				}
			} else if(i == trgcount) {
				val = 0.0;
			} else {
				if(i == j) {
					val = 0.0;
				} else {
					vec3 ci = triangle_centroid(trgs[i]);
					vec3 cj = triangle_centroid(trgs[j]);
					
					vec3 diff = vec3_sub(cj, ci);
					double difflen = vec3_len(diff);
					
					// Use a conservative distance estimate: average of
					// distances between corners.
					double dist = 0.0;
					for(int a = 0; a < 3; ++a) {
						for(int b = 0; b < 3; ++b) {
							vec3 ci = trgs[i].corners[a];
							vec3 cj = trgs[j].corners[b];
							dist += vec3_len(vec3_sub(ci, cj));
						}
					}
					dist /= 9.0;
					
					vec3 ni = triangle_normal(trgs[i]);
					vec3 nj = triangle_normal(trgs[j]);
					
					double cosi = vec3_dot(ni, diff) / (difflen * vec3_len(ni));
					double cosj = -vec3_dot(nj, diff) / (difflen * vec3_len(nj));
					if(cosi <= 0.0 || cosj <= 0.0) {
						val = 0.0;
					} else {
						val = trgs[j].reflectivity * cosi * cosj;
						val *= triangle_area(trgs[i]);
						val /= PI * dist * dist;
					}
				}
			}
			
			Y.data[Ypos] = val;
			
			++Ypos;
		}
	}
	
	// Iterate assignment B <- Y B.
	for(int i = 0; i < 40; ++i) {
		matrix_vector_mul(Y, B, B2);
		
		vector tmp = B;
		B = B2;
		B2 = tmp;
		
		double asd = 0.0;
		for(size_t i = 0; i < trgcount; ++i) {
			double d = fabs(B.data[i] - B2.data[i]);
			if(d > asd) asd = d;
		}
		printf("Iteration #%d: maximum matrix difference %lf\n", i + 1, asd);
	}
	
	// Read the result from B.
	for(size_t i = 0; i < trgcount; ++i) {
		trgs[i].radiosity = B.data[i];
	}
	
	free_matrix(Y);
	free_vector(B2);
	free_vector(B);
}
