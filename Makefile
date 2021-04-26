_DEPSH := bitmaps.h config.h gfx.h parser.h test.h types.h utils.h
_DEPSC := bitmaps.c config.c gfx.c main.c parser.c utils.c fetch.c
_DEPST := test.c test-utils.c
_DEPSE := meteo.cfg w5368335.json w6077246.json w6454573.json

DEPSH = $(patsubst %,include/%,$(_DEPSH))
DEPSC = $(patsubst %,src/%,$(_DEPSC))
DEPSE = $(patsubst %,etc/%,$(_DEPSE))
DEPST = $(patsubst %,tests/%,$(_DEPST))

meteo.po: $(DEPSH) $(DEPSC) $(DEPSE)
	$(MAKE) -C src
	$(MAKE) -C src fetch.bin
	applecommander -pro140 meteo.po meteo
	@#applecommander -p meteo.po meteo.cfg txt <etc/meteo.cfg
	applecommander -p meteo.po meteo.cfg bin <etc/meteo.cfg2
	applecommander -p meteo.po w5368335.json txt <etc/w5368335.json
	applecommander -p meteo.po w6077246.json txt <etc/w6077246.json
	applecommander -p meteo.po w6454573.json txt <etc/w6454573.json
	applecommander -as meteo.po meteo <src/meteo.bin
	applecommander -as meteo.po fetch <src/fetch.bin

config:
	./make_config.sh

clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
	rm -f meteo.po

test: $(DEPSH) $(DEPSC) $(DEPST)
	$(MAKE) -C src
	$(MAKE) -C tests
