#include <limits>
#include <stdexcept>

/** The integer for C++ library. */
namespace ixx
{
	template<class T, class>
	Sum<T> add(
		T a,
		T b,
		bool carry,
		std::nothrow_t) noexcept
	{
		Sum<T> result;
		result.value = a + b;
		if(carry)
		{
			result.exception = (b >= std::numeric_limits<T>::max() - a)
				? Exception::kOverflow
				: Exception::kNone;
			++result.value;
		} else
		{
			result.exception = (b > std::numeric_limits<T>::max() - a)
				? Exception::kOverflow
				: Exception::kNone;
		}

		return result;
	}

	template<class T, class>
	T add(
		T a,
		T b,
		bool carry)
	{
		if(carry)
			if(b >= std::numeric_limits<T>::max() - a)
				throw std::overflow_error("Unsigned addition overflow.");
			else
			{
				T result = a + b;
				return ++result;
			}
		else
			if(b > std::numeric_limits<T>::max() - a)
				throw std::overflow_error("Unsigned addition overflow.");
			else
				return a + b;
	}

	template<class T, class>
	bool is_negative(
		T const& value) noexcept
	{
		return value < 0;
	}

	template<class T, class>
	Sum<T> add(
		T a,
		T b,
		std::nothrow_t) noexcept
	{
		Sum<T> result;
		if(std::is_unsigned<T>::value)
		{
			result.value = a + b;
			result.exception = (b > std::numeric_limits<T>::max() - a)
				? Exception::kOverflow
				: Exception::kNone;
		} else
		{
			result.value = a + b;
			if(is_negative(a))
				result.exception = (b < std::numeric_limits<T>::min() - a)
					? Exception::kUnderflow
					: Exception::kNone;
			else
				result.exception = (b > std::numeric_limits<T>::max() - a)
					? Exception::kOverflow
					: Exception::kNone;
		}
		return result;
	}

	template<class T, class>
	T add(
		T a,
		T b)
	{
		if(std::is_unsigned<T>::value)
		{
			if(b > std::numeric_limits<T>::max() - a)
				throw std::overflow_error("Unsigned addition overflow.");
			else
				return a + b;
		} else
		{
			if(is_negative(a))
			{
				if(b < std::numeric_limits<T>::min() - a)
					throw std::underflow_error("Signed addition underflow.");
			}
			else
			{
				if(b > std::numeric_limits<T>::max() - a)
					throw std::overflow_error("Signed addition overflow.");
			}
			return a + b;
		}
	}

	template<class T, class>
	inline Sum<T> sub(
		T a,
		T b,
		bool carry,
		std::nothrow_t) noexcept
	{
		Sum<T> result;
		result.value = a - b;
		if(std::is_unsigned<T>::value)
		{
			if(carry)
			{
				--result.value;
				result.exception = (b >= a)
					? Exception::kUnderflow
					: Exception::kNone;
			} else
			{
				result.exception = (b > a)
					? Exception::kUnderflow
					: Exception::kNone;
			}
		} else
		{
			if(carry)
			{
				--result.value;
				if(is_negative(a))
					result.exception = (b >= std::numeric_limits<T>::min() - a)
						? Exception::kUnderflow
						: Exception::kNone;
				else
					result.exception = (b <= a - std::numeric_limits<T>::max())
						? Exception::kOverflow
						: Exception::kNone;
			} else
			{
				if(is_negative(a))
					result.exception = (b > std::numeric_limits<T>::min() - a)
						? Exception::kUnderflow
						: Exception::kNone;
				else
					result.exception = (b < a - std::numeric_limits<T>::max())
						? Exception::kOverflow
						: Exception::kNone;
			}
		}
		return result;
	}

	template<class T, class>
	inline T sub(
		T a,
		T b,
		bool carry)
	{
		if(std::is_unsigned<T>::value)
		{
			if(carry)
			{
				if(b >= a)
					throw std::underflow_error("Unsigned subtraction underflow.");
				else
					return a - b - 1;
			} else
			{
				if(b > a)
					throw std::underflow_error("Unsigned subtraction underflow.");
				else
					return a - b;
			}
		} else
		{
			if(carry)
			{
				if(is_negative(a))
				{
					if(b >= std::numeric_limits<T>::min() - a)
						throw std::underflow_error("Signed subtraction underflow.");
				}
				else
				{
					if(b <= a - std::numeric_limits<T>::max())
						throw std::overflow_error("Signed subtraction overflow.");
				}
				return a - b - 1;
			} else
			{
				if(is_negative(a))
				{
					if(b > std::numeric_limits<T>::min() - a)
						throw std::underflow_error("Signed subtraction underflow.");
				}
				else
				{
					if(b < a - std::numeric_limits<T>::max())
						throw std::overflow_error("Signed subtraction overflow.");
				}
				return a - b;
			}
		}
	}

	template<class T, class>
	Product<T> mul(
		T a,
		T b,
		std::nothrow_t) noexcept
	{
		std::size_t const half = sizeof(T) * 4;
		T const half_mask = (T(1) << half)-T(1);
		T high_a = a >> half;
		T high_b = b >> half;
		T low_a = a & half_mask;
		T low_b = b & half_mask;

		T mid_1 = high_a * low_b;
		T mid_2 = low_a * high_b;

		T carry = T(T(mid_1 & half_mask) + T(mid_2 & half_mask)) & T(T(1) << half);

		Sum<T> sum = add<T>(
			low_a * low_b,
			(mid_1 + mid_2) << half,
			std::nothrow);
		T low = sum.value;
		T high = T(mid_1>>half)
			+ T(mid_2>>half)
			+ T(carry>>half)
			+ T(sum.exception)
			+ T(high_a * high_b);
		return { low, high };
	}

	template<class T, class>
	T mul(
		T a,
		T b)
	{
		std::size_t const half = sizeof(T) * 4;
		T const half_mask = (T(1) << half)-T(1);
		T high_a = a >> half;
		T high_b = b >> half;
		T low_a = a & half_mask;
		T low_b = b & half_mask;

		T mid_1 = high_a * low_b;
		T mid_2 = low_a * high_b;

		T carry = T(T(mid_1 & half_mask) + T(mid_2 & half_mask)) & T(T(1) << half);

		Sum<T> sum = add<T>(
			low_a * low_b,
			(mid_1 + mid_2) << half,
			std::nothrow);
		T low = sum.value;
		T high = T(mid_1>>half)
			+ T(mid_2>>half)
			+ T(carry>>half)
			+ T(sum.exception)
			+ T(high_a * high_b);
		if(high)
			throw std::overflow_error("Unsigned multiplication overflow.");

		return low;
	}

	template<class T, class>
	Sum<T> sub(
		T a,
		T b,
		std::nothrow_t) noexcept
	{
		if(std::is_unsigned<T>::value)
		{
			Sum<T> result;
			result.value = a - b;
			result.exception = (b > a)
				? Exception::kUnderflow
				: Exception::kNone;

			return result;
		} else
		{
			Sum<T> result;
			result.value = a - b;
			if(is_negative(a))
				result.exception = (b > std::numeric_limits<T>::min() - a)
					? Exception::kUnderflow
					: Exception::kNone;
			else
				result.exception = (b < a - std::numeric_limits<T>::max())
					? Exception::kOverflow
					: Exception::kNone;

			return result;
		}
	}

	template<class T, class>
	T sub(
		T a,
		T b)
	{
		if(std::is_unsigned<T>::value)
		{
			if(b > a)
				throw std::underflow_error("Unsigned subtraction underflow.");
			else
				return a - b;
		} else
		{
			if(is_negative(a))
			{
				if(b > std::numeric_limits<T>::min() - a)
					throw std::underflow_error("Signed subtraction underflow.");
			} else
			{
				if(b < a - std::numeric_limits<T>::max())
					throw std::overflow_error("Signed subtraction overflow.");
			}
			return a - b;
		}
	}
}