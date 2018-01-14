namespace ixx
{
	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits>::Fixed() noexcept:
		m_atoms()
	{
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits>::Fixed(
		noinit_t) noexcept:
		m_atoms(noinit)
	{
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits>::Fixed(
		Atom const& atoms) noexcept:
		m_atoms(atoms)
	{
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> const& Fixed<Atom, kBase, kDigits>::max()
	{
		static Fixed<Atom, kBase, kDigits> const value{Atom::max()};
		return value;
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> const& Fixed<Atom, kBase, kDigits>::atom()
	{
		static Fixed<Atom, kBase, kDigits> const value{
			(std::uint8_t)1,
			false};
		return value;
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> const& Fixed<Atom, kBase, kDigits>::one()
	{
		static Fixed<Atom, kBase, kDigits> const value{Atom::pow(kBase, kDigits)};
		return value;
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits>::Fixed(
		char const * string)
	{
		static Atom const ten{10};
		for(std::size_t i = 0; string[i]; i++)
		{
			if(string[i] == '.')
			{
				if(kBase <= 1 || kDigits == 0)
					throw std::domain_error("no decimal point allowed.");

				Atom factor = one().atoms();
				m_atoms *= factor;

				std::size_t j;
				for(j = 1; string[i+j] && factor >= kBase; j++)
				{
					if(string[i+j] < '0' || string[i+j] > '9')
						throw std::domain_error("number string expected.");
					factor = (factor / ten).quotient;
					unsigned char digit = string[i+j]-'0';
					if(digit)
						m_atoms += factor * digit;
				}

				if(string[i+j])
					throw std::runtime_error("Truncation on exceeded precision (Too many decimal places).");
				return;
			}
			if(string[i] < '0' || string[i] > '9')
				throw std::domain_error("number string expected.");

			if(i)
				m_atoms *= ten;
			m_atoms += (unsigned char)(string[i]-'0');
		}

		m_atoms *= one().atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	std::string Fixed<Atom, kBase, kDigits>::to_string(
		std::size_t max_digits) const
	{
		static Atom const ten{10};
		auto div = m_atoms / one().m_atoms;
		std::string result = div.quotient.to_string();
		if(div.remainder != 0)
		{
			result.push_back('.');

			while(max_digits--)
			{
				div.remainder *= ten;
				div = div.remainder / one().m_atoms;
				result.push_back('0' + div.quotient.template to<unsigned char>());

				if(div.remainder == 0)
					break;
			}
		}

		return std::move(result);
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> Fixed<Atom, kBase, kDigits>::operator+(
		Fixed<Atom, kBase, kDigits> const& fixed) const
	{
		return Fixed{m_atoms + fixed.m_atoms};
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> &Fixed<Atom, kBase, kDigits>::operator+=(
		Fixed<Atom, kBase, kDigits> const& fixed)
	{
		m_atoms += fixed.m_atoms;
		return *this;
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> Fixed<Atom, kBase, kDigits>::operator-(
		Fixed<Atom, kBase, kDigits> const& fixed) const
	{
		return Fixed{m_atoms - fixed.m_atoms};
	}


	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> &Fixed<Atom, kBase, kDigits>::operator-=(
		Fixed<Atom, kBase, kDigits> const& fixed)
	{
		m_atoms -= fixed.m_atoms;
		return *this;
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits> Fixed<Atom, kBase, kDigits>::operator/(
		Fixed<Atom, kBase, kDigits> const& fixed) const
	{
		typedef typename Atom::twice_t twice_t;

		static twice_t const& twice_one = Fixed<twice_t, kBase, kDigits>::one().atoms();

		auto div = (twice_one*m_atoms) / fixed.atoms();

		return Atom{div.quotient};
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Fixed<Atom, kBase, kDigits>::operator*(
		Fixed<Atom, kBase, kDigits> const& fixed) const
	{
		typedef typename Atom::twice_t twice_t;

		auto ldiv = atoms() / one();
		auto rdiv = fixed.atoms() / one();

		return one() * ldiv.quotient * rdiv.quotient
			+ (ldiv.quotient * rdiv.remainder)
			+ (ldiv.remainder * rdiv.quotient)
			+ ((ldiv.remainder * rdiv.remainder) / one()).quotient;
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	bool Fixed<Atom, kBase, kDigits>::operator<(
		Fixed<Atom, kBase, kDigits> const& rhs) const
	{
		return atoms() < rhs.atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	bool Fixed<Atom, kBase, kDigits>::operator<=(
		Fixed<Atom, kBase, kDigits> const& rhs) const
	{
		return atoms() <= rhs.atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	bool Fixed<Atom, kBase, kDigits>::operator!=(
		Fixed<Atom, kBase, kDigits> const& rhs) const
	{
		return atoms() != rhs.atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	bool Fixed<Atom, kBase, kDigits>::operator==(
		Fixed<Atom, kBase, kDigits> const& rhs) const
	{
		return atoms() == rhs.atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	bool Fixed<Atom, kBase, kDigits>::operator>=(
		Fixed<Atom, kBase, kDigits> const& rhs) const
	{
		return atoms() >= rhs.atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	bool Fixed<Atom, kBase, kDigits>::operator>(
		Fixed<Atom, kBase, kDigits> const& rhs) const
	{
		return atoms() > rhs.atoms();
	}

	template<class Atom, std::size_t kBase, std::size_t kDigits>
	Atom const& Fixed<Atom, kBase, kDigits>::atoms() const noexcept
	{
		return m_atoms;
	}
}

template<class Atom, std::size_t kBase, std::size_t kDigits>
std::ostream &operator<<(
	std::ostream &o,
	ixx::Fixed<Atom, kBase, kDigits> const& fixed)
{
	return o << fixed.to_string();
}