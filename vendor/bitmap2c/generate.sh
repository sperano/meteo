#!/bin/sh
set -euo pipefail

go build

cat ../../bitmaps/01d.bmp | ./bitmap2c e6e6dfdf0000ffff=13 >../../bitmaps/01d.c
cat ../../bitmaps/01n.bmp | ./bitmap2c 0000fdfdffffffff=7 >../../bitmaps/01n.c
cat ../../bitmaps/02n.bmp | ./bitmap2c 04043333ffffffff=2 0000fdfdffffffff=7 >../../bitmaps/02n.c
cat ../../bitmaps/404.bmp | ./bitmap2c ffff26260000ffff=9 >../../bitmaps/404.c
