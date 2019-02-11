clock_speeds=( 16000000L 8000000L 1000000L )
power_levels=( RF24_PA_MIN RF24_PA_LOW RF24_PA_HIGH RF24_PA_MAX)

for f in "${clock_speeds[@]}"
 do
 for i in {1..110}
  do
  for p in "${power_levels}"
    do

   sed -e "s/\${channel}/$i/" -e "s/\${power}/$p/" MYSBootloader.c.original > MYSBootloader.c
   
   if [ $f == "16000000L" ]
   then
    baudios="115200"
   fi

   if [ $f == "8000000L" ]
   then
    baudios="57600"
   fi

   if [ $f == "1000000L" ]
   then
    baudios="9600"
   fi
      echo "Frecuency: $f Channel: $i Baud rate: $baudios"
      sed -e "s/\${frecuency}/$f/" -e "s/\${baudios}/$baudios/" Makefile.original > Makefile
      make
      mkdir compiled
      mv MYSBootloader.hex ./compiled/MYSBootloader.ch$i.$f.$p.hex
    done
  done
done
