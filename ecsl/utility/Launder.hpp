#ifndef ECSL_UTILITY_LAUNDER_HPP_
#define ECSL_UTILITY_LAUNDER_HPP_

/**
 * @file Launder.hpp
 * Adds std::launder to non C++17 environments
 */

/// STD
#include <new>

namespace ecsl {

/**
 * @brief Defines the std::launder for non C++17 environments
 * The main purpose of std::launder is to privent reorder compiler from doing
 * the optimizations on assumption that undefined behavior is never happens.
 * The std::launder primarily usable for handling the case when You need to
 * obtain pointer to object from pointer to it's storage (of course the object
 * must be within it's lifetime).
 * @tparam T The type of object located at storage poited by p (must be passed
 * as a type of pointer to that object's type)
 * @tparam U Deduced type of object storage
 * @param[in] p Pointer to object's storage
 */
template <class T, class U>
[[nodiscard]] constexpr T launder(U* p) noexcept
{
    #if defined(__cpp_lib_launder)
        return std::launder(reinterpret_cast<T>(p));
    #else
        return reinterpret_cast<T>(p);
    #endif
}

} // namespace ecsl
#endif /* ECSL_UTILITY_LAUNDER_HPP_ */