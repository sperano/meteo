TEST_DIR := tests

all:
	rm -f meteo.po
	cl65 -t apple2enh -C apple2enh.cfg -o meteo.bin \
		-I vendor/json65-master/src/ -I src \
		src/main.c src/config.c src/meteo.c src/bitmaps.c \
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

test-parser:
	@cl65 -t sim6502 -o $(TEST_DIR)/test-parser \
		-I vendor/json65-master/src/ -I src \
		$(TEST_DIR)/test-parser.c
	@./run-test.sh $(TEST_DIR)/test-parser
test: test-parser

clean:
	rm -fv vendor/json65-master/src/*.o src/*.o
