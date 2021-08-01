VERSION := 0.6
CC := cl65
LD := ld65
CFLAGS := -DMETEO_VERSION=\"$(VERSION)\" -O -t apple2enh -C apple2enh-system.cfg -I../include -I../vendor/json65-master/src/ -I../vendor/ip65
#CFLAGS := -DMETEO_VERSION=\"$(VERSION)\" -O -t apple2enh -C apple2enh.cfg -I../include -I../vendor/json65-master/src/ -I../vendor/ip65 -DMOCK_IP65

# 8192 is the default starting
LDFLAGS := -C apple2enh-system.cfg
_DEPS := bitmaps.h config.h gfx.h net.h parser.h types.h utils.h
DEPS = $(patsubst %,../include/%,$(_DEPS))

OBJ = config.o gfx.o main.o net.o parser.o utils.o json65.o
OBJ_M = config.o gfx.o main.o net_mock.o parser.o utils.o json65.o
OBJS2 = fetch.o config.o utils.o

LIBS := apple2enh.lib ../vendor/ip65/ip65_tcp.lib ../vendor/ip65/ip65_apple2.lib

meteo.system: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ apple2-iobuf-0800.o $(LIBS)

meteom.system: $(OBJ_M)
	$(LD) $(LDFLAGS) -o $@ $^ apple2-iobuf-0800.o $(LIBS)

fetch.bin: $(OBJS2)
	$(LD) $(LDFLAGS) --start-addr 0x0C00 -o $@ $^ apple2enh-iobuf-0800.o apple2enh.lib ../vendor/ip65/ip65_tcp.lib ../vendor/ip65/ip65_apple2.lib

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) --listing $<.lst --add-source -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *~ *.bin *.system
