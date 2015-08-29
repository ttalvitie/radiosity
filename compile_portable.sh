#!/bin/bash

gcc common.c draw_bmp.c main.c matrix.c radiosity.c triangle.c bmp.c raycast.c -o radiosity -g -Wall -std=c99 -lm -O2
