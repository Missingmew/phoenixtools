# Phoenix Tools

Various Ace Attorney related tools n stuff

Consider everything in here unstable and evil. Assume everything will eat your cats and dogs.

---------

For info on the game files and how to get them see [here](datafiles.md).

# Building and debugging

Building should only require a reasonably recent c compiler (e.g. GCC) and make, any required libraries are currently bundled and may be exchanged for submodules in the future (if possible).

The following libraries are currently bundled with the project and will be exchanged for git submodules in the future:
- [lodepng](https://github.com/lvandeve/lodepng)

As long as the code is still included in this repository, see the individual libraries folder for licensing of it

The Makefile is only designed to build on linux, windows users will have to work around that

Useful things for debugging are:
- gdb
- valgrind

**Note:** Code quality and style will vary wildly between different code files. Feel free to propose a style to be used (but do not count on it).
Minimal error checking is in place, but assume that any unexpected input will result in your system eating your cat.

# Terminology

For the sake of simplicity, games may have their names shortened or be referred to by their entry number, see the following table:

| Full Title                                                       | short name       |      |     |
| ---------------------------------------------------------------- | ---------------- | ---- | --- |
| Phoenix Wright: Ace Attorney                                     | phoenix1         | aa1  | gs1 |
| Phoenix Wright: Ace Attorney - Justice for All                   | phoenix2/justice | aa2  | gs2 |
| Phoenix Wright: Ace Attorney - Trials and Tribulations           | phoenix3/trials  | aa3  | gs3 |
| Apollo Justice: Ace Attorney                                     | phoenix4/apollo  | aa4  | gs4 |
| Phoenix Wright: Ace Attorney - Dual Destinies                    | phoenix5         | aa5  | gs5 |
| Phoenix Wright: Ace Attorney - Spirits of Justice                | phoenix6         | aa6  | gs6 |
| Miles Edgeworth: Ace Attorney Investigations                     | edgeworth1       | aai1 | gk1 |
| Miles Edgeworth: Ace Attorney Investigations - Prosecutor's Path | edgeworth2       | aai2 | gk2 |
| Phoenix Wright: Ace Attorney Trilogy                             | trilogy          |      |     |

Not all AA related games are included here, and not all of the above listed games have tools in this repo.

# Overview of included code and documentation

Just about all tools have a minimal "how to use" when run with no arguments.
Documentation is basically nonexistant but should be provided at _some_ point in the future...
Anything not specified below should be either self-explanatory or considered testing/scratchpad code/docs.

## filemap*.txt/xlsx
Filemaps for the specified game/region for phoenix1-3 created by checking data.bin by hand.
The spreadsheets contain the data with various notes, while the textfiles are maps that should be ready for use by extract-phoenix_data.
The spreadsheets do not contain all individual textures (most uncompressed ones are grouped together) and the maps do not yet contain all data found in the spreadsheets.
Paths found in the maps are also subject to change due to findings in how the scripts handle graphics.

## apollo cpac map.txt
List of what kinds of data are found in the various cpacs in apollo.

## lodepng/
Contains lodepng used to store converted images

## ntrcom/
Contains various NDS related compression code, subject to be replaced because its way too unstable

## convert-*

### apollo-image
Converts images from apollo that have a header to png

### apollo-raw
Converts raw images from apollo justice to png

### edgeworth-overlay
Converts scripts from overlays found in edgeworth1 into readable text

### edgeworth-raw
Converts graphics from edgeworth1 to png

### text-messages
Converts the scripts extracted via extract-mes_all-bin from phoenix1-4 into readable text

### uncompressed-image-header
Converts graphics found in phoenix1-3 with a header to png

### uncompressed-image-header-offset
Same as uncompressed-image-header but reads from a bigfile, probably not needed anymore...

## extract-*

### apollo-cpac
Extracts files from cpac*.bin found in apollo

### archive
Extracts archives found in phoenix1-3

### archive-offset
Same as archive but reads from a bigfile, probably not needed anymore...

### edgeworth-archive
Extracts archives found in edgeworth2

### edgeworth-romfile
Extracts romfile.bin found in edgeworth1

### image-offset
Converts images found in phoenix1-3 from an offset, probably not needed anymore...

### mes_all-bin
Extracts mes_all.bin from phoenix1-4 into individual script binaries, use convert-text-messages to get readable text

### phoenix_data
Uses mapfiles to extract data.bin from phoenix1-3, use this instead of *-offset

### raw-file
Dumps specified amount of data straight from a file

### trilogy-pack
Extracts the pack.* files from trilogy

### video-offset
Extracts "videos" from phoenix1-3 from bigfile, probably not needed anymore...

## find-compressed-files
Scans a specified file/range for files compressed using the LZ compression found in the GBA/NDS BIOS, uses its own decompressor for more reliability

## generate-edgeworth-tilemap
Generates tilemaps to be used with convert-edgeworth-raw from textfiles

## phoenixgfx.*
Various functions/structs for handling graphics data found in the games (tiled, linear, indexed, etc.)

# Credits
Some NDS and AA specific code was based on code and documentation made by henke37  
Other bits and pieces were gathered from GBATeK  
And probably more that I forgot over time  