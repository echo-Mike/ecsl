#ifndef ECSL_MACRO_GNU_WARNINGS_H_
#define ECSL_MACRO_GNU_WARNINGS_H_

/**
 * @file GnuWarnings.h
 * Defines macro set to manipulate warning and error diagnostics on GNU.
 */

/// ECSL
#include <ecsl/macro/Core.h>
#include <ecsl/macro/Compiler.h>
#include <ecsl/macro/Pragma.h>

#if defined($no)

/**
 * Warning manipulation
 * ! $e2i macro is not defined deliberately
 */

/**
 * Saves the state of compiler diagnostics on GNU compatible compilers
 * GNU only.
 */
#define $gnu_diagnostic_push_ \
    $notmsvc($do_pragma(GCC diagnostic push))
/**
 * Restores last saved state of compiler diagnostics on GNU compatible compilers
 * GNU only.
 */
#define $gnu_diagnostic_pop_ \
    $notmsvc($do_pragma(GCC diagnostic pop))

/**
 * After this macro specified error is suppressed to warning level.
 * GNU only.
 * X - one of -W* flags see: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#define $more_e2w(X) \
    $notmsvc($do_pragma(GCC diagnostic warning #X))
/**
 * After this macro specified error is suppressed to warning level.
 * @warning $diagnose must be used after section where error is suppressed
 * GNU only.
 * X - one of -W* flags see: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#define $e2w(X) \
    $gnu_diagnostic_push_ \
    $more_e2w(X)

/**
 * After this macro specified warning is promoted to error level.
 * GNU only.
 * X - one of -W* flags see: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#define $more_w2e(X) \
    $notmsvc($do_pragma(GCC diagnostic error #X))
/**
 * After this macro specified warning is promoted to error level.
 * @warning $diagnose must be used after section where warning is promoted
 * GNU only.
 * X - one of -W* flags see: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#define $w2e(X) \
    $gnu_diagnostic_push_ \
    $more_w2e(X)

/**
 * After this macro specified warning is suppressed.
 * GNU only.
 * X - one of -W* flags see: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#define $more_w2i(X) \
    $notmsvc($do_pragma(GCC diagnostic ignored #X))
/**
 * After this macro specified warning is suppressed.
 * @warning $diagnose must be used after section where warning is promoted
 * GNU only.
 * X - one of -W* flags see: https://gcc.gnu.org/onlinedocs/gcc/Diagnostic-Pragmas.html
 */
#define $w2i(X) \
    $gnu_diagnostic_push_ \
    $more_w2i(X)

/**
 * Restores diagnostics settings after $w2i, $w2e and $e2w.
 * GNU only.
 * Must be used pairvise with usage of $w2i, $w2e and $e2w:
 *  $w2i(-Warning0)
 *  $w2i(-Warning1)
 *  ... Code with Warning0 and Warning1 ...
 *  $diagnose // restores after -Warning1
 *  $diagnose // restores after -Warning0
 * Other use case with $more_* macro family:
 *  $w2i(-Warning0)
 *  $more_w2i(-Warning1)
 *  $more_w2e(-Warning2)
 *  ... Code with Warning0, Warning1 and Warning2 ...
 *  $diagnose // restores after -Warning0, -Warning1 and -Warning2
 */
#define $diagnose $gnu_diagnostic_pop_

#endif // $no
#endif /* ECSL_MACRO_GNU_WARNINGS_H_ */