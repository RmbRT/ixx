#include <cassert>

namespace ixx
{
	namespace detail
	{
		template<class Digit, std::size_t kDigits>
		static UInt<Digit, kDigits> const kOne = 1;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits>::UInt() noexcept
	{
		for(std::size_t i = 0; i < kDigits; i++)
			m_digits[i] = 0;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits>::UInt(
		noinit_t) noexcept
	{
	}

	template<class Digit, std::size_t kDigits>
	template<class Number, class>
	UInt<Digit, kDigits>::UInt(
		Number value)
	{
		// Could the value be negative?
		if(std::is_signed<Number>::value)
			if(is_negative(value))
				throw std::underflow_error("Negative value.");

		// Could the number be too great?
		if(sizeof(Number) > sizeof(Digit) * kDigits)
			if(value >> sizeof(Digit) * 8 * kDigits)
				throw std::overflow_error("Loss by truncation.");

		if(sizeof(Digit) < sizeof(Number))
			for(std::size_t i = 0; i < kDigits; i++)
			{
				m_digits[i] = value;
				value >>= sizeof(Digit) * 8;
			}
		else
		{
			m_digits[0] = value;
			for(std::size_t i = 1; i < kDigits; i++)
				m_digits[i] = 0;
		}
	}

	template<class Digit, std::size_t kDigits>
	template<std::size_t kOtherDigits, class>
	UInt<Digit, kDigits>::UInt(
		UInt<Digit, kOtherDigits> const& other)
	{
		if(kOtherDigits > kDigits)
		{
			if(other.highest_digit() >= kDigits)
				throw std::overflow_error("Loss by truncation.");

			for(std::size_t i = 0; i < kDigits; i++)
				m_digits[i] = other.data()[i];
		} else
		{
			for(std::size_t i = 0; i < kOtherDigits; i++)
				m_digits[i] = other.data()[i];
			for(std::size_t i = kOtherDigits; i < kDigits; i++)
				m_digits[i] = 0;
		}
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> UInt<Digit, kDigits>::operator+(
		UInt<Digit, kDigits> const& other) const
	{
		UInt<Digit, kDigits> result{noinit};

		if(kDigits > 1)
		{
			Sum<Digit> temp = add(
				m_digits[0],
				other.m_digits[0],
				std::nothrow);

			result.m_digits[0] = temp.value;

			for(std::size_t i = 1; i < kDigits-1; i++)
			{
				temp = add(
					m_digits[i],
					other.m_digits[i],
					temp.exception != Exception::kNone,
					std::nothrow);

				result.m_digits[i] = temp.value;
			}

			result.m_digits[kDigits-1] = add(
				m_digits[kDigits-1],
				other.m_digits[kDigits-1],
				temp.exception != Exception::kNone);
		} else
		{
			result.m_digits[0] = add(
				m_digits[0],
				other.m_digits[0]);
		}
		return result;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> &UInt<Digit, kDigits>::operator+=(
		UInt<Digit, kDigits> const& other)
	{
		return *this = *this + other;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> UInt<Digit, kDigits>::operator-(
		UInt<Digit, kDigits> const& other) const
	{
		UInt<Digit, kDigits> result{noinit};

		if(kDigits > 1)
		{
			Sum<Digit> temp = sub(
				m_digits[0],
				other.m_digits[0],
				std::nothrow);

			result.m_digits[0] = temp.value;

			for(std::size_t i = 1; i < kDigits-1; i++)
			{
				temp = sub(
					m_digits[i],
					other.m_digits[i],
					temp.exception != Exception::kNone,
					std::nothrow);

				result.m_digits[i] = temp.value;
			}

			result.m_digits[kDigits-1] = sub(
				m_digits[kDigits-1],
				other.m_digits[kDigits-1],
				temp.exception != Exception::kNone);
		} else
		{
			result.m_digits[0] = sub(
				m_digits[0],
				other.m_digits[0]);
		}
		return result;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> &UInt<Digit, kDigits>::operator-=(
		UInt<Digit, kDigits> const& other)
	{
		return *this = *this - other;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> UInt<Digit, kDigits>::operator*(
		UInt<Digit, kDigits> const& other) const
	{
		// easy for single-digits.
		if(kDigits == 1)
		{
			UInt<Digit, kDigits> result;
			result.m_digits[0] = ixx::mul(m_digits[0], other.m_digits[0]);
			return result;
		}

		// detect highest nonzero digits.
		std::size_t high_this = highest_digit();
		std::size_t high_other = other.highest_digit();

		if(!m_digits[high_this] || !other.m_digits[high_other])
			return UInt<Digit, kDigits>();

		// check for overflow.
		if(high_this + high_other > kDigits - 1)
			throw std::overflow_error("Unsigned multiplication overflow.");
		/*else if(high_this + high_other == kDigits-1)
		{
			// multiply the highest digits and check for overflow.
			ixx::mul(m_digits[high_this], other.m_digits[high_other]);
		}*/

		// calculate product.
		UInt<Digit, kDigits> result;
		try
		{
		for(std::size_t i = 0; i <= high_this; i++)
		{
			if(!m_digits[i])
				continue;

			UInt<Digit, kDigits> temp;

			for(std::size_t j = 0; j <= high_other; j++)
			{
				auto product = ixx::mul(
					m_digits[i],
					other.m_digits[j],
					std::nothrow);
				std::size_t target = i+j;
				auto sum = add(
					temp.m_digits[target],
					product.low,
					std::nothrow);
				temp.m_digits[target] = sum.value;
				// check for overflow.
				if(target+1 >= kDigits
				&& (sum.exception != Exception::kNone
				|| product.high))
				{
					throw std::overflow_error("Unsigned multiplication overflow.");
				}
				temp.m_digits[target+1] = add(
					temp.m_digits[target+1],
					product.high,
					sum.exception != Exception::kNone);
				target += 2;
			}
			result += temp;
		}
		} catch(...)
		{
			throw std::overflow_error("Unsigned multiplication overflow.");
		}

		return result;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> &UInt<Digit, kDigits>::operator*=(
		UInt<Digit, kDigits> const& other)
	{
		return *this = *this * other;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits*2> UInt<Digit, kDigits>::mul(
		UInt<Digit, kDigits> const& other,
		std::nothrow_t) const
	{
		UInt<Digit, 2*kDigits> big_this{noinit};
		UInt<Digit, 2*kDigits> big_other{noinit};

		for(std::size_t i = 0; i < kDigits; i++)
		{
			big_this.data()[i] = m_digits[i];
			big_other.data()[i] = other.m_digits[i];
		}
		for(std::size_t i = kDigits; i<2*kDigits; i++)
		{
			big_this.data()[i] = 0;
			big_other.data()[i] = 0;
		}

		return big_this * big_other;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> UInt<Digit, kDigits>::operator<<(
		std::size_t amount) const
	{
		return UInt<Digit, kDigits>{*this} <<= amount;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> &UInt<Digit, kDigits>::operator<<=(
		std::size_t amount)
	{
		if(!amount)
			return *this;

		std::size_t const digits = amount / (sizeof(Digit) * 8);
		std::size_t const bits = amount % (sizeof(Digit) * 8);

		if(digits >= kDigits)
			throw std::logic_error("Unsigned shift left >= type size.");

		// detect overflow.
		for(std::size_t i = kDigits; i-->kDigits-digits;)
			if(m_digits[i])
				throw std::overflow_error("Unsigned shift left overflow.");

		if(!bits)
		{
			for(std::size_t i = kDigits; i-->digits;)
				m_digits[i] = m_digits[i-digits];
		} else
		{
			// detect overflow.
			if(m_digits[kDigits-digits-1] >> (sizeof(Digit) * 8 - bits))
				throw std::overflow_error("Unsigned shift left overflow.");

			for(std::size_t i = kDigits; i-->digits+1;)
				m_digits[i] = (m_digits[i-digits] << bits)
					| (m_digits[i-digits-1] >> (sizeof(Digit) * 8 - bits));

			m_digits[digits] = m_digits[0] << bits;
		}


		for(std::size_t i = digits; i--;)
			m_digits[i] = 0;

		return *this;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> UInt<Digit, kDigits>::operator>>(
		std::size_t amount) const
	{
		return UInt<Digit, kDigits>{*this} >>= amount;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> &UInt<Digit, kDigits>::operator>>=(
		std::size_t amount)
	{
		if(!amount)
			return *this;

		std::size_t const digits = amount / (sizeof(Digit) * 8);
		std::size_t const bits = amount % (sizeof(Digit) * 8);

		if(digits >= kDigits)
			throw std::logic_error("Unsigned shift right >= type size.");

		if(!bits)
		{
			for(std::size_t i = 0; i<kDigits-digits; i++)
				m_digits[i] = m_digits[i+digits];
		} else
		{
			for(std::size_t i = 0; i<kDigits-1-digits; i++)
				m_digits[i] = (m_digits[i+digits] >> bits)
					| (m_digits[i+digits+1] << (sizeof(Digit) * 8 - bits));

			m_digits[kDigits-1-digits] = m_digits[kDigits-1] >> bits;
		}

		for(std::size_t i = kDigits-digits; i<kDigits; i++)
			m_digits[i] = 0;

		return *this;
	}

	template<class Digit, std::size_t kDigits>
	typename UInt<Digit, kDigits>::Division UInt<Digit, kDigits>::operator/(
		UInt<Digit, kDigits> const& other) const
	{
		std::size_t high_other = other.highest_digit();

		// check for 0 division.
		if(!other.m_digits[high_other])
			throw std::invalid_argument("Unsigned division by 0.");

		Division result;

		if(kDigits == 1)
		{
			result.quotient = m_digits[0] / other.m_digits[0];
			result.remainder = m_digits[0] % other.m_digits[0];
		} else
		{
			std::size_t high_this = highest_digit();

			// only calculate if this is not 0.
			if(m_digits[high_this])
			{
				// check for 0 quotient.
				if(high_other > high_this
				|| (high_other == high_this
				&& *this < other))
				{
					result.remainder = *this;
				} else if(high_other == high_this
				&& *this == other)
				{
					result.quotient = 1;
				} else // this > other
				{
					// get the order of magnitude of difference.
					std::size_t shift = 0;
					while(m_digits[high_this]>>shift > other.m_digits[high_other])
						++shift;
					if(m_digits[high_this]>>shift < other.m_digits[high_other])
						--shift;

					UInt<Digit, kDigits> countdown{*this};
					shift += (high_this - high_other) * (8 * sizeof(Digit));
					// decrease >= this
					UInt<Digit, kDigits> decrease{other<<shift};

					for(;;)
					{
						signed const compare = decrease.compare(countdown);

						if(compare == 0)
						{
							// result.quotient |= 1 << shift;
							std::size_t const digit = shift / (sizeof(Digit) * 8);
							std::size_t const bit = shift % (sizeof(Digit) * 8);
							result.quotient.m_digits[digit] |= Digit(1) << bit;
							break;
						} else if(compare > 0)
						{
							if(shift)
							{
								--shift;
								decrease >>= 1;
							} else
							{
								result.remainder = countdown;
								break;
							}
						} else
						{
							countdown -= decrease;
							decrease >>= 1;
							// result.quotient |= 1 << shift;
							std::size_t const digit = shift / (sizeof(Digit) * 8);
							std::size_t const bit = shift % (sizeof(Digit) * 8);
							result.quotient.m_digits[digit] |= Digit(1) << bit;

							if(shift)
								--shift;
							else
							{
								result.remainder = countdown;
								break;
							}
						}
					}
				}
			}
		}
		return result;
	}

	template<class Digit, std::size_t kDigits>
	template<class Number, class>
	Number UInt<Digit, kDigits>::to() const
	{
		// The highest digit that fits into the returned number.
		std::size_t const last = sizeof(Number) / sizeof(Digit);
		// check for overflow.
		if(sizeof(Number) < sizeof(Digit) * kDigits)
		{
			if(last)
			{
				for(std::size_t i = last; i < kDigits; i++)
					if(m_digits[i])
						throw std::overflow_error("Loss by truncation.");
			} else if(m_digits[0] > std::numeric_limits<Number>::max())
				throw std::overflow_error("Loss by truncation.");
		}
		// does the value have to be constructed?
		if(sizeof(Number) > sizeof(Digit))
		{
			Number result = 0;
			for(std::size_t i = last >= kDigits ? kDigits : last; i--;)
			{
				result <<= sizeof(Digit) * 8;
				result = result | Number(m_digits[i]);
			}

			return result;
		} else
			return m_digits[0];
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator<(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
				return m_digits[i] < other.m_digits[i];
		return false;
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator<=(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
				return m_digits[i] < other.m_digits[i];
		return true;
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator>(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
				return m_digits[i] > other.m_digits[i];
		return false;
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator>=(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
				return m_digits[i] > other.m_digits[i];
		return true;
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator!=(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
				return true;
		return false;
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator==(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
				return false;
		return true;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits>::operator bool() const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i])
				return true;
		return false;
	}

	template<class Digit, std::size_t kDigits>
	bool UInt<Digit, kDigits>::operator!() const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i])
				return false;
		return true;
	}

	template<class Digit, std::size_t kDigits>
	signed UInt<Digit, kDigits>::compare(
		UInt<Digit, kDigits> const& other) const noexcept
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i] != other.m_digits[i])
			{
				if(m_digits[i] > other.m_digits[i])
					return 1;
				else
					return -1;
			}
		return 0;
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> UInt<Digit, kDigits>::pow(
		UInt<Digit, kDigits> const& base,
		std::size_t exponent)
	{
		if(base <= detail::kOne<Digit, kDigits> || exponent == 1)
			return base;
		if(!exponent)
			return Digit(1);

		UInt<Digit, kDigits> factor = base;
		UInt<Digit, kDigits> accum = (exponent & 1)
			? base
			: detail::kOne<Digit, kDigits>;
		exponent &= ~std::size_t(1);
		for(std::size_t i = 1; i < 8*sizeof(exponent) && exponent; i++)
		{
			factor *= factor;
			if(exponent & (std::size_t(1)<<i))
			{
				accum *= factor;
				exponent ^= std::size_t(1)<<i;
			}
		}

		return accum;
	}

	template<class Digit, std::size_t kDigits>
	std::array<Digit, kDigits> const& UInt<Digit, kDigits>::data() const noexcept
	{
		return m_digits;
	}
	template<class Digit, std::size_t kDigits>
	std::array<Digit, kDigits> &UInt<Digit, kDigits>::data() noexcept
	{
		return m_digits;
	}

	template<class Digit, std::size_t kDigits>
	constexpr std::size_t UInt<Digit, kDigits>::digits()
	{
		return kDigits;
	}
	namespace detail
	{
		template<class Digit, std::size_t kDigits>
		UInt<Digit, kDigits> max() noexcept
		{
			UInt<Digit, kDigits> result{noinit};
			for(std::size_t i = kDigits; i--;)
				result.data()[i] = ~Digit(0);
			return result;
		}
	}

	template<class Digit, std::size_t kDigits>
	UInt<Digit, kDigits> const& UInt<Digit, kDigits>::max() noexcept
	{
		static UInt<Digit, kDigits> const result = detail::max<Digit, kDigits>();
		return result;
	}

	template<class Digit, std::size_t kDigits>
	std::string UInt<Digit, kDigits>::to_string() const
	{
		static UInt<Digit, kDigits> const ten{10};
		std::string result;
		result.reserve(3*kDigits);
		Division div = *this / ten;

		do {
			result.insert(result.begin(), '0' + div.remainder.template to<unsigned char>());
			div = div.quotient / ten;
		} while(div.quotient);

		if(div.remainder)
			result.insert(result.begin(), '0' + div.remainder.template to<unsigned char>());

		return std::move(result);
	}

	template<class Digit, std::size_t kDigits>
	std::size_t UInt<Digit, kDigits>::highest_digit() const
	{
		for(std::size_t i = kDigits; i--;)
			if(m_digits[i])
				return i;
		return 0;
	}
}

template<class Digit, std::size_t kDigits>
std::ostream &operator<<(
	std::ostream &o,
	ixx::UInt<Digit, kDigits> const& uint)
{
	if(sizeof(Digit) * kDigits <= sizeof(std::size_t))
		return o << uint.template to<std::size_t>();
	else
		return o << uint.to_string();
}