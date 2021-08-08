#!/bin/sh
set -euo pipefail

go build

C1=ffff26260000ffff
C2=04043333ffffffff
C5=797979797979ffff
C6=4b4ba5a5fbfbffff
C7=0000fdfdffffffff
C9=ffff26260000ffff
C9B=ffff93930000ffff
C10=c0c0c0c0c0c0ffff
C13=e6e6dfdf0000ffff

COLORS="${C1}=1 ${C2}=2 ${C5}=5 ${C6}=6 ${C7}=7 ${C9}=9 ${C9B}=9 ${C10}=10 ${C13}=13"
echo "01d"
cat ../../bitmaps/01d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/01d.a2lr
echo "01n"
cat ../../bitmaps/01n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/01n.a2lr
echo "02d"
cat ../../bitmaps/02d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/02d.a2lr
echo "02n"
cat ../../bitmaps/02n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/02n.a2lr
echo "03d"
cat ../../bitmaps/03d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/03d.a2lr
echo "03n"
cat ../../bitmaps/03n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/03n.a2lr
echo "04d"
cat ../../bitmaps/04d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/04d.a2lr
echo "04n"
cat ../../bitmaps/04n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/04n.a2lr
echo "09d"
cat ../../bitmaps/09d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/09d.a2lr
echo "09n"
cat ../../bitmaps/09n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/09n.a2lr
echo "10d"
cat ../../bitmaps/10d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/10d.a2lr
echo "10n"
cat ../../bitmaps/10n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/10n.a2lr
echo "11d"
cat ../../bitmaps/11d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/11d.a2lr
echo "11n"
cat ../../bitmaps/11n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/11n.a2lr
echo "13d"
cat ../../bitmaps/13d.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/13d.a2lr
echo "13n"
cat ../../bitmaps/13n.bmp | ./bitmap2a2lr ${COLORS} >../../a2lr/13n.a2lr
echo "404"
cat ../../bitmaps/404.bmp | ./bitmap2a2lr 0000f9f90000ffff=14 ffff4040ffffffff=3 949421219292ffff=3 >../../a2lr/404.a2lr
