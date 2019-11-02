#ifndef ECSL_UTILITY_BLESS_HPP_
#define ECSL_UTILITY_BLESS_HPP_

/**
 * @file Bless.hpp
 * Adds a way to create some type of objects in provided memory location
 */

/// STD
#include <cstring>
#include <type_traits>
#include <new>

namespace ecsl {

/**
 * @brief Creates an object at location provided avoiding undefined behavior.
 *
 * Wether the location must be aligned properly is implementation
 * defined. This function is a compiler optimization barrier. Using
 * this You provide to compiler clear intent to create ("bless into existence")
 * object at some location in memory avoiding undefined behavior.
 * On most platforms compiles to noop on maximum optimization level and
 * only prevents cirtain types of optimizations that are based on compiler
 * assumption that UB is never happens.
 *
 * Only default constructible, trivially copyable (using memcpy) objects
 * without const qualified members may be created this way and only in
 * non-constant memory location.
 */
template<class T>
typename std::enable_if<
    std::is_trivially_copy_constructible<T>::value,
    T*
>::type bless(void* p) noexcept(std::is_nothrow_default_constructible<T>::value)
{
    typename std::aligned_storage<sizeof(T), alignof(T)>::type s;
    auto b = reinterpret_cast<unsigned char*>(p);
    std::memcpy(&s, b, sizeof(T));
    auto r = ::new(b) T;
    std::memcpy(r, &s, sizeof(T));
    return r;
}

/**
 * @brief Pair function to ecsl::bless deletes object in cirtain memory location.
 * If object was "blessed into existence" with ecsl::bless and no longer needed
 * it must be exterminated with call to this function.
 * Compiles into no-op for objects that are created via ecsl::bless
 */
template<class T>
void exterminate(T* p) noexcept(std::is_nothrow_destructible<T>::value)
{
    p->~T();
}
} // namespace ecsl
#endif /* ECSL_UTILITY_BLESS_HPP_ */