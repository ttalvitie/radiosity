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

### Performance
All the tests were run on the [Ukko cluster](https://www.cs.helsinki.fi/tietotekniikka/laskentaklusteri-ukko).

Edge length limit | Triangles | Time (H:mm:ss) | Memory | Output
----------------- | --------- | -----| ------ | ------
0.024 | 207291 | 15:08:38 | 171.88 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.024.png)
0.028 | 154543 | 8:38:42 | 95.53 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.028.png)
0.032 | 118755 | 5:32:33 | 56.41 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.032.png)
0.036 | 94985 | 3:23:12 | 36.09 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.036.png)
0.040 | 77308 | 1:09:37 | 23.91 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.040.png)
0.044 | 64312 | 0:48:14 | 16.54 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.044.png)
0.048 | 54179 | 0:34:12 | 11.74 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.048.png)
0.052 | 46466 | 0:25:12 | 8.64 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.052.png)
0.056 | 40483 | 0:19:08 | 6.56 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.056.png)
0.060 | 35336 | 0:14:31 | 4.99 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.060.png)
0.064 | 31481 | 0:11:29 | 3.96 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.064.png)
0.068 | 28154 | 0:09:12 | 3.17 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.068.png)
0.072 | 25454 | 0:07:31 | 2.59 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.072.png)
0.076 | 22814 | 0:06:02 | 2.08 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.076.png)
0.080 | 20830 | 0:05:01 | 1.74 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.080.png)
0.084 | 18921 | 0:04:09 | 1.43 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.084.png)
0.088 | 17207 | 0:03:26 | 1.18 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.088.png)
0.092 | 15678 | 0:02:52 | 0.98 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.092.png)
0.096 | 14516 | 0:02:27 | 0.84 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.096.png)

