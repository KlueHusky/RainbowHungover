./compile.sh

avrdude -p m328pb -c stk500v2 -P /dev/ttyACM1 -U flash:w:main.elf

