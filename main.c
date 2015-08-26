#include "common.h"
#include "triangle.h"
#include "draw_html.h"
#include "radiosity.h"

#include <errno.h>

int main(int argc, char* argv[]) {
	if(argc < 3 || argc > 4) {
		fail(
			"Usage: ./radiosity <input filename> <HTML output filename> "
			"[<edge length limit used in triangle subdivision>]"
		);
	}
	
	double edge_length_limit = 0.0;
	if(argc == 4) {
		errno = 0;
		char* endptr;
		edge_length_limit = strtod(argv[3], &endptr);
		if(errno || *endptr != '\0') {
			fail("Invalid edge length limit for triangle subdivision.");
		}
	}
	
	triangle* trgs;
	size_t trgcount;
	trgcount = read_triangles_from_file(argv[1], edge_length_limit, &trgs);
	
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