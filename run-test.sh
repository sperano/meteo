sim65 $1
if [ $? == 0 ]; then
   printf "%-30s pass\n" $1
   exit 0
fi
printf "%-30s fail\n" $1
exit 1
