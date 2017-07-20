#ifndef AVRXX_UTIL_H
#define AVRXX_UTIL_H

extern "C" {
	#include <math.h>
	#include <string.h>
	#include <ctype.h>
}

namespace avr {
template <bool B, typename T = void>
	struct enable_if {
	};
 
template <typename T>
	struct enable_if<true, T> {
		using type = T;
	};

namespace detail {
template <typename T>
	struct is_integral { };

template <> struct is_integral<char> { constexpr static bool value = true; };
template <> struct is_integral<unsigned char> { constexpr static bool value = true; };
template <> struct is_integral<int> { constexpr static bool value = true; };
template <> struct is_integral<unsigned int> { constexpr static bool value = true; };
template <> struct is_integral<short> { constexpr static bool value = true; };
template <> struct is_integral<unsigned short> { constexpr static bool value = true; };
template <> struct is_integral<long> { constexpr static bool value = true; };
template <> struct is_integral<unsigned long> { constexpr static bool value = true; };
template <> struct is_integral<long long> { constexpr static bool value = true; };
template <> struct is_integral<unsigned long long> { constexpr static bool value = true; };
}

constexpr char const* glyphs = "0123456789abcdefghijklmnopqrstuvwxyz";

inline char const* reverse_string(char* str)
{
	auto const len = strlen(str);

	for (unsigned i = 0; i < len; ++i) {
		char const t = str[i];

		if (i >= len/2) {
			break;
		}

		str[i] = str[len - 1 - i];
		str[len - 1 - i] = t;
	}

	return str;
}

struct to_string_opts {
	unsigned radix = 10;
	unsigned precision = 3;

	bool upper = false;
	bool always_prefix = false;
	char decimal = '.';
};

template <typename T,
		typename = typename enable_if<detail::is_integral<T>::value>::type>
	inline int to_string(T n, char* buffer, unsigned buflen, to_string_opts opt = {})
	{
		bool sign = n < 0;

		if (sign) {
			n *= -1;
		}

		unsigned i;
		for (i = 0; i < buflen; ++i) {
			if (opt.upper) {
				buffer[i] = toupper(glyphs[n % opt.radix]);
			} else {
				buffer[i] = glyphs[n % opt.radix];
			}

			if (n < static_cast<T>(opt.radix)) {
				break;
			}

			n /= opt.radix;
		}

		// Check for space
		if ((!sign && (i == buflen))
				|| ((sign || opt.always_prefix) && ((i + 1) == buflen))) {
			return -1;
		}

		if (sign) {
			buffer[++i] = '-';
		} else if (opt.always_prefix) {
			buffer[++i] = '+';
		}

		buffer[++i] = '\0';

		reverse_string(buffer);

		return i;
	}

inline bool to_string(double n, char* buffer, unsigned buflen, to_string_opts opt = {})
{
	long long d1 = n;
	unsigned d2 = (n < 0 ? -1 : 1) * (n - d1) * pow(10, opt.precision);

	int t = to_string(d1, buffer, buflen, opt);
	if (t <= 0) {
		return t;
	}

	// TODO: errcheck
	buffer += t;
	buflen -= t;

	*buffer++ = opt.decimal;
	--buflen;

	opt.always_prefix = false;
	return to_string(d2, buffer, buflen, opt);
}

template <typename T, size_t N>
	inline int to_string(T n, char (&buffer)[N], to_string_opts opt = {})
	{
		return to_string(n, buffer, N, opt);
	}

template <size_t N>
	inline int to_string(double n, char (&buffer)[N], to_string_opts opt = {})
	{
		return to_string(n, buffer, N, opt);
	}
}

#endif
