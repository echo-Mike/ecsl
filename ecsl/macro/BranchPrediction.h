#ifndef ECSL_MACRO_BRANCH_PREDICTION_H_
#define ECSL_MACRO_BRANCH_PREDICTION_H_

/**
 * @file BranchPrediction.h
 * Allows for programmer to specify branches that is
 * likely or unlikely to be taken during program execution
 */

/// ECSL
#include <ecsl/macro/Compiler.h>

#if defined($no)

// Specifies that following branch is likely to be a hot path
#define $likely(x) $cond($msvc, (x), __builtin_expect(!!(x), 1))
// Specifies that following branch is likely to be a cold path
#define $unlikely(x) $cond($msvc, (x), __builtin_expect(!!(x), 0))

#endif // $no
#endif /* ECSL_MACRO_BRANCH_PREDICTION_H_ */