#include "draw_svg.h"

static void draw_triangle(FILE* fp, triangle trg) {
	// Determine pixel color by radiosity.
	int color;
	if(vec3_dot(triangle_normal(trg), trg.corners[0]) < 0.0) {
		float radiosity = trg.radiosity;
		if(radiosity < 0.0) radiosity = 0.0;
		if(radiosity > 1.0) radiosity = 1.0;
		
		// Gamma correction.
		radiosity = pow(radiosity, 0.85);
		
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
			
			// Draw all vertices of the triangle, projected to 3D.
			xs[vertcount] = a.x / a.z;
			ys[vertcount] = a.y / a.z;
			++vertcount;
			
			for(int j = 1; j <= 3; ++j) {
				int p = (i + j) % 3;
				vec3 b = trg.corners[p];
				
				// We only draw the points that are in front of the camera.
				// Because of that we also have to draw the collision points of
				// the triangle frame with the plane z = 0.
				if((a.z > 0.0) != (b.z > 0.0)) {
					float t = -a.z / (b.z - a.z);
					float x = a.x + t * (b.x - a.x);
					float y = a.y + t * (b.y - a.y);
					float r = sqrt(x * x + y * y);
					if(r > 0.0) {
						float coef = 1e5 / r;
						x *= coef;
						y *= coef;
					}
					xs[vertcount] = x;
					ys[vertcount] = y;
					++vertcount;
				}
				
				if(b.z > 0.0) {
					xs[vertcount] = b.x / b.z;
					ys[vertcount] = b.y / b.z;
					++vertcount;
				}
				
				a = b;
			}
			
			// Draw interior.
			fprintf(fp, "<polygon points=\"");
			for(size_t vert = 0; vert < vertcount; ++vert) {
				if(vert != 0) fprintf(fp, " ");
				fprintf(fp, "%f,%f", xs[vert], ys[vert]);
			}
			fprintf(fp,
				"\" fill=\"rgb(%d, %d, %d)\" stroke=\"none\" />\n",
				color, color, color
			);
			
			// Stroke only right-facing edges.
			for(size_t v1 = 0; v1 < vertcount; ++v1) {
				size_t v2 = (v1 + 1) % vertcount;
				if(ys[v1] < ys[v2] || (ys[v1] == ys[v2] && xs[v1] < xs[v2])) {
					fprintf(fp,
						"<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" "
						"stroke=\"rgb(%d, %d, %d)\" />",
						xs[v1], ys[v1], xs[v2], ys[v2], color, color, color
					);
				}
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

void draw_to_svg(const char* filename, triangle* trgs, size_t trgcount) {
	FILE* fp = fopen(filename, "w");
	if(!fp) fail("Could not open SVG output file '%s'.", filename);
	
	int w = 1280;
	int h = 720;
	float wd = w;
	float hd = h;
	float fov = 40;
	
	fprintf(fp,
		 "<svg xmlns=\"http://www.w3.org/2000/svg\" "
		 "width=\"%d\" height=\"%d\">\n",
		 w, h
	);
	fprintf(fp,
		 "<polygon points=\"0,0 %d,0 %d,%d 0,%d\" fill=\"black\" "
		 "stroke=\"black\" />\n",
		 w, w, h, h
	);
	fprintf(fp, "<g transform=\"scale(1, -1)\">\n");
	fprintf(fp, "<g transform=\"translate(%f, %f)\">\n", 0.5 * wd, -0.5 * hd);
	float coef = 0.5 * wd / tan(PI * fov / 180.0);
	fprintf(fp,
		"<g transform=\"scale(%f)\" stroke-width=\"%f\" "
		"stroke-linecap=\"round\">\n",
		coef, 0.3 / coef
	);
	
	// Use painter's method: order the triangles from the farthest to the
	// closest. We use the farthest corner of the triangle as the distance
	// value.
	triangle_distance* trgdists;
	trgdists = checked_malloc2(trgcount, sizeof(triangle_distance));
	
	for(size_t i = 0; i < trgcount; ++i) {
		trgdists[i].index = i;
		float maxdist = 0.0;
		for(int j = 0; j < 3; ++j) {
			float dist = vec3_len2(trgs[i].corners[j]);
			if(dist > maxdist) maxdist = dist;
		}
		trgdists[i].distance = maxdist;
	}
	
	qsort(trgdists, trgcount, sizeof(triangle_distance), triangle_distance_cmp);
	
	for(size_t i = 0; i < trgcount; ++i) {
		draw_triangle(fp, trgs[trgdists[i].index]);
	}
	
	free(trgdists);
	
	fprintf(fp, "</g>\n");
	fprintf(fp, "</g>\n");
	fprintf(fp, "</g>\n");
	fprintf(fp, "</svg>\n");
	
	fclose(fp);
}
