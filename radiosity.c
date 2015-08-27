#include "radiosity.h"

#include "matrix.h"

#ifdef WORKERTHREADS
#include <pthread.h>
#endif

static inline float radiosity_matrix_element(
	triangle* trgs, size_t trgcount,
	triangle* collision_trgs, size_t collision_trgcount,
	size_t i, size_t j
) {
	if(j == trgcount) {
		if(i == trgcount) {
			return 1.0;
		} else {
			return trgs[i].emitted_energy;
		}
	}
	if(i == trgcount || i == j) {
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
	
	if(isnan(val)) {
		fail(
			"NaN value created in radiosity matrix. "
			"Possibly caused by degenerate triangles."
		);
	}
	
	return val;
}

typedef struct {
	triangle* trgs;
	size_t trgcount;
	triangle* collision_trgs;
	size_t collision_trgcount;
	size_t next_percent;
	size_t i;
	matrix Y;
	
#ifdef WORKERTHREADS
	pthread_mutex_t lock;
#endif
} matrix_job;

static void* matrix_worker(void* ptr) {
	matrix_job* job = (matrix_job*)ptr;
	while(1) {
#ifdef WORKERTHREADS
		if(pthread_mutex_lock(&job->lock)) printf("Could not lock mutex.");
#endif
		
		size_t i = job->i;
		printf("Got %zu\n", i);
		if(i == job->trgcount + 1) {
#ifdef WORKERTHREADS
			if(pthread_mutex_unlock(&job->lock)) printf("Could not unlock mutex.");
#endif
			break;
		}
		if(100 * i >= job->next_percent * job->trgcount) {
			printf("Populating radiosity matrix: %zu%%.\n", job->next_percent++);
		}
		++job->i;
		
#ifdef WORKERTHREADS
		if(pthread_mutex_unlock(&job->lock)) printf("Could not unlock mutex.");
#endif
		
		size_t Ypos = i * (job->trgcount + 1);
		for(size_t j = 0; j <= job->trgcount; ++j) {
			job->Y.data[Ypos++] = radiosity_matrix_element(
				job->trgs, job->trgcount,
				job->collision_trgs, job->collision_trgcount,
				i, j
			);
		}
	}
	return NULL;
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
	
	matrix_job job;
	job.trgs = trgs;
	job.trgcount = trgcount;
	job.collision_trgs = collision_trgs;
	job.collision_trgcount = collision_trgcount;
	job.next_percent = 0;
	job.i = 0;
	job.Y = Y;
	
#ifdef WORKERTHREADS
	if(pthread_mutex_init(&job.lock, NULL)) fail("Could not init mutex.");
	
	pthread_t workers[WORKERTHREADS];
	
	for(size_t i = 0; i < WORKERTHREADS; ++i) {
		if(pthread_create(&(workers[i]), NULL, matrix_worker, &job)) {
			fail("Could not spawn worker thread.");
		}
	}
	for(size_t i = 0; i < WORKERTHREADS; ++i) {
		if(pthread_join(workers[i], NULL)) {
			fail("Could not join worker thread.");
		}
	}
	
	if(pthread_mutex_destroy(&job.lock)) fail("Could not destroy mutex.");
#else
	matrix_worker(&job);
#endif
	
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
