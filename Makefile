all:
	rm -f meteo.po
	cl65 -t apple2enh -C apple2enh.cfg -o meteo.bin \
		main.c config.c meteo.c bitmaps.c \
		vendor/json65-master/src/json65.s \
		vendor/json65-master/src/json65-file.c \
		vendor/json65-master/src/json65-string.s \
		vendor/json65-master/src/json65-tree.c

	applecommander -pro140 meteo.po meteo
	applecommander -as meteo.po meteo <meteo.bin
	applecommander -p meteo.po meteo.cfg txt <meteo.cfg
	applecommander -p meteo.po w5368335.json txt <w5368335.json
	applecommander -p meteo.po w6077246.json txt <w6077246.json
	applecommander -p meteo.po w6454573.json txt <w6454573.json

clean:
	rm -fv vendor/json65-master/src/*.o
