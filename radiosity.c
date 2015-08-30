#include "radiosity.h"

#include "matrix.h"

#ifdef WORKERTHREADS
#include <pthread.h>
#endif

/// Samples a point from triangle \p trg using parameters \p u, \p v. To get
/// points inside the triangle, it should hold that u, v >= 0 and u + v <= 1.
static inline vec3 triangle_point(triangle trg, float u, float v) {
	return vec3_add(
		trg.corners[0],
		vec3_add(
			vec3_mul(vec3_sub(trg.corners[1], trg.corners[0]), u),
			vec3_mul(vec3_sub(trg.corners[2], trg.corners[0]), v)
		)
	);
}

static inline float radiosity_matrix_element(
	triangle* trgs, size_t trgcount,
	raycast raycast_ctx,
	size_t trgsamplecount, float* trgsample_u, float* trgsample_v,
	size_t i, size_t j
) {
	if(j == trgcount) {
		if(i == trgcount) {
			return 1.0f;
		} else {
			return trgs[i].emitted_energy;
		}
	}
	if(i == trgcount || i == j) {
		return 0.0f;
	}
	
	vec3 ni = triangle_normal(trgs[i]);
	vec3 nj = triangle_normal(trgs[j]);
	
	float ni_len = vec3_len(ni);
	float nj_len = vec3_len(nj);
	
	// Sample some points from both triangles and use the average of the
	// radiosity value for those points as the value for the triangles.
	float val = 0.0f;
	
	for(int si = 0; si < trgsamplecount; ++si) {
		vec3 vi = triangle_point(trgs[i], trgsample_u[si], trgsample_v[si]);
		for(int sj = 0; sj < trgsamplecount; ++sj) {
			vec3 vj = triangle_point(trgs[j], trgsample_u[sj], trgsample_v[sj]);
			
			vec3 diff = vec3_sub(vj, vi);
			float difflen = vec3_len(diff);
			
			float cosi = vec3_dot(ni, diff) / (difflen * ni_len);
			float cosj = -vec3_dot(nj, diff) / (difflen * nj_len);
			
			// If the triangles do not face each other, no light propagation.
			if(cosi <= 0.0f || cosj <= 0.0f) continue;
			
			float term = trgs[i].reflectivity * cosi * cosj;
			term /= PI * difflen * difflen;
			val += term;
		}
	}
	val /= (float)trgsamplecount * (float)trgsamplecount;
	val *= triangle_area(trgs[j]);
	
	if(isnan(val)) {
		fail(
			"NaN value created in radiosity matrix. "
			"Possibly caused by degenerate triangles."
		);
	}
	
	if(val == 0.0f) return val;
	
	// Light propagates only if the triangles see each other.
	vec3 ci = triangle_centroid(trgs[i]);
	vec3 cj = triangle_centroid(trgs[j]);
	
	for(int a = 0; a < 3; ++a) {
		vec3 vi = vec3_add(
			vec3_mul(ci, 0.25f),
			vec3_mul(trgs[i].corners[a], 0.75f)
		);
		vec3 vj = vec3_add(
			vec3_mul(cj, 0.25f),
			vec3_mul(trgs[j].corners[a], 0.75f)
		);
		if(!raycast_query(raycast_ctx, vi, vj)) return val;
	}
	return 0.0f;
}

typedef struct {
	triangle* trgs;
	size_t trgcount;
	raycast raycast_ctx;
	size_t next_percent;
	size_t i;
	matrix Y;
	
#ifdef WORKERTHREADS
	pthread_mutex_t lock;
#endif
} matrix_job;

static void* matrix_worker(void* ptr) {
	// Precompute triangle sampling pattern for radiosity_matrix_element.
	size_t trgsample_res = 4;
	size_t trgsamplecount = trgsample_res * trgsample_res;
	float* trgsample_u = checked_malloc2(trgsamplecount, sizeof(float));
	float* trgsample_v = checked_malloc2(trgsamplecount, sizeof(float));
	size_t pos = 0;
	for(size_t i = 0; i < trgsample_res; ++i) {
	for(size_t j = 0; j < trgsample_res - i; ++j) {
	for(size_t a = 0; a <= (i + j != trgsample_res - 1); ++a) {
		trgsample_u[pos] = ((float)i + (a ? 2 : 1) / 3.0f) / (float)trgsample_res;
		trgsample_v[pos] = ((float)j + (a ? 2 : 1) / 3.0f) / (float)trgsample_res;
		++pos;
	}
	}
	}
	
	matrix_job* job = (matrix_job*)ptr;
	while(1) {
#ifdef WORKERTHREADS
		if(pthread_mutex_lock(&job->lock)) printf("Could not lock mutex.");
#endif
		
		size_t i = job->i;
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
				job->raycast_ctx,
				trgsamplecount, trgsample_u, trgsample_v,
				i, j
			);
		}
	}
	
	free(trgsample_u);
	free(trgsample_v);
	
	return NULL;
}

void compute_radiosity(
	triangle* trgs, size_t trgcount,
	raycast raycast_ctx
) {
	// We solve radiosity B from the equation B = E + X B, where E is the
	// emitted energy and X is the radiosity matrix. By augmenting the matrix
	// B with one element 1, we can write this in augmented form B = Y B, and
	// solve it by iteration.
	vector B = create_vector(trgcount + 1);
	vector B2 = create_vector(trgcount + 1);
	matrix Y = create_matrix(trgcount + 1);
	
	for(size_t i = 0; i < trgcount; ++i) {
		B.data[i] = 0.0f;
	}
	B.data[trgcount] = 1.0f;
	
	matrix_job job;
	job.trgs = trgs;
	job.trgcount = trgcount;
	job.raycast_ctx = raycast_ctx;
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
	for(int i = 0; i < 500; ++i) {
		matrix_vector_mul(Y, B, B2);
		
		vector tmp = B;
		B = B2;
		B2 = tmp;
		
		float maxch = 0.0f;
		for(size_t i = 0; i < trgcount; ++i) {
			double a = fabs(B.data[i] - B2.data[i]);
			double b = fabs(B2.data[i]);
			if(a == 0.0f && b == 0.0f) continue;
			float ch = a / b;
			if(ch > maxch) maxch = ch;
			
		}
		printf(
			"Iteration #%d: maximum relative change %e\n",
		i + 1, maxch);
		if(maxch < 0.005e-2f) {
			printf("Maximum relative change below limit 0.005%%, stopping.\n");
			break;
		}
	}
	
	// Read the result from B.
	for(size_t i = 0; i < trgcount; ++i) {
		trgs[i].radiosity = B.data[i];
	}
	
	free_matrix(Y);
	free_vector(B2);
	free_vector(B);
}
