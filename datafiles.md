# Ace Attorney Datafiles
---------------

This is a quick and dirty overview about what does what.
For phoenix1-3, this document only considers the NDS ports.

Usually, you will find the games as a ROM image (*.nds), which you will need to unpack first to get to the actual files of interest.
Tools for that should be rather easy to find, some examples:

- Tinke
- NitroExplorer

For overlays, the way they get named and the folders they get extracted to can differ between tools, but the data should always be the same.

The main focus of this repo are the NDS games, so phoenix5, 6 and trilogy are omitted for now.

# data.bin (found in phoenix1-3)
Contains just about all graphics and models used in the games.
This is a binary blob with no known index file, so map files and offsets are region/release specific.
Files inside can be uncompressed, compressed or archived.

# mes_all.bin (found in phoenix1-4)
Contains game scripts for all supported languages. Scripts themselves are compressed and in a game specific format.

# cpac*.bin (found in phoenix4)
Contains game graphics and models used in the game.

# *.vx (found in phoenix4)
Mobiclip encoded video files.

# *.pac (found in phoenix4)
"Videos" consisting of a series of single images

# romfile.bin (found in edgeworth1)
Similar to data.bin from phoenix1-3, but this time with a table for each individual file (only offsets, sizes, no names).

# overlay9_* (relevant in edgeworth1)
ARM9 overlays which contain compiled code and data loaded at runtime (think DLL/SO but even more dynamic), contain game scripts and possibly more in edgeworth1.

# *.mods (found in edgeworth2)
Mobiclip encoded video files, newer codec version compared to phoenix4.

# *.bin (relevant in edgeworth2)
Various kinds of archives containing game data.

# dtb,hmm,mdc,phn,str,tree.bin (found in phoenix1-4 and edgeworth1)
Apparently language specific files of currently unknown use.

# *.sdat
Standard NDS/Nitro sound archive. Note that for phoenix3, most available tools will not find all voice samples for the eu (de/fr) version.

Any files not listed here have not been checked yet.