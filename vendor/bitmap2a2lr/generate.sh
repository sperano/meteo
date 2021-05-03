#!/bin/sh
set -euo pipefail

go build

cat ../../bitmaps/01d.bmp | ./bitmap2a2lr e6e6dfdf0000ffff=13 >../../a2lr/01d.a2lr
cat ../../bitmaps/01n.bmp | ./bitmap2a2lr 0000fdfdffffffff=7 >../../a2lr/01n.a2lr
cat ../../bitmaps/02d.bmp | ./bitmap2a2lr e6e6dfdf0000ffff=13 c0c0c0c0c0c0ffff=10 >../../a2lr/02d.a2lr
cat ../../bitmaps/02n.bmp | ./bitmap2a2lr 04043333ffffffff=2 0000fdfdffffffff=7 >../../a2lr/02n.a2lr
cat ../../bitmaps/04d.bmp | ./bitmap2a2lr c0c0c0c0c0c0ffff=10 797979797979ffff=5 >../../a2lr/04d.a2lr
cat ../../bitmaps/404.bmp | ./bitmap2a2lr ffff26260000ffff=9 >../../a2lr/404.a2lr
