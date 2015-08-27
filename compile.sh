#!/bin/bash

gcc common.c draw_svg.c main.c matrix.c radiosity.c triangle.c -o radiosity -g -Wall -std=c99 -lm -Ofast -funroll-loops -march=native -mfpmath=sse
