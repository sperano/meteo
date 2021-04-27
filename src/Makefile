VERSION := 0.3
CC := cl65
LD := ld65
CFLAGS := -DMETEO_VERSION=\"$(VERSION)\" -O -t apple2enh -C apple2enh-system.cfg -I../include -I../vendor/json65-master/src/ -I../vendor/ip65 -DMOCK_IP65
# 8192 is the default starting
LDFLAGS := -C apple2enh-system.cfg
_DEPS := bitmaps.h config.h gfx.h parser.h types.h utils.h
DEPS = $(patsubst %,../include/%,$(_DEPS))

OBJ = bitmaps.o config.o gfx.o main.o parser.o utils.o json65.o
OBJS2 = fetch.o config.o utils.o

meteo.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ apple2enh.lib ../vendor/ip65/ip65_tcp.lib ../vendor/ip65/ip65_apple2.lib

fetch.bin: $(OBJS2)
	$(LD) $(LDFLAGS) --start-addr 0x0C00 -o $@ $^ apple2enh-iobuf-0800.o apple2enh.lib ../vendor/ip65/ip65_tcp.lib ../vendor/ip65/ip65_apple2.lib

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *~ meteo.bin
