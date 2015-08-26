#include "matrix.h"

vector create_vector(size_t size) {
	vector ret;
	ret.size = size;
	ret.data = checked_malloc2(size, sizeof(double));
	return ret;
}

void free_vector(vector vec) {
	free(vec.data);
}

matrix create_matrix(size_t size) {
	matrix ret;
	ret.size = size;
	ret.data = checked_malloc3(size, size, sizeof(double));
	return ret;
}

void free_matrix(matrix mat) {
	free(mat.data);
}

void matrix_vector_mul(matrix m, vector v, vector result) {
	size_t n = m.size;
	if(v.size != n) fail("matrix_vector_mul: sizes do not match.");
	if(result.size != n) fail("matrix_vector_mul: sizes do not match.");
	
	size_t pos = 0;
	for(size_t i = 0; i < n; ++i) {
		double val = 0.0;
		for(size_t j = 0; j < n; ++j) {
			val += m.data[pos] * v.data[j];
			++pos;
		}
		result.data[i] = val;
	}
}
