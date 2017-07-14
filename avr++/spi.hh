#ifndef AVRXX_SPI_HH
#define AVRXX_SPI_HH

#include "io.hh"

extern "C" {
	#include <avr/interrupt.h>
	#include <avr/io.h>
}

namespace avr {
namespace spi {

// Default pins (ATMega328p - might need adaption for other chips)
using ss = avr::pb2;
using mosi = avr::pb3;
using miso = avr::pb4;
using sck = avr::pb5;

enum class data_order {
	msb_first,
	lsb_first
};

enum class mode {
	master,
	slave
};

enum class data_mode {
	mode0, // CPOL 0, CPHA 0
	mode1, // CPOL 0, CPHA 1
	mode2, // CPOL 1, CPHA 0
	mode3  // CPOL 1, CPHA 1
};

enum class speed {
	div2,   // SPI2X 1, SPR1 0, SPR0 0
	div4,   // SPI2X 0, SPR1 0, SPR0 0
	div8,   // SPI2X 1, SPR1 0, SPR0 0
	div16,  // SPI2X 0, SPR1 0, SPR0 1
	div32,  // SPI2X 1, SPR1 1, SPR0 0
	div64,  // SPI2X 0, SPR1 1, SPR0 0
	div128, // SPI2X 0, SPR1 1, SPR0 1
};

inline static void init(
	mode mode = mode::master,
	speed speed = speed::div8,
	data_mode data_mode = data_mode::mode0,
	bool interrupt = false)
{
	if (mode == mode::master) {
		ss::set(true);
		avr::output_pin<ss>::init();

		SPCR = (1 << SPE) // Enable SPI
			|  (1 << MSTR) // Master Mode
			|  (interrupt << SPIE);

		switch (speed) {
		case speed::div2:   SPCR |= ((1 << SPI2X) | (0 << SPR1) | (0 << SPR0)); break;
		case speed::div4:   SPCR |= ((0 << SPI2X) | (0 << SPR1) | (0 << SPR0)); break;
		case speed::div8:   SPCR |= ((1 << SPI2X) | (0 << SPR1) | (0 << SPR0)); break;
		case speed::div16:  SPCR |= ((0 << SPI2X) | (0 << SPR1) | (1 << SPR0)); break;
		case speed::div32:  SPCR |= ((1 << SPI2X) | (1 << SPR1) | (0 << SPR0)); break;
		case speed::div64:  SPCR |= ((0 << SPI2X) | (1 << SPR1) | (0 << SPR0)); break;
		case speed::div128: SPCR |= ((0 << SPI2X) | (1 << SPR1) | (1 << SPR0)); break;
		}

		switch (data_mode) {
		case data_mode::mode0: SPCR |= ((0 << CPOL) | (0 << CPHA)); break;
		case data_mode::mode1: SPCR |= ((0 << CPOL) | (1 << CPHA)); break;
		case data_mode::mode2: SPCR |= ((1 << CPOL) | (0 << CPHA)); break;
		case data_mode::mode3: SPCR |= ((1 << CPOL) | (1 << CPHA)); break;
		}

		avr::output_pin<sck>::init();
		avr::output_pin<mosi>::init();
	} else {
		avr::output_pin<miso>::init();

		SPCR = (1 << SPE);
	}
}

inline static uint8_t transmit(uint8_t data, data_order data_order = data_order::msb_first)
{
	if (data_order == data_order::msb_first) {
		SPCR |= (0 << DORD);
	} else {
		SPCR |= (1 << DORD);
	}

	SPDR = data;

	while (!(SPSR & (1 << SPIF))) {

	}

	return SPDR;
}

}
}

#endif
