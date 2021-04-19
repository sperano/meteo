VERSION := 0.2
CC := cl65
LD := ld65
CFLAGS := -DMETEO_VERSION=\"$(VERSION)\" -Cl -O -t apple2enh -C apple2enh.cfg -I../include -I../vendor/json65-master/src/ -I../vendor/ip65
# 8192 is the default starting
LDFLAGS := -t apple2enh
_DEPS := bitmaps.h config.h gfx.h parser.h types.h utils.h
DEPS = $(patsubst %,../include/%,$(_DEPS))

OBJ = bitmaps.o config.o gfx.o main.o parser.o utils.o json65.o json65-file.o json65-string.o json65-tree.o

meteo.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ apple2enh.lib ../vendor/ip65/ip65_tcp.lib ../vendor/ip65/ip65_apple2.lib

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *~ meteo.bin
