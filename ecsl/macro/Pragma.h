#ifndef ECSL_MACRO_PRAGMA_H_
#define ECSL_MACRO_PRAGMA_H_

/**
 * @file Pragma.h
 * Defines macro that can execute compiler defined pragmas
 */

/// ECSL
#include <ecsl/macro/Core.h>
#include <ecsl/macro/Compiler.h>

#if defined($no)

#if $on($gcc) || $on($clang) || $on($icc)
#   define $do_pragma(x) _Pragma(#x)
#elif $on($msvc)
#   define $do_pragma(x) __pragma(x)
#else
#   define $do_pragma(x)
#   pragma message ("$do_pragma macro is unavailable")
#endif

/**
 * @def $do_pragma(x)
 * On known compilers executes given pragma. Useful for other $* macro
 */

#endif // $no
#endif /* ECSL_MACRO_PRAGMA_H_ */