#!/bin/sh

URL=https://api.openweathermap.org/data/2.5/weather?appid=f3b3e0e41592b6706cc8b6040a335f45

curl -o etc/w5368335.json "${URL}&id=5368335"
curl -o etc/w5847504.json "${URL}&id=5847504"
curl -o etc/w6077246.json "${URL}&id=6077246"
curl -o etc/w6454573.json "${URL}&id=6454573"
curl -o etc/w7839805.json "${URL}&id=7839805"
curl -o etc/w5847504.json "${URL}&id=5847504"
