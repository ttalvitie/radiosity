#include <stdio.h>

double x = 0.0;
double y = 0.0;
double z = 0.0;
double ref = 0.5;
double emi = 0.0;
int inv = 0;
void trg(
	double x1, double y1, double z1,
	double x2, double y2, double z2,
	double x3, double y3, double z3
) {
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

int main() {
	emi = 1;
	quad(
		-1, 0, -1,
		1, 0, -1,
		1, 0, 1,
		-1, 0, 1
	);
	emi = 0;
	
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
	
	return 0;
}
