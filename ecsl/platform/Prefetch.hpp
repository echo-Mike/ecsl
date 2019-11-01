#ifndef ECSL_PLATFORM_PREFETCH_HPP_
#define ECSL_PLATFORM_PREFETCH_HPP_

/**
 * @file Prefetch.h
 * Allows to prefetch data into different cache levels for different purposes
 *
 * Sources:
 *  https://msdn.microsoft.com/ru-ru/library/hh977022.aspx
 *  https://msdn.microsoft.com/ru-ru/library/hh875058.aspx
 *  https://msdn.microsoft.com/ru-ru/library/b0084kay.aspx
 *  https://msdn.microsoft.com/ru-ru/library/hh977023.aspx
 *  https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 *  https://stackoverflow.com/questions/46521694/what-are-mm-prefetch-locality-hints
 *
 * Tested on:
 *  gcc (x86_64) >= 4.1.2
 *  clang (x86_64) >= 3.0.0
 *  msvc (x86_64, x64, ARM) 2015 v19.0 (1900)
 *  icc (x86_64) >= 13.0.1
 */

/// ECSL
#include <ecsl/platform/Compiler.hpp>
/// Intrinsics
#if defined(ECSL_COMPILER_MSVC)
#   include <intrin.h>
#endif

namespace ecsl {
namespace detail {
namespace prefetch {
enum state : int
{
    read = 0,
    modify = 1,
};

enum level : int
{
#if defined(ECSL_COMPILER_MSVC) || defined(ECSL_COMPILER_UNKNOWN)
    L0 = 1,
    L1 = 2,
    L2 = 3,
    NT = 0,
#else
    L0 = 3,
    L1 = 2,
    L2 = 1,
    NT = 0,
#endif
};

#if defined(ECSL_COMPILER_MSVC)
template<state s, level l>
void prefetch_impl(const void* ptr)
{
#   if defined(_M_ARM) || defined(_M_ARM64)
    __prefetch(ptr);
#   else
    _mm_prefetch(ptr, static_cast<int>(l));
#endif
}
#elif defined(ECSL_COMPILER_UNKNOWN)
template<state, level> void prefetch_impl(const void*) {}
#else
template<state s, level l>
void prefetch_impl(const void* ptr)
{
    __builtin_prefetch(ptr, static_cast<int>(s), static_cast<int>(l));
}
#endif

} // namespace prefetch
} // namespace detail

namespace prefetch {

template<detail::prefetch::state s, detail::prefetch::level l>
void any(const void* ptr)
{
    detail::prefetch::prefetch_impl<s, l>(ptr);
}
/**
 * Prefetch a cache line in SHARED state into all cache levels
 * @param ptr Address to prefetch
 */
inline void l0_r(const void* ptr)
{
    any<detail::prefetch::read, detail::prefetch::L0>(ptr);
}
/**
 * Prefetch a cache line in SHARED state into all cache
 * levels except the 0th cache level
 * @param ptr Address to prefetch
 */
inline void l1_r(const void* ptr)
{
    any<detail::prefetch::read, detail::prefetch::L1>(ptr);
}
/**
 * Prefetch a cache line in SHARED state into all cache
 * levels except the 0th and 1th cache levels.
 * @param ptr Address to prefetch
 */
inline void l2_r(const void* ptr)
{
    any<detail::prefetch::read, detail::prefetch::L2>(ptr);
}
/**
 * Prefetch a cache line in SHARED state into all cache levels (non-temporal/transient version).
 * The non-temporal prefetch is intended as a prefetch hint that processor will
 * use the prefetched data only once or short period, unlike the
 * l0_r function which imply that prefetched data to use repeatedly.
 * @param ptr Address to prefetch
 */
inline void nt_r(const void* ptr)
{
    any<detail::prefetch::read, detail::prefetch::NT>(ptr);
}
/**
 * Prefetch a cache line in EXCLUSIVE state into all cache levels
 * @param ptr Address to prefetch
 */
inline void l0_m(const void* ptr)
{
    any<detail::prefetch::modify, detail::prefetch::L0>(ptr);
}
/**
 * Prefetch a cache line in EXCLUSIVE state into all cache
 * levels except the 0th cache level
 * @param ptr Address to prefetch
 */
inline void l1_m(const void* ptr)
{
    any<detail::prefetch::modify, detail::prefetch::L1>(ptr);
}
/**
 * Prefetch a cache line in EXCLUSIVE state into all cache
 * levels except the 0th and 1th cache levels.
 * @param ptr Address to prefetch
 */
inline void l2_m(const void* ptr)
{
    any<detail::prefetch::modify, detail::prefetch::L2>(ptr);
}
/**
 * Prefetch a cache line in EXCLUSIVE state into all cache levels (non-temporal/transient version).
 * The non-temporal prefetch is intended as a prefetch hint that processor will
 * use the prefetched data only once or short period, unlike the
 * l0_r function which imply that prefetched data to use repeatedly.
 * @param ptr Address to prefetch
 */
inline void nt_m(const void* ptr)
{
    any<detail::prefetch::modify, detail::prefetch::NT>(ptr);
}
/**
 * Default prefetch operation to L0
 * @param ptr Address to prefetch
 */
inline void l0(const void* ptr)
{
    l0_m(ptr);
}
/**
 * Default prefetch operation to L1
 * @param ptr Address to prefetch
 */
inline void l1(const void* ptr)
{
    l1_m(ptr);
}
/**
 * Default prefetch operation to L2
 * @param ptr Address to prefetch
 */
inline void l2(const void* ptr)
{
    l2_m(ptr);
}
/**
 * Default operation for non-temporal prefetch
 * @param ptr Address to prefetch
 */
inline void nt(const void* ptr)
{
    nt(ptr);
}

} // namespace prefetch
} // namespace ecsl
#endif /* ECSL_PLATFORM_PREFETCH_HPP_ */