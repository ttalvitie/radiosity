# radiosity
Radiosity lightning implemented in C.

Doxygen documentation for the code is available in http://www.cs.helsinki.fi/u/totalvit/radiosity/doc/files.html

### Example scene
![Example scene](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.024.png)

### Rendering the example scene
First compile the code with
```
$ ./compile.sh
```
If it doesn't work, try `./compile_portable.sh` or `./compile_mac.sh`.

Then enter the `example` directory, and run `./gen.sh`
```
$ cd example
$ ./gen.sh
```
`gen.sh` is a script that compiles and runs a scene generation program, generating the file `example.in` containing a description of all the triangles. Using this file as input, it runs the radiosity renderer to that file, generating the output image `example.bmp`. In the end, it also converts this BMP image to a PNG image `example.png` (this might not work if you do not have ImageMagick installed). For more details, read the script `gen.sh`.

As an optional parameter, you may specify the edge length limit to `./gen.sh`. The default value is `0.2`. If you specify a smaller value, the triangles are subdivided into smaller triangles and thus the rendered image will have larger quality. Of course, this drastically increases the time and memory complexity.

