#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

float x, y, z, ref, emi;
int inv;

void reset() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	ref = 0.9;
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
	ref = 0.8;
	quad(
		-1, 1, -1,
		-1, 1, 1,
		1, 1, 1,
		1, 1, -1
	);
	reset();
	quad(
		-1, 0, -1,
		-1, 0, 1,
		-1, 1, 1,
		-1, 1, -1
	);
	quad(
		-1, 0, 1,
		1, 0, 1,
		1, 1, 1,
		-1, 1, 1
	);
	
	// Wall with hole
	float holey1 = 0.3;
	float holey2 = 0.95;
	float holez1 = -0.2;
	float holez2 = 0.5;
	float holemy = 0.5 * (holey1 + holey2);
	float holemz = 0.5 * (holez1 + holez2);
	quad(
		1, 0, -1,
		1, 1, -1,
		1, 1, holez1,
		1, 0, holez1
	);
	quad(
		1, 0, holez2,
		1, 1, holez2,
		1, 1, 1,
		1, 0, 1
	);
	quad(
		1, 0, holez1,
		1, holey1, holez1,
		1, holey1, holez2,
		1, 0, holez2
	);
	quad(
		1, holey2, holez1,
		1, 1, holez1,
		1, 1, holez2,
		1, holey2, holez2
	);
	
	float barsz = 0.02;
	quad(
		1, holey1, holemz - barsz,
		1, holey2, holemz - barsz,
		1, holey2, holemz + barsz,
		1, holey1, holemz + barsz
	);
	quad(
		1, holemy - barsz, holez1,
		1, holemy + barsz, holez1,
		1, holemy + barsz, holemz - barsz,
		1, holemy - barsz, holemz - barsz
	);
	quad(
		1, holemy - barsz, holemz + barsz,
		1, holemy + barsz, holemz + barsz,
		1, holemy + barsz, holez2,
		1, holemy - barsz, holez2
	);
	
	// Deepen window
	double windepth = 0.04;
	quad(
		1, holey1, holez1,
		1 + windepth, holey1, holez1,
		1 + windepth, holey1, holemz - barsz,
		1, holey1, holemz - barsz
	);
	quad(
		1, holey1, holemz + barsz,
		1 + windepth, holey1, holemz + barsz,
		1 + windepth, holey1, holez2,
		1, holey1, holez2
	);
	quad(
		1, holemy + barsz, holez1,
		1 + windepth, holemy + barsz, holez1,
		1 + windepth, holemy + barsz, holemz - barsz,
		1, holemy + barsz, holemz - barsz
	);
	quad(
		1, holemy + barsz, holemz + barsz,
		1 + windepth, holemy + barsz, holemz + barsz,
		1 + windepth, holemy + barsz, holez2,
		1, holemy + barsz, holez2
	);
	
	inv = !inv;
	quad(
		1, holey2, holez1,
		1 + windepth, holey2, holez1,
		1 + windepth, holey2, holemz - barsz,
		1, holey2, holemz - barsz
	);
	quad(
		1, holey2, holemz + barsz,
		1 + windepth, holey2, holemz + barsz,
		1 + windepth, holey2, holez2,
		1, holey2, holez2
	);
	quad(
		1, holemy - barsz, holez1,
		1 + windepth, holemy - barsz, holez1,
		1 + windepth, holemy - barsz, holemz - barsz,
		1, holemy - barsz, holemz - barsz
	);
	quad(
		1, holemy - barsz, holemz + barsz,
		1 + windepth, holemy - barsz, holemz + barsz,
		1 + windepth, holemy - barsz, holez2,
		1, holemy - barsz, holez2
	);
	inv = !inv;
	
	quad(
		1, holey1, holez2,
		1 + windepth, holey1, holez2,
		1 + windepth, holemy - barsz, holez2,
		1, holemy - barsz, holez2
	);
	quad(
		1, holemy + barsz, holez2,
		1 + windepth, holemy + barsz, holez2,
		1 + windepth, holey2, holez2,
		1, holey2, holez2
	);
	quad(
		1, holey1, holemz - barsz,
		1 + windepth, holey1, holemz - barsz,
		1 + windepth, holemy - barsz, holemz - barsz,
		1, holemy - barsz, holemz - barsz
	);
	quad(
		1, holemy + barsz, holemz - barsz,
		1 + windepth, holemy + barsz, holemz - barsz,
		1 + windepth, holey2, holemz - barsz,
		1, holey2, holemz - barsz
	);
	
	inv = !inv;
	quad(
		1, holey1, holez1,
		1 + windepth, holey1, holez1,
		1 + windepth, holemy - barsz, holez1,
		1, holemy - barsz, holez1
	);
	quad(
		1, holemy + barsz, holez1,
		1 + windepth, holemy + barsz, holez1,
		1 + windepth, holey2, holez1,
		1, holey2, holez1
	);
	quad(
		1, holey1, holemz + barsz,
		1 + windepth, holey1, holemz + barsz,
		1 + windepth, holemy - barsz, holemz + barsz,
		1, holemy - barsz, holemz + barsz
	);
	quad(
		1, holemy + barsz, holemz + barsz,
		1 + windepth, holemy + barsz, holemz + barsz,
		1 + windepth, holey2, holemz + barsz,
		1, holey2, holemz + barsz
	);
	inv = !inv;
	
	// Sun
	float sunsz = 0.6;
	emi = 12.5 / (sunsz * sunsz);
	float suny = 5;
	float sunx = 5;
	float sunz = -1.0 + sunsz;
	quad(
		sunx, suny - sunsz, sunz - sunsz,
		sunx, suny + sunsz, sunz - sunsz,
		sunx, suny + sunsz, sunz + sunsz,
		sunx, suny - sunsz, sunz + sunsz
	);
	
	// Lamp
	emi = 2;
	y = 0.725;
	x = -0.3;
	sphere(0.075, 10);
	
	// Lamp bar
	reset();
	ref = 0.3;
	y = 0.8;
	x = -0.3;
	y_cylinder(0.005, 0.2, 10);
	
	// Table legs
	reset();
	ref = 0.7;
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
	ref = 0.7;
	y = 0.36;
	box(0.55, 0.01, 0.35);
	
	return 0;
}
