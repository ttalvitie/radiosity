#include "common.h"
#include "triangle.h"
#include "draw_html.h"

int main(int argc, char* argv[]) {
	if(argc != 3) {
		fail("Usage: ./radiosity <input filename> <HTML output filename>");
	}
	
	triangle* trgs;
	size_t trgcount = read_triangles_from_file(argv[1], &trgs);
	
	normalize_triangle_radiosities(trgs, trgcount);
	
	draw_to_html(argv[2], trgs, trgcount);
	
	return 0;
}