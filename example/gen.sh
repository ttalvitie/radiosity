#!/bin/bash

set -e

gcc scene.c -o scene -g -Wall -std=c99 -lm -O2
./scene > example.in
../radiosity example.in example.svg 0.1 0.3 0.8 -2.3 8.5 -13 0
