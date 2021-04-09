#!/bin/sh
set -euo pipefail

go build

cat ../bitmaps/02n.bmp | ./bitmap2c  e6e6dfdf0000ffff=13 >../bitmaps/02n.c
