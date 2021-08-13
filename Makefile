_DEPSH := bitmaps.h config.h config_screen.h gfx.h net.h parser.h test.h types.h ui.h utils.h
_DEPSC := bitmaps.c config.c config_screen.c fetch.c gfx.c main.c main_config.c net.c net_mock.c parser.c ui.c utils.c video.c
_DEPST := test.c test-utils.c
_DEPSE := meteo.cfg w5368335.json w6077246.json w6454573.json

DEPSH = $(patsubst %,include/%,$(_DEPSH))
DEPSC = $(patsubst %,src/%,$(_DEPSC))
DEPSE = $(patsubst %,etc/%,$(_DEPSE))
DEPST = $(patsubst %,tests/%,$(_DEPST))

meteo.po: $(DEPSH) $(DEPSC) $(DEPSE)
	$(MAKE) -C src meteo.system meteom.system config.system configm.system
	applecommander -pro140 meteo.po meteo
	@#applecommander -as meteo.po meteom.system <src/meteom.system
	@#applecommander -as meteo.po configm.system <src/configm.system
	applecommander -as meteo.po meteo.system <src/meteo.system
	@#applecommander -as meteo.po config.system <src/config.system
	applecommander -p meteo.po meteo.cfg bin <etc/meteo.cfg
	applecommander -p meteo.po I01D.A2LR bin <a2lr/01d.a2lr
	applecommander -p meteo.po I01N.A2LR bin <a2lr/01n.a2lr
	applecommander -p meteo.po I02D.A2LR bin <a2lr/02d.a2lr
	applecommander -p meteo.po I02N.A2LR bin <a2lr/02n.a2lr
	applecommander -p meteo.po I03D.A2LR bin <a2lr/03d.a2lr
	applecommander -p meteo.po I03N.A2LR bin <a2lr/03n.a2lr
	applecommander -p meteo.po I04D.A2LR bin <a2lr/04d.a2lr
	applecommander -p meteo.po I04N.A2LR bin <a2lr/04n.a2lr
	applecommander -p meteo.po I09D.A2LR bin <a2lr/09d.a2lr
	applecommander -p meteo.po I09N.A2LR bin <a2lr/09n.a2lr
	applecommander -p meteo.po I10D.A2LR bin <a2lr/10d.a2lr
	applecommander -p meteo.po I10N.A2LR bin <a2lr/10n.a2lr
	applecommander -p meteo.po I11D.A2LR bin <a2lr/11d.a2lr
	applecommander -p meteo.po I11N.A2LR bin <a2lr/11n.a2lr
	applecommander -p meteo.po I13D.A2LR bin <a2lr/13d.a2lr
	applecommander -p meteo.po I13N.A2LR bin <a2lr/13n.a2lr
	applecommander -p meteo.po I404.A2LR bin <a2lr/404.a2lr
	@#applecommander -p meteo.po w5368335.json txt <etc/w5368335.json
	@#applecommander -p meteo.po w5847504.json txt <etc/w5847504.json
	@#applecommander -p meteo.po w6077246.json txt <etc/w6077246.json
	@#applecommander -p meteo.po w6454573.json txt <etc/w6454573.json
	@#applecommander -p meteo.po w7839805.json txt <etc/w7839805.json
	@#applecommander -as meteo.po fetch <src/fetch.bin

config:
	./make_config.sh

clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean
	rm -f meteo.po

test: $(DEPSH) $(DEPSC) $(DEPST)
	$(MAKE) -C src
	$(MAKE) -C tests

all: clean config meteo.po
	cp meteo.po "/Volumes/APPLE II"
