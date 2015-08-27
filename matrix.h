/// @file
#pragma once

#include "common.h"

/// Vector of floats. Allocate with create_vector(), free with free_vector().
typedef struct {
	/// Number of elements in the vector.
	size_t size;
	
	/// The array of elements of the vector.
	float* data;
} vector;

/// Square matrix of floats. Allocate with create_matrix(), free with
/// free_matrix().
typedef struct {
	/// Number of elements in each row and column of the vector.
	size_t size;
	
	/// The array of elements of the matrix. The element (\p i, \p j) is stored
	/// in position \p size * \p i + \p j
	float* data;
} matrix;

/// Allocates a vector.
/// \param size The number of elements.
vector create_vector(size_t size);

/// Frees a vector.
/// \param vec The vector to free.
void free_vector(vector vec);

/// Allocates a matrix.
/// \param size The number of elements per row and column.
matrix create_matrix(size_t size);

/// Frees a matrix.
/// \param mat The matrix to free.
void free_matrix(matrix mat);

/// Multiplies matrix \p m by vector \p v, storing the result in \p result. The
/// sizes must match.
void matrix_vector_mul(matrix m, vector v, vector result);
