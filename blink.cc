#define AVR_USART_TX_BUFSIZ 256
#define AVR_USART_RX_BUFSIZ 128

#include "avr++/io.hh"
#include "avr++/adc.hh"
#include "avr++/usart.hh"
#include "avr++/util.hh"

extern "C" {
	#include <util/delay.h>
}


using led = avr::output_pin<arduino::dp13>;
using temp = avr::input_pin<arduino::ai0>;
using usart = avr::usart<0>;

float read_adc()
{
	float const R = 9850 * ((1024.0f / temp::get_analog()) - 1);
	float const log_temp = log(R);

	return 1 /
			(  0.001129148
			+ (0.000234125 * log_temp)
			+ (0.0000000876741 * log_temp * log_temp * log_temp)) - 273.15;
}


int main(void)
{
	sei();

	// Turn on ADC
	avr::adc::init(avr::adc::reference_selection::avcc);

	usart::init<9600>();
	usart::send("C++: Online m8\r\n");

	temp::init();

	led::init();
	led::set(false);

	bool state = true;
	char temp_buffer[8] = {0};

	for (;;) {
loop:
		switch (char c = usart::get()) {
		case 't':
			state = !state;
			break;

		case '1':
			state = true;
			break;

		case '0':
			state = false;
			break;

		case 'T':
			avr::to_string(read_adc(), temp_buffer, { 10, 3, false, true, ',' });

			usart::send("Temperatur: ");
			usart::send(temp_buffer);
			usart::send("C\r\n");
			goto loop;

		default:
			usart::send(c);
			usart::send(": What?\r\n");
		case '\0': 
			goto loop;
		}

		if (state) {
			led::set(true);
			usart::send("LED Status: on\r\n");
		} else {
			led::set(false);
			usart::send("LED Status: off\r\n");
		}
	}
}
