#33972
#31303
VERSION := 1.0
CC := cl65
LD := ld65
CFLAGS := -DMETEO_VERSION=\"$(VERSION)\" -DWITH_CONFIG -O -t apple2enh -C apple2enh-system.cfg -I../include -I../vendor/json65-master/src/ -I../vendor/ip65

# 8192 is the default starting
LDFLAGS := -C apple2enh-system.cfg
_DEPS := bitmaps.h config.h config_screen.h gfx.h net.h parser.h types.h ui.h utils.h
DEPS = $(patsubst %,../include/%,$(_DEPS))

OBJ = config.o config_screen.o gfx.o json65.o main.o net.o parser.o ui.o utils.o video.o
OBJ_M = config.o config_screen.o gfx.o json65.o main.o net_mock.o parser.o ui.o utils.o video.o
OBJS_FETCH = fetch.o config.o utils.o
OBJS_CONFIG = config.o config_screen.o json65.o main_config.o net.o parser.o ui.o utils.o video.o
OBJS_CONFIG_M = config.o config_screen.o json65.o main_config.o net_mock.o parser.o ui.o utils.o video.o

COMMON_LIBS := apple2enh.lib
LIBS := $(COMMON_LIBS) ../vendor/ip65/ip65_tcp.lib ../vendor/ip65/ip65_apple2.lib

meteo.system: $(OBJ)
	@#$(LD) $(LDFLAGS) -o $@ $^ apple2-iobuf-0800.o $(LIBS)
	$(LD) $(LDFLAGS) -m $@.map -vm -o $@ $^ $(LIBS)

meteom.system: $(OBJ_M)
	@#$(LD) $(LDFLAGS) -o $@ $^ apple2-iobuf-0800.o $(LIBS)
	$(LD) $(LDFLAGS) -m $@.map -vm -o $@ $^ $(COMMON_LIBS)

config.system: $(OBJS_CONFIG)
	$(LD) $(LDFLAGS) -m $@.map -vm -o $@ $^ $(LIBS)

configm.system: $(OBJS_CONFIG_M)
	$(LD) $(LDFLAGS) -m $@.map -vm -o $@ $^ $(COMMON_LIBS)

fetch.bin: $(OBJS_FETCH)
	$(LD) $(LDFLAGS) --start-addr 0x0C00 -o $@ $^ apple2-iobuf-0800.o $(LIBS)

all: meteo.system meteom.system config.system fetch.bin

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) --listing $<.lst --add-source -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *~ *.bin *.system *.map
