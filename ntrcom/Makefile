COMOBJS = nitrocompression.o lz.o
COMAR = nitrocompression.a
TARGETS = simpleunpak simpleunpak-offset

CFLAGS = -Wall -g

all: $(TARGETS)

$(COMAR): $(COMAR)($(COMOBJS))

simpleunpak: $(COMAR)

simpleunpak-offset: $(COMAR)

clean:
	$(RM) $(COMAR) $(TARGETS)
