clock_speeds=( 16000000L 8000000L 1000000L )
power_levels=( RF24_PA_MIN RF24_PA_LOW RF24_PA_HIGH RF24_PA_MAX )

mkdir compiled

compiler=compiled/compiler-info.txt
echo -e "avr-gcc version:\n================" > ${compiler}
# avr-gcc seems to dump its specfications on stderr, so we have to redirect stderr
avr-gcc -### 2>> ${compiler}
echo -e "\navr-ld version:\n===============" >> ${compiler}
avr-ld --version >> ${compiler}
echo -e "\navr-objcopy version:\n====================" >> ${compiler}
avr-objcopy -V >> ${compiler}
echo -e "\navr-size version:\n=================" >> ${compiler}
avr-size --version >> ${compiler}
echo -e "\nCFLAGS:\n=======" >> ${compiler}
grep "^CFLAGS" >> ${compiler} Makefile
echo -e "\nLDFLAGS:\n========" >> ${compiler}
grep "^LDFLAGS" >> ${compiler} Makefile


for f in "${clock_speeds[@]}"
 do
 for i in {1..110}
  do
  for p in "${power_levels[@]}"
    do
     sed -e "s/\${channel}/$i/" -e "s/\${power}/$p/" MYSBootloader.c.original > MYSBootloader.c
   
      if [ $f == "16000000L" ]
      then
       baudrate="115200"
      fi
  
      if [ $f == "8000000L" ] 
      then
       baudrate="57600"
      fi 
  
      if [ $f == "1000000L" ] 
      then
       baudrate="9600"
      fi
     
      echo "Frequency: $f Channel: $i Baud rate: $baudrate Power level: $p"
      sed -e "s/\${frequency}/$f/" -e "s/\${baudrate}/$baudrate/" Makefile.original > Makefile
      make
      if [ `avr-size -B -d MYSBootloader.elf  | tail -n 1 | awk '{print $1+$2}'` -le 2048 ]
      then 
	cp MYSBootloader.hex ./compiled/MYSBootloader.ch$i.$f.$p.hex
      else
      	echo "Compiled boot loader is too large to fit in bootloader area!!"
      	rm --force ./compiled/MYSBootloader.ch$i.$f.$p.hex
      fi
      
    done
  done
done
