#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

float x, y, z, ref, emi;
int inv;

void reset() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	ref = 0.93;
	emi = 0.0;
	inv = 0;
}

void trg(
	float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3
) {
	if(x1 == x2 && y1 == y2 && z1 == z2) return;
	if(x1 == x3 && y1 == y3 && z1 == z3) return;
	if(x2 == x3 && y2 == y3 && z2 == z3) return;
	
	x1 += x; y1 += y; z1 += z;
	x2 += x; y2 += y; z2 += z;
	x3 += x; y3 += y; z3 += z;
	if(inv) {
		printf(
			"%f %f %f %f %f %f %f %f %f %f %f\n",
			x1, y1, z1, x3, y3, z3, x2, y2, z2, ref, emi
		);
	} else {
		printf(
			"%f %f %f %f %f %f %f %f %f %f %f\n",
			x1, y1, z1, x2, y2, z2, x3, y3, z3, ref, emi
		);
	}
}
void quad(
	float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3,
	float x4, float y4, float z4
) {
	trg(x1, y1, z1, x2, y2, z2, x3, y3, z3);
	trg(x1, y1, z1, x3, y3, z3, x4, y4, z4);
}

void sphere(float r, int detail) {
	for(int i = 0; i < detail; ++i) {
		float a = r * sin(PI * ((float)i / (float)detail - 0.5));
		float b = r * sin(PI * ((float)(i + 1) / (float)detail - 0.5));
		float ra = sqrt(r * r - a * a);
		float rb = sqrt(r * r - b * b);
		
		for(int j = 0; j < detail; ++j) {
			float p = 2.0 * PI * (float)j / (float)detail;
			float q = 2.0 * PI * (float)(j + 1) / (float)detail;
			
			quad(
				ra * cos(p), a, ra * sin(p),
				ra * cos(q), a, ra * sin(q),
				rb * cos(q), b, rb * sin(q),
				rb * cos(p), b, rb * sin(p)
			);
		}
		
	}
}
// Half where x < 0 for sphere().
void halfsphere(float r, int detail) {
	for(int i = 0; i < detail; ++i) {
		float a = r * sin(PI * ((float)i / (float)detail - 0.5));
		float b = r * sin(PI * ((float)(i + 1) / (float)detail - 0.5));
		float ra = sqrt(r * r - a * a);
		float rb = sqrt(r * r - b * b);
		
		for(int j = 0; j < detail; ++j) {
			float p = 0.5 * PI + PI * (float)j / (float)detail;
			float q = 0.5 * PI + PI * (float)(j + 1) / (float)detail;
			quad(
				ra * cos(p), a, ra * sin(p),
				ra * cos(q), a, ra * sin(q),
				rb * cos(q), b, rb * sin(q),
				rb * cos(p), b, rb * sin(p)
			);
		}
		
	}
}
void y_cylinder(float r, float h, int detail) {
	for(int j = 0; j < detail; ++j) {
		float p = 2.0 * PI * (float)j / (float)detail;
		float q = 2.0 * PI * (float)(j + 1) / (float)detail;
		quad(
			r * cos(p), 0.0, r * sin(p),
			r * cos(q), 0.0, r * sin(q),
			r * cos(q), h, r * sin(q),
			r * cos(p), h, r * sin(p)
		);
	}
}

void box(float a, float b, float c) {
	quad(
		a, b, c,
		-a, b, c,
		-a, b, -c,
		a, b, -c
	);
	quad(
		a, -b, c,
		a, -b, -c,
		-a, -b, -c,
		-a, -b, c
	);
	quad(
		a, -b, -c,
		a, b, -c,
		-a, b, -c,
		-a, -b, -c
	);
	quad(
		a, -b, c,
		-a, -b, c,
		-a, b, c,
		a, b, c
	);
	quad(
		a, -b, -c,
		a, -b, c,
		a, b, c,
		a, b, -c
	);
	quad(
		-a, -b, -c,
		-a, b, -c,
		-a, b, c,
		-a, -b, c
	);
}

int main() {
	reset();
	
	// room
	quad(
		-1, 0, -1,
		1, 0, -1,
		1, 0, 1,
		-1, 0, 1
	);
	quad(
		-1, 1, -1,
		-1, 1, 1,
		1, 1, 1,
		1, 1, -1
	);
	quad(
		-1, 0, -1,
		-1, 0, 1,
		-1, 1, 1,
		-1, 1, -1
	);
	quad(
		1, 0, -1,
		1, 1, -1,
		1, 1, 1,
		1, 0, 1
	);
	quad(
		-1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		-1, 1, 1
	);
	
	// Lamp
	emi = 1;
	y = 0.775;
	x = -0.2;
	sphere(0.075, 10);
	
	// Lamp bar
	reset();
	ref = 0.3;
	y = 0.85;
	x = -0.2;
	y_cylinder(0.005, 0.15, 10);
	
	// Wall lamp
	reset();
	emi = 0.5;
	x = 1;
	y = 0.55;
	z = -0.3;
	halfsphere(0.05, 12);
	
	// Table legs
	reset();
	x = -0.5;
	z = -0.3;
	y_cylinder(0.02, 0.35, 16);
	x = -x;
	y_cylinder(0.02, 0.35, 16);
	z = -z;
	y_cylinder(0.02, 0.35, 16);
	x = -x;
	y_cylinder(0.02, 0.35, 16);
	
	// Table
	reset();
	y = 0.36;
	box(0.55, 0.01, 0.35);
	
	return 0;
}
