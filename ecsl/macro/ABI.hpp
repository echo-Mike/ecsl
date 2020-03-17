#ifndef ECSL_MACRO_ABI_HPP_
#define ECSL_MACRO_ABI_HPP_

#include <cstddef>

/**
 * @file ABI.h
 * Implements compile time structure ABI checks
 */

#if !defined(__cplusplus)
#   error This header can only be used in C++ context as it uses static_assert facility of C++
#endif

/**
 * Checks size of field in some struct
 */
#define $abi_member_size(STRUCT, FIELD, SIZE) \
    static_assert(sizeof(STRUCT::FIELD) == SIZE,\
        "ABI failure: The " #STRUCT "::" #FIELD \
        " have size differ from expected: " #SIZE )

/**
 * Checks offset of field in some struct
 */
#define $abi_member_offset(STRUCT, FIELD, OFFSET) \
    static_assert(offsetof(STRUCT, FIELD) == OFFSET,\
        "ABI failure: The " #STRUCT "::" #FIELD \
        " is not placed at expected offset: " #OFFSET )

/**
 * Checks alignment of field in some struct
 */
#define $abi_member_align(STRUCT, FIELD, ALIGNMENT) \
    static_assert(alignof(STRUCT::FIELD) == ALIGNMENT,\
        "ABI failure: The " #STRUCT "::" #FIELD \
        " have alignment differ from expected: " #ALIGNMENT )

/**
 * Checks size and offset of field in some struct
 */
#define $abi_member_size_offset(STRUCT, FIELD, SIZE, OFFSET) \
    $abi_member_size(STRUCT, FIELD, SIZE);\
    $abi_member_offset(STRUCT, FIELD, OFFSET)

/**
 * Checks size, offset and alignment of field in some struct
 */
#define $abi_member_size_offset_align(STRUCT, FIELD, SIZE, OFFSET, ALIGNMENT) \
    $abi_member_size_offset(STRUCT, FIELD, SIZE, OFFSET); \
    $abi_member_align(STRUCT, FIELD, ALIGNMENT)

/**
 * Checks size of entity
 */
#define $abi_size(NAME, SIZE) \
    static_assert(sizeof(NAME) == SIZE,\
        "ABI failure: The " #NAME \
        " have size differ from expected: " #SIZE )

/**
 * Checks alignment of entity
 */
#define $abi_align(NAME, ALIGNMENT) \
    static_assert(alignof(NAME) == ALIGNMENT,\
        "ABI failure: The " #NAME \
        " have alignment differ from expected: " #ALIGNMENT )

/**
 * Checks size and alignment of entity
 */
#define $abi_size_align(NAME, SIZE, ALIGNMENT) \
    $abi_size(NAME, SIZE); \
    $abi_align(NAME, ALIGNMENT)

/**
 * Unused macro
 */
#define $abi_invalid_1_(_1) \
    static_assert(0, "Invalid count of arguments in $abi macro")

/**
 * Derive macro name from argument count for 5 arguments
 */
#define $abi_get_macro_5_(_1, _2, _3, _4, _5, name, ...) name

/**
 * Multiple tests compressed to single macro
 * 1 argument: invalid
 * 2 arguments: 1-st name, 2-nd sizeof(name)
 * 3 arguments: 1-st struct name, 2-nd member name, 3-rd offsetof member in struct
 * 4 arguments: 1-st struct name, 2-nd member name,
 *  3-rd sizeof member, 4-th offsetof member in struct
 * 5 arguments: 1-st struct name, 2-nd member name,
 *  3-rd sizeof member, 4-th offsetof member in struct, 5-th alignment of member
 */
#define $abi(...) $abi_get_macro_5_(__VA_ARGS__,\
    $abi_member_size_offset_align,\
    $abi_member_size_offset,\
    $abi_member_offset,\
    $abi_size,\
    $abi_invalid_1_,\
    )(__VA_ARGS__)

#endif /* ECSL_MACRO_ABI_HPP_ */