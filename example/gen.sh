#!/bin/bash

set -e

gcc scene.c -o scene -g -Wall -std=c99 -lm -O2
./scene > example.in
../radiosity example.in example.bmp 0.06 -0.7 0.8 -2.3 -18 -13 0
convert example.bmp example.png
