#include "draw_bmp.h"

#include "bmp.h"

static void draw_triangle(
	bmp_canvas canvas, triangle trg, float wd, float hd, float coef
) {
	// Determine pixel color by radiosity.
	int color;
	if(vec3_dot(triangle_normal(trg), trg.corners[0]) < 0.0f) {
		float radiosity = trg.radiosity;
		if(radiosity < 0.0f) radiosity = 0.0f;
		if(radiosity > 1.0f) radiosity = 1.0f;
		
		// Gamma correction.
		radiosity = pow(radiosity, 0.8f);
		
		color = floor(256 * radiosity);
		if(color < 0) color = 0;
		if(color > 255) color = 255;
	} else {
		// Triangle is facing the wrong direction, do not draw.
		return;
	}
	
	for(int i = 0; i < 3; ++i) {
		vec3 a = trg.corners[i];
		if(a.z > 0) {
			size_t vertcount = 0;
			double xs[10];
			double ys[10];
			
			// Draw the triangle, projected to 3D.
			
			xs[vertcount] = a.x / a.z;
			ys[vertcount] = a.y / a.z;
			++vertcount;
			
			for(int j = 1; j <= 3; ++j) {
				int p = (i + j) % 3;
				vec3 b = trg.corners[p];
				
				// We only draw the points that are in front of the camera.
				// Because of that we also have to draw the collision points of
				// the triangle frame with the plane z = 0.
				if((a.z > 0.0f) != (b.z > 0.0f)) {
					float t = -a.z / (b.z - a.z);
					float x = a.x + t * (b.x - a.x);
					float y = a.y + t * (b.y - a.y);
					float r = sqrt(x * x + y * y);
					if(r > 0.0f) {
						float coef = 1e5 / r;
						x *= coef;
						y *= coef;
					}
					xs[vertcount] = x;
					ys[vertcount] = y;
					++vertcount;
				}
				
				if(b.z > 0.0f) {
					xs[vertcount] = b.x / b.z;
					ys[vertcount] = b.y / b.z;
					++vertcount;
				}
				
				a = b;
			}
			
			// Draw the polygon we have constructed to (xs, ys) to the canvas.
			for(size_t i = 2; i < vertcount; ++i) {
				bmp_draw(
					canvas, 
					0.5f * wd + coef * xs[0], 0.5f * hd + coef * ys[0],
					0.5f * wd + coef * xs[1], 0.5f * hd + coef * ys[1],
					0.5f * wd + coef * xs[i], 0.5f * hd + coef * ys[i],
					color, color, color
				);
			}
			
			break;
		}
	}
}

typedef struct {
	size_t index;
	float distance;
} triangle_distance;

int triangle_distance_cmp(const void* a, const void* b) {
	const triangle_distance* x = (const triangle_distance*)a;
	const triangle_distance* y = (const triangle_distance*)b;
	if(x->distance == y->distance) return 0;
	if(x->distance < y->distance) return 1;
	return -1;
}

void draw_to_bmp(const char* filename, triangle* trgs, size_t trgcount) {
	size_t w = 2560;
	size_t h = 1440;
	float wd = w;
	float hd = h;
	float fov = 40;

	bmp_canvas canvas = bmp_start(w, h);
	
	// Use painter's method: order the triangles from the farthest to the
	// closest. We use the farthest corner of the triangle as the distance
	// value.
	triangle_distance* trgdists;
	trgdists = checked_malloc2(trgcount, sizeof(triangle_distance));
	
	for(size_t i = 0; i < trgcount; ++i) {
		trgdists[i].index = i;
		float maxdist = 0.0f;
		for(int j = 0; j < 3; ++j) {
			float dist = vec3_len2(trgs[i].corners[j]);
			if(dist > maxdist) maxdist = dist;
		}
		trgdists[i].distance = maxdist;
	}
	
	qsort(trgdists, trgcount, sizeof(triangle_distance), triangle_distance_cmp);
	
	float coef = 0.5f * wd / tan(PI * fov / 180.0f);
	for(size_t i = 0; i < trgcount; ++i) {
		draw_triangle(canvas, trgs[trgdists[i].index], wd, hd, coef);
	}
	
	free(trgdists);
	
	bmp_write(canvas, filename);
}
