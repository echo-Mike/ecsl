#ifndef ECSL_BITS_CONSTANTS_H_
#define ECSL_BITS_CONSTANTS_H_

/**
 * @file Constants.h
 * Macro definitions for binary constants for bits upto 64-bits
 *
 * Any ECSL_BIT_N masks N'th less significant bit.
 * Convenience macros ECSL_BIT(bit) is provided
 */

// This value is a special case for no bit set
#define ECSL_BIT_NUL ( 0 )

/// BYTE

#define ECSL_BIT_0 (1 << 0)
#define ECSL_BIT_1 (1 << 1)
#define ECSL_BIT_2 (1 << 2)
#define ECSL_BIT_3 (1 << 3)
#define ECSL_BIT_4 (1 << 4)
#define ECSL_BIT_5 (1 << 5)
#define ECSL_BIT_6 (1 << 6)
#define ECSL_BIT_7 (1u << 7)

/// WORD

#define ECSL_BIT_8  (1 << 8)
#define ECSL_BIT_9  (1 << 9)
#define ECSL_BIT_10 (1 << 10)
#define ECSL_BIT_11 (1 << 11)
#define ECSL_BIT_12 (1 << 12)
#define ECSL_BIT_13 (1 << 13)
#define ECSL_BIT_14 (1 << 14)
#define ECSL_BIT_15 (1u << 15)

// DOUBLE WORD (DWORD)

#define ECSL_BIT_16 (1 << 16)
#define ECSL_BIT_17 (1 << 17)
#define ECSL_BIT_18 (1 << 18)
#define ECSL_BIT_19 (1 << 19)
#define ECSL_BIT_20 (1 << 20)
#define ECSL_BIT_21 (1 << 21)
#define ECSL_BIT_22 (1 << 22)
#define ECSL_BIT_23 (1 << 23)
#define ECSL_BIT_24 (1 << 24)
#define ECSL_BIT_25 (1 << 25)
#define ECSL_BIT_26 (1 << 26)
#define ECSL_BIT_27 (1 << 27)
#define ECSL_BIT_28 (1 << 28)
#define ECSL_BIT_29 (1 << 29)
#define ECSL_BIT_30 (1 << 30)
#define ECSL_BIT_31 (1u << 31)

// QUADRUPLE WORD (QWORD)

#define ECSL_BIT_32 (1ll << 32)
#define ECSL_BIT_33 (1ll << 33)
#define ECSL_BIT_34 (1ll << 34)
#define ECSL_BIT_35 (1ll << 35)
#define ECSL_BIT_36 (1ll << 36)
#define ECSL_BIT_37 (1ll << 37)
#define ECSL_BIT_38 (1ll << 38)
#define ECSL_BIT_39 (1ll << 39)
#define ECSL_BIT_40 (1ll << 40)
#define ECSL_BIT_41 (1ll << 41)
#define ECSL_BIT_42 (1ll << 42)
#define ECSL_BIT_43 (1ll << 43)
#define ECSL_BIT_44 (1ll << 44)
#define ECSL_BIT_45 (1ll << 45)
#define ECSL_BIT_46 (1ll << 46)
#define ECSL_BIT_47 (1ll << 47)
#define ECSL_BIT_48 (1ll << 48)
#define ECSL_BIT_49 (1ll << 49)
#define ECSL_BIT_50 (1ll << 50)
#define ECSL_BIT_51 (1ll << 51)
#define ECSL_BIT_52 (1ll << 52)
#define ECSL_BIT_53 (1ll << 53)
#define ECSL_BIT_54 (1ll << 54)
#define ECSL_BIT_55 (1ll << 55)
#define ECSL_BIT_56 (1ll << 56)
#define ECSL_BIT_57 (1ll << 57)
#define ECSL_BIT_58 (1ll << 58)
#define ECSL_BIT_59 (1ll << 59)
#define ECSL_BIT_60 (1ll << 60)
#define ECSL_BIT_61 (1ll << 61)
#define ECSL_BIT_62 (1ll << 62)
#define ECSL_BIT_63 (1ull << 63)

// <bit> must be an integer literal without uU/lL/llLL in range [0:63] or NUL
#define ECSL_BIT(bit) ECSL_BIT_##bit

/**
 * This macro generates bit constant of type unsigned long long
 * <bit> may be any number but anything above 63 is expected to generate 0
 */
#define ECSL_BIT_ULL(bit) (1ull << bit)

#if defined(__cplusplus)
// This macro generates bit constant of type TYPE
#   define ECSL_BIT_TYPED(bit, TYPE) (static_cast<TYPE>(1) << bit)
#else
// This macro generates bit constant of type TYPE
#   define ECSL_BIT_TYPED(bit, TYPE) (((TYPE)(1)) << bit)
#endif /* __cplusplus */

#endif /* ECSL_BITS_CONSTANTS_H_ */