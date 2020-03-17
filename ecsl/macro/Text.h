#ifndef ECSL_MACRO_TEXT_H_
#define ECSL_MACRO_TEXT_H_

/**
 * @file Text.h
 * C strings generation and codebase manipulation
 */

/// ECSL
#include <ecsl/macro/Core.h>

#if defined($no)

/// Text modifiers

// Quotes provided arguments
#define $quote(...) #__VA_ARGS__
// Stringifies macro like __LINE__ with expansion (for "line_number")
#define $stringify(...) $yes($quote(__VA_ARGS__))
// Makes string in quotes: $json_s(any string) -> "\"any string\""
#define $json_str(...) $stringify($quote(__VA_ARGS__))
/**
 * Returns compiler defined string containing file and line information.
 * @note Uses __LINE__ macro.
 */
#define $fileline __FILE__ ":" $stringify(__LINE__) ":"

/// Code modifiers

// Comments part of code
#define $comment $no
// Complimentary for $comment
#define $uncomment $yes

#endif // $no
#endif /* ECSL_MACRO_TEXT_H_ */