
default:
	# compile for attiny2313a with warnings, optimizations, and 1 MHz clock frequency
	avr-gcc -Wall -Os -mmcu=attiny2313 -o main.o main.c
	avr-objcopy -j .text -j .data -O ihex main.o main.hex
	
	# Cleanup
	rm main.o
	

flash:
	avrdude -c avrispmkII -p t2313 -P usb -U flash:w:main.hex
