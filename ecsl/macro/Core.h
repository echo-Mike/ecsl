#ifndef ECSL_MACRO_CORE_H_
#define ECSL_MACRO_CORE_H_

/**
 * @file Core.h
 * Core code of Pretty Macro framework.
 * Initial implementation is borrowed from:
 * https://github.com/r-lyeh/tinybits/blob/master/tinywtf.h
 */

#if !defined($no)

/// Core

// Suppresses all it's arguments
#define $no(...)
#define $negated_$no(...) __VA_ARGS__
// Places all it's arguments
#define $yes(...) __VA_ARGS__
#define $negated_$yes(...)

/// Macro manipulations

#define $empty()
// Makes a deferred macro expansion on macro M and given arguments
#define $defer(M) M $empty()
// Essential macro for macro expansion routine on some platforms (MSVC)
#define $expand(...) __VA_ARGS__
// Merges to tokens into one
#define $merge(x, y) x ## y
/**
 * Inverses value of $* macro (only that defined with $yes/$no)
 * May be used where $not* version of macro is not provided.
 */
#define $neg(x) $merge($negated_, x)

/// Directive modifiers

// Usage: #if $on($clang) or if ($on($clang)) { code }
#define $on(v) (v(1) + 0)
// Usage: #if $is($debug) or if ($is($debug)) { code }
#define $is(v) (v(1) + 0)
// Usage: #if $not($debug) or if ($not($debug)) { code }
#define $not(v) (v(-1) + 1)
/**
 * Yields if clause with condition based on macro expansion
 * Usage:
 *  $if($debug) { code on debug } else { code on not debug }
 */
#define $if(v) if ($is(v))
/**
 * Checks opposite of macro
 * Usage:
 *  $ifn($debug) { code on not debug } else { code on debug }
 */
#define $ifn(v) if ($not(v))
/**
 * Conditianally returns:
 *  y - if x is defined as $yes and
 *  n - if x is defined as $no
 */
#define $cond(m, y, n) m(y) $neg(m)(n)

// Used when something can be deduced.
#define $known $yes
// Used when something can't be deduced.
#define $unknown $no

#endif // ! $no
#endif /* ECSL_MACRO_CORE_H_ */