# MoC

⚠️ As of 08/06/26 the code is not finished ! (but it will be soon)
Before it is done, you can still use the ![python version](https://github.com/FourcadeL/mosaic).

A mosaic creator written in C
This utility can be used to generate
[photo-mosaic](http://en.wikipedia.org/wiki/Photographic_mosaic) images.

This is a complete rewrite of another project you can find
![here](https://github.com/FourcadeL/mosaic) which was itself a fork of ![this project](https://github.com/codebox/mosaic).

The more tiles in the mosaic, the better !
But also, the harder for the poor Python PIL library ...
This rewrite in C is much more efficient
so you can make even prettier mosaics 💅 !

## How to build ?

There is a build script right at the project's root ⚙️

## How to use ?

Once buils, you will find the **moc** executable in the **build** directory.

To use it, just run :

```bash
./moc input_image input_tiles_path
```

The tweaking optins will be listed with

```bash
./moc -h
```
