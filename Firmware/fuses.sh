#avrdude -p m48p -c stk500v2 -P /dev/ttyACM0 

avrdude -p m328pb -c stk500v2 -P /dev/ttyACM1 -U lfuse:w:0xC2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m -F
