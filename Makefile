NITROCOMPRESSION = ntrcom/nitrocompression.a
LODEPNG = lodepng/lodepng.o
PHOENIXGFX = phoenixgfx.o
CTRPORTUTILS = ctrportutils.o
OBJS = $(LODEPNG) $(PHOENIXGFX) $(CTRPORTUTILS)

TARGETS_COMPRESSION = find-compressed-files
TARGETS_GENERAL = extract-raw-file gen-uncomheadermap simple-hash decrypt-ios-data
TARGETS_PHOENIX = convert-uncompressed-image-header extract-archive extract-mes_all-bin extract-phoenix_data dump-person-animations
TARGETS_PHOENIX_OFFSET = convert-uncompressed-image-header-offset extract-video-offset extract-image-offset extract-archive-offset
TARGETS_APOLLO = extract-apollo-cpac convert-apollo-image convert-apollo-raw
TARGETS_EDGEWORTH = extract-edgeworth-romfile extract-edgeworth-archive convert-edgeworth-raw generate-edgeworth-tilemap convert-edgeworth-overlay
TARGETS_CITRUS = extract-trilogy-pack
TARGETS_PHOENIXPC = extract-pc-file
TARGETS = $(TARGETS_COMPRESSION) $(TARGETS_GENERAL) $(TARGETS_PHOENIX) $(TARGETS_PHOENIX_OFFSET) $(TARGETS_APOLLO) $(TARGETS_EDGEWORTH) $(TARGETS_CITRUS) $(TARGETS_PHOENIXPC)

CFLAGS += -Wall -g

.PHONY: 3dsetc tools scripting ntrcom

all: $(TARGETS) 3dsetc scripting tools

$(TARGETS): $(OBJS) $(NITROCOMPRESSION)

$(NITROCOMPRESSION):
	$(MAKE) -C ntrcom

scripting:
	$(MAKE) -C scripting

tools:
	$(MAKE) -C tools/gbagfx

3dsetc:
	$(MAKE) -C 3dsetc

extract-video-offset: $(PHOENIXGFX) $(LODEPNG) $(NITROCOMPRESSION)

extract-image-offset: $(PHOENIXGFX) $(LODEPNG) $(NITROCOMPRESSION)

dump-person-animations: $(PHOENIXGFX) $(LODEPNG)

find-compressed-files: $(NITROCOMPRESSION)

extract-phoenix_data: $(PHOENIXGFX) $(LODEPNG) $(NITROCOMPRESSION)

extract-trilogy-pack: $(NITROCOMPRESSION) $(CTRPORTUTILS)

extract-pc-file: LDLIBS=-lz

simple-hash: $(CTRPORTUTILS)

extract-%: $(NITROCOMPRESSION)

extract-raw-file: $(NITROCOMPRESSION)

convert-%: $(PHOENIXGFX) $(LODEPNG) $(NITROCOMPRESSION)

clean:
	$(RM) $(TARGETS) $(OBJS)
	$(MAKE) clean -C tools/gbagfx
	$(MAKE) clean -C scripting
	$(MAKE) clean -C ntrcom
	$(MAKE) clean -C 3dsetc
