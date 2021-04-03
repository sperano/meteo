all:
	rm -f meteo.po
	cl65 -t apple2enh -C apple2enh.cfg main.c vendor/json65-master/src/json65.s vendor/json65-master/src/json65-file.c vendor/json65-master/src/json65-print.c vendor/json65-master/src/json65-quote.s vendor/json65-master/src/json65-string.s vendor/json65-master/src/json65-tree.c -o meteo.bin
	applecommander -pro140 meteo.po meteo
	applecommander -as meteo.po meteo <meteo.bin
	applecommander -p meteo.po weather.json txt <weather.json

clean:
	rm -fv vendor/json65-master/src/*.o
