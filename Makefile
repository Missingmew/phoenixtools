# NITROCOMPRESSION = ntrcom/lze.o ntrcom/blz.o ntrcom/huffman.o ntrcom/lzss.o ntrcom/lzx.o ntrcom/rle.o ntrcom/ndscompression.o
#~ NITROCOMPRESSION = ntrcom/huffman.o ntrcom/lzss.o ntrcom/lzx.o ntrcom/rle.o ntrcom/nitrocompression.o
NITROCOMPRESSION = ntrcom/nitrocompression.o
LODEPNG = lodepng/lodepng.o
PHOENIXLIB = phoenixgfx.o

.PHONY: tools

# all: compression phoenix apollo edgeworth
all: compression general phoenix phoenix-offset apollo edgeworth citrus phoenixpc tools test
compression: ntrcom/simpleunpak.elf ntrcom/simpleunpak-offset.elf find-compressed-files.elf
general: extract-raw-file.elf gen-uncomheadermap.elf
phoenix: convert-text-messages.elf convert-uncompressed-image-header.elf extract-archive.elf extract-mes_all-bin.elf extract-phoenix_data.elf
phoenix-offset: convert-uncompressed-image-header-offset.elf extract-video-offset.elf extract-image-offset.elf extract-archive-offset.elf
apollo: extract-apollo-cpac.elf convert-apollo-image.elf convert-apollo-raw.elf
edgeworth: extract-edgeworth-romfile.elf extract-edgeworth-archive.elf convert-edgeworth-raw.elf generate-edgeworth-tilemap.elf convert-edgeworth-overlay.elf
citrus: extract-trilogy-pack.elf 3dsetc/etc1util.elf
phoenixpc: extract-pc-file.elf
test: test-char-oam.elf
	
tools:
	$(MAKE) -C tools/gbagfx

extract-video-offset.elf: $(PHOENIXLIB) $(LODEPNG) $(NITROCOMPRESSION) extract-video-offset.c
	$(CC) -Wall -g -o extract-video-offset.elf extract-video-offset.c $(PHOENIXLIB) $(LODEPNG) $(NITROCOMPRESSION)
	
extract-image-offset.elf: $(PHOENIXLIB) $(LODEPNG) $(NITROCOMPRESSION) extract-image-offset.c
	$(CC) -Wall -g -o extract-image-offset.elf extract-image-offset.c $(PHOENIXLIB) $(LODEPNG) $(NITROCOMPRESSION)
	
3dsetc/etc1util.elf: 3dsetc/main.c
	$(CXX) -Wall -g -o 3dsetc/etc1util.elf 3dsetc/main.c 3dsetc/rg_etc1.cpp
	
ntrcom/%.elf: $(NITROCOMPRESSION) ntrcom/%.c
	$(CC) -Wall -g -o $@ $^
	
extract-raw-file.elf: $(NITROCOMPRESSION) extract-raw-file.c
	$(CC) -Wall -g -o $@ $^
	
find-compressed-files.elf: $(NITROCOMPRESSION) find-compressed-files.c
	$(CC) -Wall -g -o $@ $^
	
extract-phoenix_data.elf: $(PHOENIXLIB) $(LODEPNG) $(NITROCOMPRESSION) extract-phoenix_data.c
	$(CC) -Wall -g -o $@ $^
	
extract-%.elf: $(NITROCOMPRESSION) extract-%.c
	$(CC) -Wall -g -o $@ $^
	
convert-%.elf: $(PHOENIXLIB) $(LODEPNG) $(NITROCOMPRESSION) convert-%.c
	$(CC) -Wall -g -o $@ $^
	
extract-pc-file.elf: extract-pc-file.c
	$(CC) -Wall -g -o $@ $^ -lz
	
%.elf: %.c
	$(CC) -Wall -g -o $@ $<
	
%.o: %.c
	$(CC) -c -Wall -g -o $@ $<

clean:
	-rm ntrcom/*.o ntrcom/*.elf *.elf lodepng/*.o *.o
	$(MAKE) clean -C tools/gbagfx
