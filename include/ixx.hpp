#ifndef __ixx_ixx_hpp_defined
#define __ixx_ixx_hpp_defined

#include <type_traits>
#include <new>

namespace ixx
{
	/** The exception status of an arithmetic operation. */
	enum class Exception : char
	{
		/** No exception. */
		kNone,
		/** Overflow / Carry. */
		kOverflow,
		/** Underflow. */
		kUnderflow
	};

	template<class T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	struct Sum
	{
		T value;
		Exception exception;
	};

	template<class T>
	struct is_uint
	{
		static constexpr bool value =
			std::is_integral<T>::value
		&&  std::is_unsigned<T>::value;
	};

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	/** Addition with carry detection.
	@param[in] a:
		The first summand.
	@param[in] b:
		The second summand.
	@param[in] carry:
		The carry of the previous addition.
	@return
		The sum. */
	inline Sum<T> add(
		T a,
		T b,
		bool carry,
		std::nothrow_t) noexcept;

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	/** Addition with exception detection.
	@param[in] a:
		The first summand.
	@param[in] b:
		The second summand.
	@return
		The sum.
	@throws std::overflow_error
		If there is an overflow. */
	inline T add(
		T a,
		T b,
		bool carry);

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	inline Sum<T> sub(
		T a,
		T b,
		bool carry,
		std::nothrow_t) noexcept;

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	inline T sub(
		T a,
		T b,
		bool carry);

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	struct Product
	{
		T low;
		T high;
	};

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	inline Product<T> mul(
		T a,
		T b,
		std::nothrow_t) noexcept;

	template<class T, class = typename std::enable_if<ixx::is_uint<T>::value>::type>
	inline T mul(
		T a,
		T b);

	template<class T>
	struct is_sint
	{
		static constexpr bool value = std::is_integral<T>::value && std::is_signed<T>::value;
	};

	template<class T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	/** Checks whether a value is negative.
		This can be specialised for custom types.
	@param[in] value:
		The value to check.
	@return
		Whether the value is negative. */
	bool is_negative(
		T const& value) noexcept;

	template<
		class T,
		class = typename std::enable_if<std::is_integral<T>::value>::type>
	/** Addition with exception detection.
	@param[in] a:
		The first summand.
	@param[in] b:
		The second summand.
	@return
		The sum. */
	inline Sum<T> add(
		T a,
		T b,
		std::nothrow_t) noexcept;

	template<
		class T,
		class = typename std::enable_if<std::is_integral<T>::value>::type>
	/** Addition with exception detection.
	@param[in] a:
		The first summand.
	@param[in] b:
		The second summand.
	@return
		The sum.
	@throws std::overflow_exception
		If an overflow occurs.
	@throws std::underflow_exception
		If `T` is signed and an underflow occurs. */
	inline T add(
		T a,
		T b);

	template<
		class T,
		class = typename std::enable_if<std::is_integral<T>::value>::type>
	/** Subtraction with exception detection.
	@param[in] a:
		The minuend.
	@param[in] b:
		The subtrahend.
	@return
		The difference. */
	inline Sum<T> sub(
		T a,
		T b,
		std::nothrow_t) noexcept;

	template<
		class T,
		class = typename std::enable_if<std::is_integral<T>::value>::type>
	/** Subtraction with exception detection.
	@param[in] a:
		The minuend.
	@param[in] b:
		The subtrahend.
	@return
		The difference.
	@throws std::overflow_exception
		If `T` is signed and an overflow occurs.
	@throws std::underflow_exception
		If an underflow occurs. */
	inline T sub(
		T a,
		T b);
}

#include "ixx.inl"

#endif