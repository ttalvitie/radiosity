#include "common.h"
#include "triangle.h"
#include "draw_bmp.h"
#include "radiosity.h"
#include "matrix.h"
#include "raycast.h"

#include <errno.h>

static float param_to_float(const char* s) {
	errno = 0;
	char* endptr;
	float ret = strtod(s, &endptr);
	if(errno || *endptr != '\0') {
		fail("Invalid real number argument '%s'.", s);
	}
	return ret;
}

static void apply_transform(
	triangle* trgs, size_t trgcount,
	float x, float y, float z,
	float yaw, float pitch, float roll
) {
	float a = yaw;
	float yaw_matrix_data[] = {
		 cos(a),     0.0,  sin(a),
		    0.0,     1.0,     0.0,
		-sin(a),     0.0,  cos(a)
	};
	a = pitch;
	float pitch_matrix_data[] = {
		    1.0,     0.0,     0.0,
		    0.0,  cos(a), -sin(a),
		    0.0,  sin(a),  cos(a)
	};
	a = roll;
	float roll_matrix_data[] = {
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
	
	float pos1_data[3];
	vector pos1;
	pos1.size = 3;
	pos1.data = pos1_data;
	float pos2_data[3];
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
			"Usage: ./radiosity <input filename> <BMP output filename> "
			"<edge length limit used in triangle subdivision>"
			"<camera x> <camera y> <camera z> "
			"<camera yaw> <camera pitch> <camera roll> "
		);
	}
	
	float edge_length_limit = param_to_float(argv[3]);
	float camera_x = param_to_float(argv[4]);
	float camera_y = param_to_float(argv[5]);
	float camera_z = param_to_float(argv[6]);
	float camera_yaw = PI * param_to_float(argv[7]) / 180.0;
	float camera_pitch = PI * param_to_float(argv[8]) / 180.0;
	float camera_roll = PI * param_to_float(argv[9]) / 180.0;
	
	triangle* orig_trgs;
	size_t orig_trgcount;
	orig_trgcount = read_triangles_from_file(argv[1], &orig_trgs);
	
	// To accomplish the right camera angle, we transform all vertices such that
	// the camera is in (0, 0, 0) facing towards the positive z-axis.
	apply_transform(
		orig_trgs, orig_trgcount,
		-camera_x, -camera_y, -camera_z,
		camera_yaw, camera_pitch, camera_roll
	);
	
	triangle* trgs;
	size_t trgcount;
	trgcount = subdivide_triangles(
		orig_trgs, orig_trgcount, edge_length_limit, &trgs
	);
	
	// Shuffle the triangles so that progress meters works more accurately.
	for(size_t i = 0; i < trgcount; ++i) {
		size_t j = rand() % (i + 1);
		triangle tmp = trgs[i];
		trgs[i] = trgs[j];
		trgs[j] = tmp;
	}
	
	raycast raycast_ctx = raycast_init(orig_trgs, orig_trgcount);
	
	printf(
		"Read %zu triangles, subdivided to %zu triangles using edge length "
		"limit %f.\n",
		orig_trgcount, trgcount, edge_length_limit
	);
	
	compute_radiosity(trgs, trgcount, raycast_ctx);
	
	normalize_triangle_radiosities(trgs, trgcount);
	draw_to_bmp(argv[2], trgs, trgcount);
	
	raycast_free(raycast_ctx);
	free(orig_trgs);
	free(trgs);
	
	return 0;
}
