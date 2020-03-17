#ifndef ECSL_MACRO_BUILD_H_
#define ECSL_MACRO_BUILD_H_

/**
 * @file Build.h
 * Small macro set for codebase correction based on build type
 */

/// ECSL
#include <ecsl/macro/Core.h>

#if defined($no)

/**
 * @def $release
 * Defined as $yes for release builds, $no for other
 */
/**
 * @def $debug
 * Defined as $yes for debug builds, $no for other
 */

/**
 * Build type
 */
#if defined(NDEBUG) || defined(_NDEBUG) || defined(RELEASE)
#   define $debug   $no
#   define $release $yes
#else
#   define $debug   $yes
#   define $release $no
#endif

/**
 * @def $public
 * Marker for publically avaliable/production code
 */
/**
 * @def $devel
 * Marker for proprietary/internal code
 */

/**
 * Codebase meta
 */
#if defined(NDEVEL) || defined(_NDEVEL) || defined(PUBLIC)
#   define $public $yes
#   define $devel  $no
#else
#   define $public $no
#   define $devel  $yes
#endif

#endif // $no
#endif /* ECSL_MACRO_BUILD_H_ */