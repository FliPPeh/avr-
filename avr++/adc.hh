#ifndef AVRXX_ADC_HH
#define AVRXX_ADC_HH

extern "C" {
	#include <avr/io.h>
}

namespace avr {
namespace adc {
namespace {
constexpr uint8_t calculate_prescaler() noexcept
{
	constexpr unsigned long raw = F_CPU / 125000; // Aim for 125kHz

	if (raw > 128) {
		return 128;
	} else if (raw < 2) {
		return 2;
	} else {
		return raw;
	}
}
}

enum class reference_selection {
	aref = 0,
	avcc = 1,
	internal = 3
};

inline static void set_reference(reference_selection ref) noexcept
{
	ADMUX &= 0x0f;

	switch (ref) {
	case reference_selection::aref:
		// Default
		break;

	case reference_selection::avcc:
		ADMUX |= (1 << REFS0);
		break;

	case reference_selection::internal:
		ADMUX |= (1 << REFS1) | (1 << REFS0);
		break;
	}
}

inline static void init(reference_selection ref = reference_selection::aref) noexcept
{
	constexpr uint8_t pre = calculate_prescaler();

	ADCSRA |= (1 << ADEN)
		| ((pre >= 128 ? 1 : 0) << ADPS2)
		| ((pre >= 64 ? 1 : 0) << ADPS1)
		| ((pre >= 2 ? 1 : 0) << ADPS0);

	set_reference(ref);
}

}
}

#endif
