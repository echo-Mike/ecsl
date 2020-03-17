#ifndef ECSL_MACRO_COMPILER_H_
#define ECSL_MACRO_COMPILER_H_

/**
 * @file Compiler.h
 * Codebase correction based on compiler type
 */

/// ECSL
#include <ecsl/platform/Compiler.hpp>
#include <ecsl/macro/Core.h>

#if defined($no)

#if defined (ECSL_COMPILER_CLANG)
#   define $clang    $yes
#elif defined (ECSL_COMPILER_MSVC)
#   define $msvc     $yes
#elif defined (ECSL_COMPILER_GCC)
#   define $gcc      $yes
#elif defined (ECSL_COMPILER_ICC)
#   define $icc      $yes
#else
#   define $compiler $unknown
#endif

#if !defined($clang)
#   define $clang    $no
#   define $notclang $yes
#else
#   define $notclang $no
#endif
/**
 * @def $clang
 * Defined as $yes if compiler is clang
 */
/**
 * @def $notclang
 * Defined as $yes if compiler is not clang
 */

#if !defined($msvc)
#   define $msvc     $no
#   define $notmsvc  $yes
#else
#   define $notmsvc  $no
#endif
/**
 * @def $msvc
 * Defined as $yes if compiler is msvc
 */
/**
 * @def $notmsvc
 * Defined as $yes if compiler is not msvc
 */

#if !defined($gcc)
#   define $gcc      $no
#   define $notgcc   $yes
#else
#   define $notgcc   $no
#endif
/**
 * @def $gcc
 * Defined as $yes if compiler is gcc
 */
/**
 * @def $notgcc
 * Defined as $yes if compiler is not gcc
 */

#if !defined($icc)
#   define $icc      $no
#   define $noticc   $yes
#else
#   define $noticc   $no
#endif
/**
 * @def $icc
 * Defined as $yes if compiler is icc
 */
/**
 * @def $noticc
 * Defined as $yes if compiler is not icc
 */

#if !defined($compiler)
#   define $compiler $known
#endif
/**
 * @def $compiler
 * Defined as $yes if compiler is known
 */

#endif // $no
#endif /* ECSL_MACRO_COMPILER_H_ */