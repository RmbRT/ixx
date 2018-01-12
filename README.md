# ixx

> Integer Library for C++

This library is intended to create an exception-aware interface for integers in C++. It not only allows the detection of exceptions in integer arithmetic, but also composes an interface for custom sized integer types. The code is documented in a fashion compatible with doxygen.

## License

This library is released under the *GNU Affero General Public License, version 3* or newer.

## Contents

Code files:

 * `include/ixx.hpp`: This file contains the core functionality of the library: Functions for exception-aware addition, subtraction and multiplication on native integers.
 * `include/UInt.hpp`: This file contains a class for custom-sized unsigned integers.
 * `include/Fixed.hpp`: This file contains a class for custom-sized unsigned fixed point types, where the base factor is customizable.

Additional files:

 * `README.md`: This file.
 * `LICENSE`: A copy of the GNU Affero General Public License, version 3.