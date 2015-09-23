
default:
	# compile for attiny2313a with warnings, optimizations, and 1 MHz clock frequency
	avr-gcc -Wall -Os -mmcu=attiny2313 -o blink.o blink.c
	avr-objcopy -j .text -j .data -O ihex blink.o blink.hex
	
	# Cleanup
	rm blink.o
	

flash:
	avrdude -c avrispmkII -p t2313 -P usb -U flash:w:blink.hex
