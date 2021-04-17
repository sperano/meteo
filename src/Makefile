IDIR := ../include
CC := cl65
LD := ld65
CFLAGS := -Cl -O -t apple2enh -C apple2enh.cfg -I../include -I../vendor/json65-master/src/
LDFLAGS := -t apple2enh
_DEPS := config.h meteo.h parser.h utils.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

OBJ = bitmaps.o config.o main.o parser.o utils.o json65.o json65-file.o json65-string.o json65-tree.o

meteo.bin: $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^ apple2enh.lib

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *~ meteo.bin
