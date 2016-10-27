#ifndef AVRXX_IO_HH
#define AVRXX_IO_HH

#include "util.hh"

extern "C" {
	#include <avr/io.h>
}

namespace avr {
enum class pin_mode : bool {
	input = 0,
	output = 1
};

enum class portid : char {
#if defined(PORTB)
	PORT_B,
#endif
#if defined(PORTC)
	PORT_C,
#endif
#if defined(PORTD)
	PORT_D
#endif
};

template <enum portid Port>
	struct port {
		inline static constexpr uint8_t volatile& port_reg();
		inline static constexpr uint8_t volatile& ddr_reg();
		inline static constexpr uint8_t volatile& pin_reg();
	};

#if defined(PORTB)
template <>
inline constexpr uint8_t volatile& port<portid::PORT_B>::port_reg() { return PORTB; }
template <>
inline constexpr uint8_t volatile& port<portid::PORT_B>::ddr_reg() { return DDRB; }
template <>
inline constexpr uint8_t volatile& port<portid::PORT_B>::pin_reg() { return PINB; }
#endif

#if defined(PORTC)
template <>
inline constexpr uint8_t volatile& port<portid::PORT_C>::port_reg() { return PORTC; }
template <>
inline constexpr uint8_t volatile& port<portid::PORT_C>::ddr_reg() { return DDRC; }
template <>
inline constexpr uint8_t volatile& port<portid::PORT_C>::pin_reg() { return PINC; }
#endif

#if defined(PORTD)
template <>
inline constexpr uint8_t volatile& port<portid::PORT_D>::port_reg() { return PORTD; }
template <>
inline constexpr uint8_t volatile& port<portid::PORT_D>::ddr_reg() { return DDRD; }
template <>
inline constexpr uint8_t volatile& port<portid::PORT_D>::pin_reg() { return PIND; }
#endif


template <enum portid Port, unsigned Mask, bool Adc = false, unsigned AdcN = 0>
	struct pin {
		using pport = port<Port>;
		using is_in_out = void;

		inline static constexpr void init() noexcept
		{
		}

		inline static constexpr void set_mode(pin_mode m) noexcept
		{
			if (m == pin_mode::output) {
				pport::ddr_reg() |= Mask;
			} else {
				pport::ddr_reg() &= ~Mask;
			}
		}

		inline static constexpr void set(bool s) noexcept
		{
			if (s) {
				pport::port_reg() |= Mask;
			} else {
				pport::port_reg() &= ~Mask;
			}
		}

		inline static constexpr bool get() noexcept
		{
			return bool{pport::pin_reg() & Mask};
		}

		template <bool B = Adc, typename = typename enable_if<B>::type>
		inline static unsigned get_analog() noexcept
		{
			ADMUX |= (AdcN & 0x0f);
			ADCSRA |= (1 << ADSC);

			while ((ADCSRA & (1 << ADSC))) {
			}

			return ADC;
		}
	};

template <typename Pin>
	struct output_pin : Pin {
		using is_out = void;

		inline static constexpr void init() noexcept
		{
			Pin::set_mode(pin_mode::output);
		}

		using Pin::set;
	};

template <typename Pin>
	struct input_pin : Pin {
		using is_in = void;

		inline static constexpr void init() noexcept
		{
			Pin::set_mode(pin_mode::input);
		}

		inline static constexpr void set_pullup(bool s) noexcept
		{
			Pin::set(s);
		}

		using Pin::get;
		using Pin::get_analog;
	};

template <typename Pin, typename _ = void>
	struct inverted;

template <typename Pin>
	struct inverted<Pin, typename Pin::is_out> : Pin {
		using is_out = void;

		inline static constexpr void set(bool s) noexcept
		{
			Pin::set(!s);
		}
	};

template <typename Pin>
	struct inverted<Pin, typename Pin::is_in> : Pin {
		using is_in = void;

		inline static constexpr bool get() noexcept
		{
			return !Pin::get();
		}
	};

#if defined(PORTB)
	using pb0 = pin<portid::PORT_B, 1 << 0>;
	using pb1 = pin<portid::PORT_B, 1 << 1>;
	using pb2 = pin<portid::PORT_B, 1 << 2>;
	using pb3 = pin<portid::PORT_B, 1 << 3>;
	using pb4 = pin<portid::PORT_B, 1 << 4>;
	using pb5 = pin<portid::PORT_B, 1 << 5>;
	using pb6 = pin<portid::PORT_B, 1 << 6>;
	using pb7 = pin<portid::PORT_B, 1 << 7>;
#endif
#if defined(PORTC)
	using pc0 = pin<portid::PORT_C, 1 << 0, true, 0x00>; // ADC0
	using pc1 = pin<portid::PORT_C, 1 << 1, true, 0x01>; // ADC1
	using pc2 = pin<portid::PORT_C, 1 << 2, true, 0x02>; // ADC2
	using pc3 = pin<portid::PORT_C, 1 << 3, true, 0x03>; // ADC3
	using pc4 = pin<portid::PORT_C, 1 << 4, true, 0x04>; // ADC4
	using pc5 = pin<portid::PORT_C, 1 << 5, true, 0x05>; // ADC5
	using pc6 = pin<portid::PORT_C, 1 << 6, true, 0x06>; // ADC6
	using pc7 = pin<portid::PORT_C, 1 << 7, true, 0x07>; // ADC7
#endif
#if defined(PORTD)
	using pd0 = pin<portid::PORT_D, 1 << 0>;
	using pd1 = pin<portid::PORT_D, 1 << 1>;
	using pd2 = pin<portid::PORT_D, 1 << 2>;
	using pd3 = pin<portid::PORT_D, 1 << 3>;
	using pd4 = pin<portid::PORT_D, 1 << 4>;
	using pd5 = pin<portid::PORT_D, 1 << 5>;
	using pd6 = pin<portid::PORT_D, 1 << 6>;
	using pd7 = pin<portid::PORT_D, 1 << 7>;
#endif
}

namespace arduino {
	// Digital pins 0 - 7 (PORTD)
	using dp0 = avr::pd0;
	using dp1 = avr::pd1;
	using dp2 = avr::pd2;
	using dp3 = avr::pd3;
	using dp4 = avr::pd4;
	using dp5 = avr::pd5;
	using dp6 = avr::pd6;
	using dp7 = avr::pd7;

	// Digital pins 8 - 13 (PORTB)
	using dp8 = avr::pb0;
	using dp9 = avr::pb1;
	using dp10 = avr::pb2;
	using dp11 = avr::pb3;
	using dp12 = avr::pb4;
	using dp13 = avr::pb5;

	// Analog-in pins 0 - 5 (PORTC, ADC mapped)
	using ai0 = avr::pc0;
	using ai1 = avr::pc1;
	using ai2 = avr::pc2;
	using ai3 = avr::pc3;
	using ai4 = avr::pc4;
	using ai5 = avr::pc5;
}

#endif
