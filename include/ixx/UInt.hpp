#ifndef __ixx_uint_hpp_defined
#define __ixx_uint_hpp_defined

#include <cstddef>
#include "ixx.hpp"
#include <array>
#include <iostream>

namespace ixx
{
	struct noinit_t {} const noinit;

	template<class Digit, std::size_t kDigits>
	class UInt;

	template<class Digit, std::size_t kDigits>
	struct is_uint<UInt<Digit, kDigits>>
	{
		static constexpr bool value = true;
	};

	template<class Digit, std::size_t kDigits>
	class UInt
	{
		std::array<Digit, kDigits> m_digits;
	public:
		static_assert(std::is_unsigned<Digit>::value, "Only unsigned digit types allowed.");

		UInt() noexcept;
		UInt(
			noinit_t) noexcept;

		template<
			class Number,
			class = typename std::enable_if<std::is_integral<Number>::value>::type>
		UInt(
			Number value);

		template<
			std::size_t kOtherDigits,
			class = typename std::enable_if<kOtherDigits != kDigits>::type>
		UInt(
			UInt<Digit, kOtherDigits> const& other);

		UInt<Digit, kDigits> operator+(
			UInt<Digit, kDigits> const& other) const;
		UInt<Digit, kDigits> &operator+=(
			UInt<Digit, kDigits> const& other);

		UInt<Digit, kDigits> operator-(
			UInt<Digit, kDigits> const& other) const;
		UInt<Digit, kDigits> &operator-=(
			UInt<Digit, kDigits> const& other);

		UInt<Digit, kDigits> operator*(
			UInt<Digit, kDigits> const& other) const;
		UInt<Digit, kDigits> &operator*=(
			UInt<Digit, kDigits> const& other);

		UInt<Digit, kDigits*2> mul(
			UInt<Digit, kDigits> const& other,
			std::nothrow_t) const;

		UInt<Digit, kDigits> operator<<(
			std::size_t amount) const;
		UInt<Digit, kDigits> &operator<<=(
			std::size_t amount);
		UInt<Digit, kDigits> operator>>(
			std::size_t amount) const;
		UInt<Digit, kDigits> &operator>>=(
			std::size_t amount);

		struct Division
		{
			UInt<Digit, kDigits> quotient;
			UInt<Digit, kDigits> remainder;
		};

		Division operator/(
			UInt<Digit, kDigits> const& other) const;

		template<
			class Number,
			class = typename std::enable_if<std::is_integral<Number>::value>::type>
		Number to() const;

		bool operator<(
			UInt<Digit, kDigits> const& other) const noexcept;
		bool operator<=(
			UInt<Digit, kDigits> const& other) const noexcept;
		bool operator>(
			UInt<Digit, kDigits> const& other) const noexcept;
		bool operator>=(
			UInt<Digit, kDigits> const& other) const noexcept;
		bool operator!=(
			UInt<Digit, kDigits> const& other) const noexcept;
		bool operator==(
			UInt<Digit, kDigits> const& other) const noexcept;

		operator bool() const noexcept;
		bool operator!() const noexcept;

		signed compare(
			UInt<Digit, kDigits> const& other) const noexcept;

		/** The power function. */
		static UInt<Digit, kDigits> pow(
			UInt<Digit, kDigits> const& base,
			std::size_t exponent);

		/** The digit type. */
		typedef Digit digit_t;
		typedef UInt<Digit, kDigits*2> twice_t;
		/** The digits, going from the least significant digit to the most significant digit. */
		std::array<Digit, kDigits> const& data() const noexcept;
		/** The digits, going from the least significant digit to the most significant digit. */
		std::array<Digit, kDigits> &data() noexcept;
		/** The digit count. */
		static constexpr std::size_t digits();

		/** The highest possible value. */
		static UInt<Digit, kDigits> const& max() noexcept;

		std::string to_string() const;

		std::size_t highest_digit() const;
	};

	template<std::size_t kBytes>
	struct uint
	{
		typedef typename std::conditional<
				(0 == kBytes % 8),
				UInt<std::uint64_t, kBytes / 8>,
				typename std::conditional<
					(0 == kBytes % 4),
					UInt<std::uint32_t, kBytes / 4>,
					typename std::conditional<
						(0 == kBytes % 2),
						UInt<std::uint16_t, kBytes / 2>,
						UInt<std::uint8_t, kBytes>
					>::type
				>::type
			>::type type;
	};

	template<>
	struct uint<1>
	{
		typedef UInt<std::uint8_t, 1> type;
	};

	template<>
	struct uint<2>
	{
		typedef UInt<std::uint16_t, 1> type;
	};

	template<>
	struct uint<4>
	{
		typedef UInt<std::uint32_t, 1> type;
	};

	template<>
	struct uint<8>
	{
		typedef UInt<std::uint64_t, 1> type;
	};

	template<std::size_t kBytes>
	using uint_t = typename uint<kBytes>::type;

	template<std::size_t kBytes>
	/** The integer with the exact requested size. */
	struct uint_fast
	{
		typedef typename std::conditional<
			(kBytes < 8),
			typename std::conditional<
				(kBytes < 4),
				uint_t<4>,
				uint_t<8>
			>::type,
			UInt<std::uint64_t,
				(kBytes % sizeof(std::uint64_t))
					? (kBytes/sizeof(std::uint64_t)+1)
					: (kBytes/sizeof(std::uint64_t))>
		>::type type;
	};

	template<>
	struct uint_fast<1>
	{
		typedef UInt<std::uint8_t, 1> type;
	};

	template<>
	struct uint_fast<2>
	{
		typedef UInt<std::uint16_t, 1> type;
	};

	template<>
	struct uint_fast<4>
	{
		typedef UInt<std::uint32_t, 1> type;
	};

	template<>
	struct uint_fast<8>
	{
		typedef UInt<std::uint64_t, 1> type;
	};

	template<std::size_t kBytes>
	/** The fastest possible integer with at least the requested size. */
	using uint_fast_t = typename uint_fast<kBytes>::type;
}

template<class Digit, std::size_t kDigits>
std::ostream &operator<<(
	std::ostream &o,
	ixx::UInt<Digit, kDigits> const& uint);

#include "UInt.inl"

#endif