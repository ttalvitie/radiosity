#!/bin/bash

gcc common.c draw_bmp.c main.c matrix.c radiosity.c triangle.c bmp.c raycast.c -o radiosity -g -Wall -std=gnu99 -lm -O3 -funroll-loops -mfpmath=sse -pthread -DWORKERTHREADS=24
# For matrices that do not fit in memory, add -DLARGE_MATRIX_TMPFILE_TEMPLATE=\"/path/to/tmpXXXXXX\"
