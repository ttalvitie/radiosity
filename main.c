#include "common.h"
#include "triangle.h"
#include "draw_html.h"
#include "radiosity.h"
#include "matrix.h"

#include <errno.h>

static double param_to_double(const char* s) {
	errno = 0;
	char* endptr;
	double ret = strtod(s, &endptr);
	if(errno || *endptr != '\0') {
		fail("Invalid real number argument '%s'.", s);
	}
	return ret;
}

static void apply_transform(
	triangle* trgs, size_t trgcount,
	double x, double y, double z,
	double yaw, double pitch, double roll
) {
	double a = yaw;
	double yaw_matrix_data[] = {
		 cos(a),     0.0,  sin(a),
		    0.0,     1.0,     0.0,
		-sin(a),     0.0,  cos(a)
	};
	a = pitch;
	double pitch_matrix_data[] = {
		    1.0,     0.0,     0.0,
		    0.0,  cos(a), -sin(a),
		    0.0,  sin(a),  cos(a)
	};
	a = roll;
	double roll_matrix_data[] = {
		 cos(a), -sin(a),     0.0,
		 sin(a),  cos(a),     0.0,
		    0.0,     0.0,     1.0
	};
	
	matrix yaw_matrix;
	yaw_matrix.size = 3;
	yaw_matrix.data = yaw_matrix_data;
	matrix pitch_matrix;
	pitch_matrix.size = 3;
	pitch_matrix.data = pitch_matrix_data;
	matrix roll_matrix;
	roll_matrix.size = 3;
	roll_matrix.data = roll_matrix_data;
	
	double pos1_data[3];
	vector pos1;
	pos1.size = 3;
	pos1.data = pos1_data;
	double pos2_data[3];
	vector pos2;
	pos2.size = 3;
	pos2.data = pos2_data;
	
	for(size_t i = 0; i < trgcount; ++i) {
		for(int j = 0; j < 3; ++j) {
			pos1_data[0] = trgs[i].corners[j].x + x;
			pos1_data[1] = trgs[i].corners[j].y + y;
			pos1_data[2] = trgs[i].corners[j].z + z;
			
			matrix_vector_mul(yaw_matrix, pos1, pos2);
			matrix_vector_mul(pitch_matrix, pos2, pos1);
			matrix_vector_mul(roll_matrix, pos1, pos2);
			
			trgs[i].corners[j].x = pos2_data[0];
			trgs[i].corners[j].y = pos2_data[1];
			trgs[i].corners[j].z = pos2_data[2];
		}
	}
}

int main(int argc, char* argv[]) {
	if(argc != 10) {
		fail(
			"Usage: ./radiosity <input filename> <HTML output filename> "
			"<edge length limit used in triangle subdivision>"
			"<camera x> <camera y> <camera z> "
			"<camera yaw> <camera pitch> <camera roll> "
		);
	}
	
	double edge_length_limit = param_to_double(argv[3]);
	double camera_x = param_to_double(argv[4]);
	double camera_y = param_to_double(argv[5]);
	double camera_z = param_to_double(argv[6]);
	double camera_yaw = PI * param_to_double(argv[7]) / 180.0;
	double camera_pitch = PI * param_to_double(argv[8]) / 180.0;
	double camera_roll = PI * param_to_double(argv[9]) / 180.0;
	
	triangle* trgs;
	size_t trgcount;
	trgcount = read_triangles_from_file(argv[1], edge_length_limit, &trgs);
	
	// To accomplish the right camera angle, we transform all vertices such that
	// the camera is in (0, 0, 0) facing towards the positive z-axis.
	apply_transform(
		trgs, trgcount,
		-camera_x, -camera_y, -camera_z,
		camera_yaw, camera_pitch, camera_roll
	);
	
	trgs[1].emitted_energy = 150;
	trgs[531].emitted_energy = 5;
	
	printf("Read %zu triangles", trgcount);
	if(edge_length_limit > 0.0) {
		printf(" (using edge length limit %lf)", edge_length_limit);
	}
	printf("\n");
	
	compute_radiosity(trgs, trgcount);
	
	normalize_triangle_radiosities(trgs, trgcount);
	draw_to_html(argv[2], trgs, trgcount);
	
	return 0;
}