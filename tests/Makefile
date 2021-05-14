CC := cl65
LD := ld65
CFLAGS := -DNOCONSOLE -DMETEO_VERSION=\"test-$(VERSION)\" -Cl -t sim6502 -I../include -I../vendor/json65-master/src/ -I../vendor/ip65
LDFLAGS := -t sim6502
_DEPS := bitmaps.h config.h gfx.h parser.h test.h types.h utils.h
DEPS = $(patsubst %,../include/%,$(_DEPS))
OBJ = config.o test-config.o gfx.o test.o test-utils.o utils.o

test: $(OBJ) $(DEPSO)
	$(LD) $(LDFLAGS) -o $@ $^ sim6502.lib
	sim65 ./test

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o *~ test
