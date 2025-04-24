# avr-g++ -c -g -Os -w -mmcu=atmega328p main.cpp -o main.o -Wall -Wextra
# avr-strip --strip-debug main.o --strip-unneeded
# avr-g++ -g -Os -w -mmcu=atmega328p -Wl,--gc-sections main.o -o main.elf -Wall -Wextra

avr-g++ -c -Os -w -mmcu=atmega328p main.cpp -o main.o -Wall -Wextra
#avr-g++ -c -Os -w -mmcu=atmega328p modes.cpp -o modes.o -Wall -Wextra

#avr-g++ -g -Os -w -mmcu=atmega328p -Wl,--gc-sections main.o modes.o -o main.elf -Wall -Wextra
avr-g++ -g -Os -w -mmcu=atmega328p -Wl,--gc-sections main.o -o main.elf -Wall -Wextra