all:
	avr-gcc z80.c io.c -mmcu=atmega328p -o z80.elf -Os
	avr-objcopy -O ihex z80.elf z80.hex
	avrdude -p m328p -c dapa -U flash:w:z80.hex
