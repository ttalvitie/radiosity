#include "draw_html.h"

static void draw_triangle(FILE* fp, triangle trg) {
	for(int i = 0; i < 3; ++i) {
		vec3 a = trg.corners[i];
		if(a.z > 0) {
			fprintf(fp, "  ctx.beginPath();\n");
			
			// Draw all vertices of the triangle, projected to 3D.
			fprintf(fp, "  ctx.moveTo(%lf, %lf);\n", a.x / a.z, a.y / a.z);
			for(int j = 1; j <= 3; ++j) {
				int p = (i + j) % 3;
				vec3 b = trg.corners[p];
				
				// We only draw the points that are in front of the camera.
				// Because of that we also have to draw the collision points of
				// the triangle frame with the plane z = 0.
				if((a.z > 0.0) != (b.z > 0.0)) {
					double t = -a.z / (b.z - a.z);
					double x = a.x + t * (b.x - a.x);
					double y = a.y + t * (b.y - a.y);
					double r = sqrt(x * x + y * y);
					if(r > 0.0) {
						double coef = 1e5 / r;
						x *= coef;
						y *= coef;
					}
					fprintf(fp, "  ctx.lineTo(%lf, %lf);\n", x, y);
				}
				
				if(b.z > 0.0) {
					fprintf(fp, "  ctx.lineTo(%lf, %lf);\n", b.x / b.z, b.y / b.z);
				}
				
				a = b;
			}
			
			// Determine pixel color: by radiosity if CCW oriented, otherwise
			// black.
			int color;
			if(vec3_dot(triangle_normal(trg), trg.corners[0]) < 0.0) {
				double radiosity = trg.radiosity;
				if(radiosity < 0.0) radiosity = 0.0;
				if(radiosity > 1.0) radiosity = 1.0;
				
				// Gamma correction.
				radiosity = pow(radiosity, 0.9);
				
				color = floor(256 * radiosity);
				if(color < 0) color = 0;
				if(color > 255) color = 255;
			} else {
				color = 0;
			}
			
			fprintf(
				fp, "  ctx.fillStyle = 'rgb(%d, %d, %d)';\n",
				color, color, color
			);
			fprintf(
				fp, "  ctx.strokeStyle = 'rgb(%d, %d, %d)';\n",
				color, color, color
			);
			fprintf(fp, "  ctx.fill();\n");
			fprintf(fp, "  ctx.stroke();\n");
			
			break;
		}
	}
}

void draw_to_html(const char* filename, triangle* trgs, size_t trgcount) {
	FILE* fp = fopen(filename, "w");
	if(!fp) fail("Could not open HTML output file '%s'.", filename);
	
	int w = 1000;
	int h = 700;
	double wd = w;
	double hd = h;
	double fov = 40;
	
	fprintf(fp, "<!DOCTYPE html>\n");
	fprintf(fp, "<html>\n");
	fprintf(fp, "<head>\n");
	fprintf(fp, "<meta charset=\"utf-8\">\n");
	fprintf(fp, "<title>Radiosity output file</title>\n");
	fprintf(fp, "<script>\n");
	fprintf(fp, "document.addEventListener('DOMContentLoaded', function(event) {\n");
	fprintf(fp, "  var canvas = document.getElementById('render');\n");
	fprintf(fp, "  var ctx = canvas.getContext('2d');\n");
	fprintf(fp, "  ctx.fillRect(0, 0, %d, %d);\n", w, h);
	fprintf(fp, "  ctx.scale(1, -1);\n");
	fprintf(fp, "  ctx.translate(%lf, %lf);\n", 0.5 * wd, -0.5 * hd);
	fprintf(fp, "  ctx.fillStyle = '#FFFFFF';\n");
	double coef = 0.5 * wd / tan(PI * fov / 180.0);
	fprintf(fp, "  ctx.scale(%lf, %lf);\n", coef, coef);
	fprintf(fp, "  ctx.lineWidth = %lf;\n", 1.6 / coef);
	fprintf(fp, "  ctx.lineJoin = 'round';\n");
	fprintf(fp, "  ctx.lineCap = 'round';\n");
	
	for(size_t i = 0; i < trgcount; ++i) {
		draw_triangle(fp, trgs[i]);
	}
	
	fprintf(fp, "});\n");
	fprintf(fp, "</script>\n");
	fprintf(fp, "</head>\n");
	fprintf(fp, "<body>\n");
	fprintf(fp, "<canvas id=\"render\" width=\"%d\" height=\"%d\"></canvas>\n", w, h);
	fprintf(fp, "</body>\n");
	fprintf(fp, "</html>\n");
	
	fclose(fp);
}
