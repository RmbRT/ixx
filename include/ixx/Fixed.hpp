#ifndef __ixx_fixed_hpp_defined
#define __ixx_fixed_hpp_defined

#include "UInt.hpp"

namespace ixx
{
	template<class Atom, std::size_t kBase, std::size_t kDigits>
	/** Fixed point class.
		One atom is `pow(kBase, -kDigits)` units.
		Maximum magnitude is `uint_t<Atom>::max() * pow(kBase, -kDigits)`.
	@tparam Atom:
		An `ixx::UInt` instantiation.
	@tparam kBase:
		The base for the `kDigits` count.
		2 for bit count, 10 for decimal digit count.
		Non-10 bases will have a precision loss when converting from and to text.
	@tparam kDigits:
		The digits (in kBase representation) after the point. */
	class Fixed
	{
		/** The atom count. */
		Atom m_atoms;
	public:
		/** Zero. */
		Fixed() noexcept;
		Fixed(
			noinit_t) noexcept;

		/** Creates a fixed point from atoms and sign.
		@param[in] atoms:
			The atoms. */
		Fixed(
			Atom const& atoms) noexcept;

		/** The maximum value. */
		static inline Fixed<Atom, kBase, kDigits> const& max();
		/** The smallest possible positive value. */
		static inline Fixed<Atom, kBase, kDigits> const& atom();
		/** @return `1`. */
		static inline Fixed<Atom, kBase, kDigits> const& one();

		/** Parses a fixed point number from a string.
		@param[in] string:
			The fixed point number.
		@param[in] unit:
			How many atoms form a unit. */
		Fixed(
			char const * string);

		Fixed<Atom, kBase, kDigits> operator+(
			Fixed<Atom, kBase, kDigits> const& fixed) const;
		Fixed<Atom, kBase, kDigits> &operator+=(
			Fixed<Atom, kBase, kDigits> const& fixed);

		Fixed<Atom, kBase, kDigits> operator-(
			Fixed<Atom, kBase, kDigits> const& fixed) const;
		Fixed<Atom, kBase, kDigits> &operator-=(
			Fixed<Atom, kBase, kDigits> const& fixed);

		Fixed<Atom, kBase, kDigits> operator/(
			Fixed<Atom, kBase, kDigits> const& fixed) const;

		Fixed<Atom, kBase, kDigits> operator*(
			Fixed<Atom, kBase, kDigits> const& fixed) const;

		inline bool operator<(
			Fixed<Atom, kBase, kDigits> const& rhs) const;
		inline bool operator<=(
			Fixed<Atom, kBase, kDigits> const& rhs) const;
		inline bool operator!=(
			Fixed<Atom, kBase, kDigits> const& rhs) const;
		inline bool operator==(
			Fixed<Atom, kBase, kDigits> const& rhs) const;
		inline bool operator>=(
			Fixed<Atom, kBase, kDigits> const& rhs) const;
		inline bool operator>(
			Fixed<Atom, kBase, kDigits> const& rhs) const;

		/** Converts the number into a string. */
		std::string to_string(
			std::size_t max_digits = (kBase % 10)
				? 10
				: kDigits * (kBase / 10)) const;

		/** Converts the number into a double. */
		double to_double() const noexcept;

		/** The number's atoms.
		@return
			The number's magnitude in atoms. */
		inline Atom const& atoms() const noexcept;
	};
}

template<class Atom, std::size_t kBase, std::size_t kDigits>
std::ostream &operator<<(
	std::ostream &o,
	ixx::Fixed<Atom, kBase, kDigits> const& fixed);

#include "Fixed.inl"

#endif