CXX=avr-g++
OBJCOPY=avr-objcopy

CXXFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p -g -std=c++14 -Wall -Wextra -fno-exceptions #-lm
PORT=/dev/ttyACM0

blink.hex: blink.elf
	avr-objcopy -j .text -j .data -O ihex blink.elf blink.hex 

blink.elf: blink.cc avr++/usart.hh avr++/io.hh avr++/adc.hh avr++/util.hh
	${CXX} ${CXXFLAGS} -o blink.elf blink.cc

install: blink.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P ${PORT} -b 115200 -U flash:w:blink.hex
