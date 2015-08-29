#include "matrix.h"

#ifdef LARGE_MATRIX_TMPFILE_TEMPLATE
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#endif

vector create_vector(size_t size) {
	vector ret;
	ret.size = size;
	ret.data = checked_malloc2(size, sizeof(float));
	return ret;
}

void free_vector(vector vec) {
	free(vec.data);
}

matrix create_matrix(size_t size) {
	matrix ret;
	ret.size = size;
	
#ifdef LARGE_MATRIX_TMPFILE_TEMPLATE
	if(size >= 5000) {
		// Large matrix, create a temporary file.
		const char* template = LARGE_MATRIX_TMPFILE_TEMPLATE;
		ret.tmpfilename = checked_malloc(strlen(template) + 1);
		strcpy(ret.tmpfilename, template);
		int fd = mkstemp(ret.tmpfilename);
		if(fd == -1) {
			fail(
				"Could not create temporary file with mkstemp for a large "
				"matrix."
			);
		}
		size_t bytes = size;
		if(bytes == 0 || size > SIZE_MAX / bytes) {
			fail("Matrix too large.");
		}
		bytes *= size;
		if(bytes == 0 || sizeof(float) > SIZE_MAX / bytes) {
			fail("Matrix too large.");
		}
		bytes *= sizeof(float);
		
		// Enlarge the file to have the right size.
		char zeros[4096] = {0};
		size_t written = 0;
		while(1) {
			if(write(fd, zeros, 4096) == -1) {
				fail("Could not write to temporary file for a large matrix.");
			}
			size_t old_written = written;
			written += 4096;
			if(written > bytes || written <= old_written) break;
		}
		if(fsync(fd)) fail("Could not fsync temporary file for a large matrix.");
		
		// Create memory mapping.
		ret.data = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if(ret.data == MAP_FAILED) {
			fail("Could not mmap() temporary file for a large matrix.");
		}
		close(fd);
		return ret;
	}
#endif
	
	ret.data = checked_malloc3(size, size, sizeof(float));
	return ret;
}

void free_matrix(matrix mat) {
#ifdef LARGE_MATRIX_TMPFILE_TEMPLATE
	if(mat.size >= 5000) {
		if(munmap(mat.data, mat.size * mat.size * sizeof(float))) {
			fail("Could not munmap() temporary file for a large matrix.");
		}
		if(unlink(mat.tmpfilename)) {
			fail("Could not unlink() temporary file for a large matrix.");
		}
		free(mat.tmpfilename);
		return;
	}
#endif
	free(mat.data);
}

void matrix_vector_mul(matrix m, vector v, vector result) {
	size_t n = m.size;
	if(v.size != n) fail("matrix_vector_mul: sizes do not match.");
	if(result.size != n) fail("matrix_vector_mul: sizes do not match.");
	
	size_t pos = 0;
	for(size_t i = 0; i < n; ++i) {
		float val = 0.0;
		for(size_t j = 0; j < n; ++j) {
			val += m.data[pos] * v.data[j];
			++pos;
		}
		result.data[i] = val;
	}
}
