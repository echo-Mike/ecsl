#ifndef ECSL_TYPE_TRAITS_MINIMAL_INTEGER_HPP_
#define ECSL_TYPE_TRAITS_MINIMAL_INTEGER_HPP_

/**
 * @file MinimalInteger.hpp
 * Defines trait for identification of integer representation
 * of arbitrary types.
 */

/// STD
#include <type_traits>

namespace ecsl {
namespace detail {

void signed_minimal_integer_helper(...);

template<class T, class = typename std::enable_if<
    sizeof(T) <= sizeof(signed char)
>::type>
signed char signed_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(signed char) < sizeof(T) && sizeof(T) <= sizeof(signed short)
>::type>
signed short signed_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(signed short) < sizeof(T) && sizeof(T) <= sizeof(signed int)
>::type>
signed int signed_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(signed int) < sizeof(T) && sizeof(T) <= sizeof(signed long)
>::type>
signed long signed_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(signed long) < sizeof(T) && sizeof(T) <= sizeof(signed long long)
>::type>
signed long long signed_minimal_integer_helper(T*);

void unsigned_minimal_integer_helper(...);

template<class T, class = typename std::enable_if<
    sizeof(T) <= sizeof(unsigned char)
>::type>
unsigned char unsigned_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(unsigned char) < sizeof(T) && sizeof(T) <= sizeof(unsigned short)
>::type>
unsigned short unsigned_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(unsigned short) < sizeof(T) && sizeof(T) <= sizeof(unsigned int)
>::type>
unsigned int unsigned_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(unsigned int) < sizeof(T) && sizeof(T) <= sizeof(unsigned long)
>::type>
unsigned long unsigned_minimal_integer_helper(T*);

template<class T, class = typename std::enable_if<
    sizeof(unsigned long) < sizeof(T) && sizeof(T) <= sizeof(unsigned long long)
>::type>
unsigned long long unsigned_minimal_integer_helper(T*);

template<class T>
struct signed_minimal_integer_trait_helper
{
    using type = decltype(signed_minimal_integer_helper(static_cast<T*>(0)));
};

template<class T>
struct unsigned_minimal_integer_trait_helper
{
    using type = decltype(unsigned_minimal_integer_helper(static_cast<T*>(0)));
};

template<class U>
struct minimal_integer_trait_cond
{
    using type = U;
};

template<>
struct minimal_integer_trait_cond<void> {};

template<class T>
using signed_minimal_integer_trait = typename minimal_integer_trait_cond<
    typename signed_minimal_integer_trait_helper<T>::type
>::type;

template<class T>
using unsigned_minimal_integer_trait = typename minimal_integer_trait_cond<
    typename unsigned_minimal_integer_trait_helper<T>::type
>::type;

template <class T>
using has_signed_minimal_integer_trait = typename std::conditional<
    std::is_same<
        void,
        typename signed_minimal_integer_trait_helper<T>::type
    >::value,
    std::false_type,
    std::true_type
>::type;

template <class T>
using has_unsigned_minimal_integer_trait = typename std::conditional<
    std::is_same<
        void,
        typename unsigned_minimal_integer_trait_helper<T>::type
    >::value,
    std::false_type,
    std::true_type
>::type;

} // namespace detail

template <class T>
/**
 * Defines minimal signed integer type
 * that is capable of holding T's bit representation.
 * Will fault the compilation if no known signed integer type
 * is capable of holding T's bit representation.
 * Use ecsl::has_signed_minimal_integer<T>::value to check
 */
struct signed_minimal_integer
{
    using type = detail::signed_minimal_integer_trait<T>;
};

template<class T>
using signed_minimal_integer_t = typename signed_minimal_integer<T>::type;

template<class T>
/* may be std::true_type or std::false_type */
using has_signed_minimal_integer = detail::has_signed_minimal_integer_trait<T>;

template <class T>
/**
 * Defines minimal unsigned integer type
 * that is capable of holding T's bit representation.
 * Will fault the compilation if no known unsigned integer type
 * is capable of holding T's bit representation.
 * Use ecsl::has_unsigned_minimal_integer<T>::value to check
 */
struct unsigned_minimal_integer
{
    using type = detail::unsigned_minimal_integer_trait<T>;
};

template<class T>
using unsigned_minimal_integer_t = typename unsigned_minimal_integer<T>::type;

template<class T>
/* may be std::true_type or std::false_type */
using has_unsigned_minimal_integer = detail::has_unsigned_minimal_integer_trait<T>;

template <class T>
/**
 * Defines minimal integer (signed or not is not specified) type
 * that is capable of holding T's bit representation.
 * Will fault the compilation if no known integer type
 * is capable of holding T's bit representation.
 * Use ecsl::has_minimal_integer<T>::value to check
 */
struct minimal_integer
{
    using type = detail::unsigned_minimal_integer_trait<T>;
};

template<class T>
using minimal_integer_t = typename minimal_integer<T>::type;

template<class T>
/* may be std::true_type or std::false_type */
using has_minimal_integer = detail::has_unsigned_minimal_integer_trait<T>;

} // namespace ecsl
#endif /* ECSL_TYPE_TRAITS_MINIMAL_INTEGER_HPP_ */