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
All the tests were run on the [Ukko cluster](https://www.cs.helsinki.fi/tietotekniikka/laskentaklusteri-ukko). Note that the tests with memory usage above 32G had to use disk storage, causing worse performance.

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
0.100 | 13331 | 0:02:03 | 0.71 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.100.png)
0.104 | 12458 | 0:01:48 | 0.62 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.104.png)
0.108 | 11458 | 0:01:31 | 0.53 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.108.png)
0.112 | 10744 | 0:01:20 | 0.46 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.112.png)
0.116 | 10072 | 0:01:10 | 0.41 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.116.png)
0.120 | 9567 | 0:01:04 | 0.37 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.120.png)
0.124 | 9020 | 0:00:56 | 0.33 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.124.png)
0.128 | 8601 | 0:00:51 | 0.30 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.128.png)
0.132 | 8238 | 0:00:47 | 0.27 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.132.png)
0.136 | 7799 | 0:00:42 | 0.24 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.136.png)
0.140 | 7470 | 0:00:39 | 0.22 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.140.png)
0.144 | 7083 | 0:00:35 | 0.20 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.144.png)
0.148 | 6704 | 0:00:31 | 0.18 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.148.png)
0.152 | 6386 | 0:00:28 | 0.16 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.152.png)
0.156 | 6036 | 0:00:25 | 0.15 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.156.png)
0.160 | 5820 | 0:00:23 | 0.14 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.160.png)
0.164 | 5537 | 0:00:21 | 0.12 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.164.png)
0.168 | 5227 | 0:00:19 | 0.11 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.168.png)
0.172 | 5067 | 0:00:18 | 0.10 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.172.png)
0.176 | 4854 | 0:00:16 | 0.09 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.176.png)
0.180 | 4562 | 0:00:14 | 0.08 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.180.png)
0.184 | 4324 | 0:00:13 | 0.07 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.184.png)
0.188 | 4185 | 0:00:12 | 0.07 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.188.png)
0.192 | 4006 | 0:00:11 | 0.06 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.192.png)
0.196 | 3866 | 0:00:10 | 0.06 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.196.png)
0.200 | 3690 | 0:00:10 | 0.05 GB | [PNG](http://www.cs.helsinki.fi/u/totalvit/radiosity/example/example0.200.png)

