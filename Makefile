all:
	rm -f meteo.po
	cl65 -t apple2enh -C apple2enh.cfg main.c -o meteo.bin
	applecommander -pro140 meteo.po meteo
	applecommander -as meteo.po meteo <meteo.bin
