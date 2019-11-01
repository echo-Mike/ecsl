#ifndef ECSL_PLATFORM_BYTEORDER_HPP_
#define ECSL_PLATFORM_BYTEORDER_HPP_

/**
 * @file Byteorder.hpp
 * Detects system byteorder and defines functions
 * to convert byteorder of 2, 4 and 8 byte long integers.
 *
 * When possible the compiler or OS intrinsics are used
 */

/// STD
#include <cstdint>
/// ECSL
#include <ecsl/platform/Compiler.hpp>
/// Compiler intrinsics
#if defined(ECSL_COMPILER_MSVC)
#   include <stdlib.h>
#endif
/// OS intrinsics
/// Main source:
/// https://github.com/google/cityhash/blob/master/src/city.cc
#if defined(ECSL_COMPILER_UNKNOWN)
#   if defined(__APPLE__)
// https://opensource.apple.com/source/xnu/xnu-792.13.8/libkern/libkern/OSByteOrder.h
#       include <libkern/OSByteOrder.h>
#   elif defined(__sun) || defined(sun)
//Confirmed for Solaris 10: https://github.com/perim/zzdicom/blob/master/byteorder.h
#       include <sys/byteorder.h>
#   elif defined(__FreeBSD__) || defined(__NetBSD__)
#       include <sys/endian.h>
#   elif defined(__OpenBSD__)
#       include <sys/types.h>
#       include <machine/endian.h>
#   endif
#endif /* ECSL_COMPILER_UNKNOWN */

namespace ecsl {

enum class endianness
{
    unknown = 0x0000,
    big_endian = 0x4321,
    little_endian = 0x1234,

    /**
     * The strongly typed value of platform byte order.
     * Can be used as non type template parameter.
     * Must be accessed by: ecsl::endianness::value
     */
    value =
#if defined(__BYTE_ORDER__)
#   if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        endianness::big_endian,
#   elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        endianness::little_endian,
#   endif
#elif defined(__BYTE_ORDER)
#   if __BYTE_ORDER == __BIG_ENDIAN
        endianness::big_endian,
#   elif __BYTE_ORDER == __LITTLE_ENDIAN
        endianness::little_endian,
#   endif
#elif defined(__BYTE_ORDER)
        endianness::big_endian,
#elif defined(__LITTLE_ENDIAN__)
        endianness::little_endian,
#else
        endianness::unknown,
#endif
};

namespace detail {
namespace byteorder {

enum class os_
{
    os_unknown,
    os_apple,
    os_sun,
    os_freebsd, // and NetBSD
    os_openbsd,

    value =
#if defined(__APPLE__)
        os_::os_apple,
#elif defined(__sun) || defined(sun)
        os_::os_sun,
#   elif defined(__FreeBSD__) || defined(__NetBSD__)
        os_::os_freebsd,
#   elif defined(__OpenBSD__)
        os_::os_openbsd,
#   else
        os_::os_unknown,
#endif
};

template<class T> T simple_bswap(T);

template<>
inline std::uint16_t simple_bswap<std::uint16_t>(std::uint16_t x)
{
    return ((x & 0x00ff) << 8) |
           ((x & 0xff00) >> 8);
}

template<>
inline std::uint32_t simple_bswap<std::uint32_t>(std::uint32_t x)
{
    return (( x & 0x000000ffU ) << 24) |
           (( x & 0x0000ff00U ) <<  8) |
           (( x & 0x00ff0000U ) >>  8) |
           (( x & 0xff000000U ) >> 24);
}

template<>
inline std::uint64_t simple_bswap<std::uint64_t>(std::uint64_t x)
{
    return (( x & 0x00000000000000ffULL ) << 56) |
           (( x & 0x000000000000ff00ULL ) << 40) |
           (( x & 0x0000000000ff0000ULL ) << 24) |
           (( x & 0x00000000ff000000ULL ) <<  8) |
           (( x & 0x000000ff00000000ULL ) >>  8) |
           (( x & 0x0000ff0000000000ULL ) >> 24) |
           (( x & 0x00ff000000000000ULL ) >> 40) |
           (( x & 0xff00000000000000ULL ) >> 56);
}

template<class T,
    compiler::type = compiler::type::value,
    os_ = os_::value
>
T intrinsic_bswap(T x)
{   // Case for compiler::type::compiler_unknown and os_::os_unknown
    return simple_bswap(x);
}

/// Compiler based intrinsic bswap

#if defined(ECSL_COMPILER_GCC)

#if ECSL_COMPILER_VERSION_CHECK(GE, 4,8)
template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_gcc, os_::value>(std::uint16_t x)
{
    return __builtin_bswap16(x);
}
#endif

#if ECSL_COMPILER_VERSION_CHECK(GE, 4,7,7)
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_gcc, os_::value>(std::uint32_t x)
{
    return __builtin_bswap32(x);
}

template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_gcc, os_::value>(std::uint64_t x)
{
    return __builtin_bswap64(x);
}
#endif

#endif /* ECSL_COMPILER_GCC */

#if defined(ECSL_COMPILER_CLANG)

#if ECSL_COMPILER_VERSION_CHECK(GE, 3,2)
template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_clang, os_::value>(std::uint16_t x)
{
    return __builtin_bswap16(x);
}
#endif

#if ECSL_COMPILER_VERSION_CHECK(GE, 3,0)
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_clang, os_::value>(std::uint32_t x)
{
    return __builtin_bswap32(x);
}

template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_clang, os_::value>(std::uint64_t x)
{
    return __builtin_bswap64(x);
}
#endif

#endif /* ECSL_COMPILER_CLANG */

#if defined(ECSL_COMPILER_ICC)

#if ECSL_COMPILER_VERSION_CHECK(GE, 16,0)
template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_icc, os_::value>(std::uint16_t x)
{
    return __builtin_bswap16(x);
}
#else
template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_icc, os_::value>(std::uint16_t x)
{
    return _bswap16(x);
}
#endif

#if ECSL_COMPILER_VERSION_CHECK(GE, 13,0)
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_icc, os_::value>(std::uint32_t x)
{
    return __builtin_bswap32(x);
}
template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_icc, os_::value>(std::uint64_t x)
{
    return __builtin_bswap64(x);
}
#else
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_icc, os_::value>(std::uint32_t x)
{
    return _bswap(x);
}
template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_icc, os_::value>(std::uint64_t x)
{
    return _bswap64(x);
}
#endif

#endif /* ECSL_COMPILER_ICC */

#if defined(ECSL_COMPILER_MSVC)

template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_msvc, os_::value>(std::uint16_t x)
{
    return _byteswap_ushort(x);
}

template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_msvc, os_::value>(std::uint32_t x)
{
    return _byteswap_ulong(x);
}

template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_msvc, os_::value>(std::uint64_t x)
{
    return _byteswap_uint64(x);
}

#endif /* ECSL_COMPILER_CLANG */

/// OS based intrinsic bswap

#if defined(ECSL_COMPILER_UNKNOWN)
#if defined(__APPLE__)

template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_unknown, os_::os_apple>(std::uint16_t x)
{
    return OSSwapInt16(x);
}
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_unknown, os_::os_apple>(std::uint32_t x)
{
    return OSSwapInt32(x);
}
template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_unknown, os_::os_apple>(std::uint64_t x)
{
    return OSSwapInt64(x);
}

#elif defined(__sun) || defined(sun)

template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_unknown, os_::os_sun>(std::uint16_t x)
{
    return BSWAP_16(x);
}
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_unknown, os_::os_sun>(std::uint32_t x)
{
    return BSWAP_32(x);
}
template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_unknown, os_::os_sun>(std::uint64_t x)
{
    return BSWAP_64(x);
}

#elif defined(__FreeBSD__) || defined(__NetBSD__)

template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_unknown, os_::os_freebsd>(std::uint16_t x)
{
    return bswap16(x);
}
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_unknown, os_::os_freebsd>(std::uint32_t x)
{
    return bswap32(x);
}
template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_unknown, os_::os_freebsd>(std::uint64_t x)
{
    return bswap64(x);
}

#elif defined(__OpenBSD__)

template<>
std::uint16_t intrinsic_bswap<std::uint16_t,
    compiler::type::compiler_unknown, os_::os_openbsd>(std::uint16_t x)
{
    return swap16(x);
}
template<>
std::uint32_t intrinsic_bswap<std::uint32_t,
    compiler::type::compiler_unknown, os_::os_openbsd>(std::uint32_t x)
{
    return swap32(x);
}
template<>
std::uint64_t intrinsic_bswap<std::uint64_t,
    compiler::type::compiler_unknown, os_::os_openbsd>(std::uint64_t x)
{
    return swap64(x);
}

#endif
#endif /* ECSL_COMPILER_UNKNOWN */

} // namespace byteorder
} // namespace detail

template<class T>
T bswap(T x)
{
    return detail::byteorder::intrinsic_bswap(x);
}

template<class T, endianness = endianness::value>
T to_big_endian(T x)
{
    return x;
}

template<class T>
T to_big_endian<T, endianness::little_endian>(T x)
{
    return bswap(x);
}

template<class T, endianness = endianness::value>
T from_big_endian(T x)
{
    return x;
}

template<class T>
T from_big_endian<T, endianness::little_endian>(T x)
{
    return bswap(x);
}

template<class T, endianness = endianness::value>
T to_little_endian(T x)
{
    return x;
}

template<class T>
T to_little_endian<T, endianness::big_endian>(T x)
{
    return bswap(x);
}

template<class T, endianness = endianness::value>
T from_little_endian(T x)
{
    return x;
}

template<class T>
T from_little_endian<T, endianness::big_endian>(T x)
{
    return bswap(x);
}

} // namespace ecsl
#endif /* ECSL_PLATFORM_BYTEORDER_HPP_ */