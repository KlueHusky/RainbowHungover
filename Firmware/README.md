# AVR PWM software

## dnf install list

- ``avrdude``
- ``gcc-avr32-linux-gnu``
- ``binutils-avr32-linux-gnu``
- ``avr-libc``
- ``avr-binutils``
- ``avr-gcc``

C++ :

- ``avr-gcc-c++`` (compiler)
- ``avrdude`` (flash)
- ``avr-libc`` (libraries)

Compiler : ``avr32-linux-gnu-gcc`` or ``avr32-linux-gnu-gcc``

## Compilation

``avr-c++ -mmcu=attiny2313 -O2 main.cpp -o main.elf``

## Flash

``avrdude -p m48p -c usbasp -U flash:w:main.elf``

## Fuses

``avrdude -p m48p -c usbasp -U lfuse:w:0xc2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m``

## Notes

Libraries in ``/usr/avr/include``