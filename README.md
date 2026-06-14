# MoC

A mosaic creator written in C
This utility can be used to generate
[photo-mosaic](http://en.wikipedia.org/wiki/Photographic_mosaic) images.

This is a complete rewrite of another project you can find
![here](https://github.com/FourcadeL/mosaic) which was itself a fork of
![this project](https://github.com/codebox/mosaic).

The more tiles in the mosaic, the better !
But also, the harder for the poor Python PIL library ...
This rewrite in C is much more efficient
so you can make even prettier mosaics 💅 !

## How to build ?

There is a build script right at the project's root ⚙️

Dependencies for the *build.sh* script are :

- cmake
- a C compilation tool chain

## How to use ?

Once built, you will find the **moc** executable in the **build** directory.

To use it, just run :

```bash
./moc input_image input_tiles_path
```

The tweaking options will be listed with

```bash
./moc -h
```

## Some things to keep in mind

- The tool will load all of the images from the input directory in RAM
  during execution. You will only get as much data as you have memory available.
- The final image's dimensions are defined by the **IMRES** and **TILERES** parameter
  don't use extensively large tiles if you don't want a 500Mo output image.
- The **THREADS** parameter will default to the number of available cores on
  your machine there is no performance gain passed this threshold.
