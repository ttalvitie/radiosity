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
					double dist2 = vec3_len2(vec3_sub(ci, cj));
					
					val = trgs[i].reflectivity / (PI * dist2);
				}
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
	}
	
	// Read the result from B.
	for(size_t i = 0; i < trgcount; ++i) {
		trgs[i].radiosity = B.data[i];
	}
	
	free_matrix(Y);
	free_vector(B2);
	free_vector(B);
}
