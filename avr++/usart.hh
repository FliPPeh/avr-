#ifndef AVRXX_USART_HH
#define AVRXX_USART_HH

#include "util.hh"

extern "C" {
	#include <string.h>

	#include <avr/interrupt.h>
	#include <avr/io.h>
}

#if !defined(AVR_USART_TX_BUFSIZ)
	#define AVR_USART_TX_BUFSIZ 128
#endif
#if !defined(AVR_USART_RX_BUFSIZ)
	#define AVR_USART_RX_BUFSIZ 128
#endif


namespace avr {
namespace detail {

template <typename T> using volatile_t = T volatile;
template <typename T> using identity_t = T;

template <
		typename T,
		size_t S,
		template <typename> typename IRead = identity_t,
		template <typename> typename IWrite = IRead>

	struct ringbuffer {
		using element_type = T;
		using read_index_type = IRead<unsigned>;
		using write_index_type = IWrite<unsigned>;

		T buffer[S];
		read_index_type read_pos;
		write_index_type write_pos;

		inline void put(element_type elem) noexcept
		{
			buffer[write_pos] = elem;
			write_pos = (write_pos + 1) % S;
		}

		inline bool full() const noexcept
		{
			return ((write_pos + 1) % S) == read_pos;
		}

		inline bool empty() const noexcept
		{
			return write_pos == read_pos;
		}

		inline element_type peek() const noexcept
		{
			return buffer[read_pos];
		}

		inline element_type get() noexcept
		{
			element_type ret = buffer[read_pos];
			read_pos = (read_pos + 1) % S;

			return ret;
		}
	};
}

inline constexpr unsigned baudrate(unsigned long baud = 9600) noexcept
{
	return F_CPU / 16.f / baud - 1;
}

constexpr unsigned USART_TX_BUFSIZ = AVR_USART_TX_BUFSIZ;
constexpr unsigned USART_RX_BUFSIZ = AVR_USART_RX_BUFSIZ;

template <unsigned N>
	struct usart {
		using data_type = unsigned char;

		using tx_buffer_type =
			detail::ringbuffer<
				data_type, USART_TX_BUFSIZ, detail::volatile_t, detail::identity_t>;

		using rx_buffer_type =
			detail::ringbuffer<
				data_type, USART_RX_BUFSIZ, detail::identity_t, detail::volatile_t>;

		static tx_buffer_type tx;
		static rx_buffer_type rx;

		template <unsigned long Baud>
		inline static constexpr void init() noexcept;

		inline static data_type peek() noexcept
		{
			data_type ret{};

			if (!rx.empty()) {
				ret = rx.peek();
			}

			return ret;
		}

		inline static data_type get() noexcept
		{
			data_type ret{};

			if (!rx.empty()) {
				ret = rx.get();
			}

			return ret;
		}

		inline static void send(data_type byte) noexcept
		{
			send(&byte, 1);
		}

		inline static void send(int num, avr::to_string_opts opt = {}) noexcept
		{
			char numbuf[32] = {0};

			avr::to_string(num, numbuf, sizeof(numbuf), opt);
			send(numbuf);
		}

		inline static void send(char const* data) noexcept
		{
			send(reinterpret_cast<data_type const*>(data), strlen(data));
		}

		template <unsigned long Nd>
			inline static void send(uint8_t const (&data)[Nd]) noexcept
			{
				send(data, Nd);
			}

		template <unsigned long Nd>
			inline static void send(char const (&data)[Nd]) noexcept
			{
				send(reinterpret_cast<data_type const*>(&data[0]), Nd);
			}

		inline static void clear();

		inline static void send(data_type const* data, unsigned long len)
		{
			while (len--) {
				while (tx.full()) {

				}

				tx.put(*data++);
			}

			clear();
		}
	};

template <unsigned N> typename usart<N>::tx_buffer_type usart<N>::tx{{}, 0, 0};
template <unsigned N> typename usart<N>::rx_buffer_type usart<N>::rx{{}, 0, 0};

// Implementation specific for USART0
#ifdef UCSR0A
template <>
template <unsigned long Baud>
	inline constexpr void usart<0>::init() noexcept
	{
		unsigned constexpr rate = baudrate(Baud);
		
		UBRR0H = static_cast<uint8_t>(rate >> 8);
		UBRR0L = static_cast<uint8_t>(rate);

		UCSR0B |=
			  (1 << RXEN0)
			| (1 << TXEN0)
			| (1 << RXCIE0)
			| (1 << TXCIE0);

		UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	}

template <>
	inline void usart<0>::clear() noexcept
	{
		if (UCSR0A & (1 << UDRE0)) {
			UDR0 = 0;
		}
	}
#endif

ISR(USART_TX_vect)
{
#ifdef UCSR0A
	if (UCSR0A & (1 << UDRE0)) {
		if (!avr::usart<0>::tx.empty()) {
			UDR0 = avr::usart<0>::tx.get();
		}
	}
#endif
}

ISR(USART_RX_vect)
{
#ifdef UCSR0A
	if (UCSR0A & (1 << RXC0)) {
		avr::usart<0>::rx.put(UDR0);
	}
#endif
}
}

#endif
