# Quadtree Image Compression Codec

A C codec that compresses grayscale PGM images into a compact quadtree format (`.qtc`) and decodes them back. It supports lossless compression and lossy compression with an adjustable filter.

Individual course project at Université Gustave Eiffel. Comments and the report are in French.

## How it works

- The image (which must be 2^n x 2^n pixels) is split recursively into a quadtree.
- Each node stores the mean of its four children plus small error and uniformity fields, written bit by bit so that fields take fewer than eight bits when possible.
- With `-a <alpha>`, nearly uniform regions are merged, which gives lossy compression; without it the result is lossless.
- `-g` also writes the segmentation grid as an image.

## Build

Requires `gcc` and `make`.

```sh
make            # builds lib/libqtc.so and the codec executable
make doxygen    # optional: generate the API documentation
make clean
```

## Usage

```sh
./codec -c -i input.pgm -o output.qtc           # encode, lossless
./codec -c -i input.pgm -o output.qtc -a 1.5    # encode, lossy
./codec -u -i input.qtc -o output.pgm           # decode
./codec -h
```

| Option | Meaning |
| --- | --- |
| `-c` / `-u` | Encode (PGM to QTC) / decode (QTC to PGM) |
| `-i <file>` | Input file |
| `-o <file>` | Output file (default `out.qtc` or `out.pgm`) |
| `-a <alpha>` | Filter strength for lossy compression |
| `-g` | Write the segmentation grid |

## Layout

| Path | Purpose |
| --- | --- |
| `src/quadtree.c` | Quadtree construction |
| `src/codage.c`, `src/decodage.c` | Encoding and decoding |
| `src/bitreader.c` | Bit-level reading and writing |
| `src/filtrage.c` | Lossy filter |
| `src/pgm_reader.c` | PGM input and output |
| `main/codec.c` | Command-line program |
| `Readme.pdf` | Project report (French) |
