#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846

double x, y, z, ref, emi;
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
	double x1, double y1, double z1,
	double x2, double y2, double z2,
	double x3, double y3, double z3
) {
	if(x1 == x2 && y1 == y2 && z1 == z2) return;
	if(x1 == x3 && y1 == y3 && z1 == z3) return;
	if(x2 == x3 && y2 == y3 && z2 == z3) return;
	
	x1 += x; y1 += y; z1 += z;
	x2 += x; y2 += y; z2 += z;
	x3 += x; y3 += y; z3 += z;
	if(inv) {
		printf(
			"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
			x1, y1, z1, x3, y3, z3, x2, y2, z2, ref, emi
		);
	} else {
		printf(
			"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
			x1, y1, z1, x2, y2, z2, x3, y3, z3, ref, emi
		);
	}
}
void quad(
	double x1, double y1, double z1,
	double x2, double y2, double z2,
	double x3, double y3, double z3,
	double x4, double y4, double z4
) {
	trg(x1, y1, z1, x2, y2, z2, x3, y3, z3);
	trg(x1, y1, z1, x3, y3, z3, x4, y4, z4);
}

void sphere(double r, int detail) {
	for(int i = 0; i < detail; ++i) {
		double a = r * sin(PI * ((double)i / (double)detail - 0.5));
		double b = r * sin(PI * ((double)(i + 1) / (double)detail - 0.5));
		double ra = sqrt(r * r - a * a);
		double rb = sqrt(r * r - b * b);
		
		for(int j = 0; j < detail; ++j) {
			double p = 2.0 * PI * (double)j / (double)detail;
			double q = 2.0 * PI * (double)(j + 1) / (double)detail;
			
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
void halfsphere(double r, int detail) {
	for(int i = 0; i < detail; ++i) {
		double a = r * sin(PI * ((double)i / (double)detail - 0.5));
		double b = r * sin(PI * ((double)(i + 1) / (double)detail - 0.5));
		double ra = sqrt(r * r - a * a);
		double rb = sqrt(r * r - b * b);
		
		for(int j = 0; j < detail; ++j) {
			double p = 0.5 * PI + PI * (double)j / (double)detail;
			double q = 0.5 * PI + PI * (double)(j + 1) / (double)detail;
			quad(
				ra * cos(p), a, ra * sin(p),
				ra * cos(q), a, ra * sin(q),
				rb * cos(q), b, rb * sin(q),
				rb * cos(p), b, rb * sin(p)
			);
		}
		
	}
}
void y_cylinder(double r, double h, int detail, int levels) {
	for(int i = 0; i < levels; ++i) {
		double a = (double)i / (double)levels;
		double b = (double)(i + 1) / (double)levels;
		
		for(int j = 0; j < detail; ++j) {
			double p = 2.0 * PI * (double)j / (double)detail;
			double q = 2.0 * PI * (double)(j + 1) / (double)detail;
			quad(
				r * cos(p), a * h, r * sin(p),
				r * cos(q), a * h, r * sin(q),
				r * cos(q), b * h, r * sin(q),
				r * cos(p), b * h, r * sin(p)
			);
		}
	}
}

void box(double a, double b, double c) {
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
	sphere(0.075, 20);
	
	// Lamp bar
	reset();
	ref = 0.3;
	y = 0.85;
	x = -0.2;
	y_cylinder(0.01, 0.15, 10, 10);
	
	// Wall lamp
	reset();
	emi = 0.5;
	x = 1;
	y = 0.55;
	z = -0.3;
	halfsphere(0.05, 10);
	
	// Table legs
	reset();
	x = -0.5;
	z = -0.3;
	y_cylinder(0.02, 0.35, 10, 10);
	x = -x;
	y_cylinder(0.02, 0.35, 10, 10);
	z = -z;
	y_cylinder(0.02, 0.35, 10, 10);
	x = -x;
	y_cylinder(0.02, 0.35, 10, 10);
	
	// Table
	reset();
	y = 0.36;
	box(0.55, 0.01, 0.35);
	
	return 0;
}
