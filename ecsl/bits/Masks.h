#ifndef ECSL_BITS_MASKS_H_
#define ECSL_BITS_MASKS_H_

/**
 * @file Masks.h
 * Macro definitions for binary constants for masks upto 64-bits
 *
 * Any ECSL_MASK_N masks first N less significant bits.
 * Convenience macros ECSL_MASK(count)
 * and ECSL_MASK_POS(count, pos) are provided
 */

/// BYTE

#define ECSL_MASK_0 ((1 << 0) - 1)
#define ECSL_MASK_1 ((1 << 1) - 1)
#define ECSL_MASK_2 ((1 << 2) - 1)
#define ECSL_MASK_3 ((1 << 3) - 1)
#define ECSL_MASK_4 ((1 << 4) - 1)
#define ECSL_MASK_5 ((1 << 5) - 1)
#define ECSL_MASK_6 ((1 << 6) - 1)
#define ECSL_MASK_7 ((1 << 7) - 1)
#define ECSL_MASK_8 ((1 << 8) - 1)

/// WORD

#define ECSL_MASK_9  ((1 <<  9) - 1)
#define ECSL_MASK_10 ((1 << 10) - 1)
#define ECSL_MASK_11 ((1 << 11) - 1)
#define ECSL_MASK_12 ((1 << 12) - 1)
#define ECSL_MASK_13 ((1 << 13) - 1)
#define ECSL_MASK_14 ((1 << 14) - 1)
#define ECSL_MASK_15 ((1 << 15) - 1)
#define ECSL_MASK_16 ((1 << 16) - 1)

/// DOUBLE WORD (DWORD)

#define ECSL_MASK_17 ((1 << 17) - 1)
#define ECSL_MASK_18 ((1 << 18) - 1)
#define ECSL_MASK_19 ((1 << 19) - 1)
#define ECSL_MASK_20 ((1 << 20) - 1)
#define ECSL_MASK_21 ((1 << 21) - 1)
#define ECSL_MASK_22 ((1 << 22) - 1)
#define ECSL_MASK_23 ((1 << 23) - 1)
#define ECSL_MASK_24 ((1 << 24) - 1)
#define ECSL_MASK_25 ((1 << 25) - 1)
#define ECSL_MASK_26 ((1 << 26) - 1)
#define ECSL_MASK_27 ((1 << 27) - 1)
#define ECSL_MASK_28 ((1 << 28) - 1)
#define ECSL_MASK_29 ((1 << 29) - 1)
#define ECSL_MASK_30 ((1 << 30) - 1)
#define ECSL_MASK_31 ((1u << 31) - 1)
#define ECSL_MASK_32 (0u - 1)

/// QUADRUPLE WORD (QWORD)

#define ECSL_MASK_33 ((1ll << 33) - 1)
#define ECSL_MASK_34 ((1ll << 34) - 1)
#define ECSL_MASK_35 ((1ll << 35) - 1)
#define ECSL_MASK_36 ((1ll << 36) - 1)
#define ECSL_MASK_37 ((1ll << 37) - 1)
#define ECSL_MASK_38 ((1ll << 38) - 1)
#define ECSL_MASK_39 ((1ll << 39) - 1)
#define ECSL_MASK_40 ((1ll << 40) - 1)
#define ECSL_MASK_41 ((1ll << 41) - 1)
#define ECSL_MASK_42 ((1ll << 42) - 1)
#define ECSL_MASK_43 ((1ll << 43) - 1)
#define ECSL_MASK_44 ((1ll << 44) - 1)
#define ECSL_MASK_45 ((1ll << 45) - 1)
#define ECSL_MASK_46 ((1ll << 46) - 1)
#define ECSL_MASK_47 ((1ll << 47) - 1)
#define ECSL_MASK_48 ((1ll << 48) - 1)
#define ECSL_MASK_49 ((1ll << 49) - 1)
#define ECSL_MASK_50 ((1ll << 50) - 1)
#define ECSL_MASK_51 ((1ll << 51) - 1)
#define ECSL_MASK_52 ((1ll << 52) - 1)
#define ECSL_MASK_53 ((1ll << 53) - 1)
#define ECSL_MASK_54 ((1ll << 54) - 1)
#define ECSL_MASK_55 ((1ll << 55) - 1)
#define ECSL_MASK_56 ((1ll << 56) - 1)
#define ECSL_MASK_57 ((1ll << 57) - 1)
#define ECSL_MASK_58 ((1ll << 58) - 1)
#define ECSL_MASK_59 ((1ll << 59) - 1)
#define ECSL_MASK_60 ((1ll << 60) - 1)
#define ECSL_MASK_61 ((1ll << 61) - 1)
#define ECSL_MASK_62 ((1ll << 62) - 1)
#define ECSL_MASK_63 ((1ull << 63) - 1)
#define ECSL_MASK_64 (0ull - 1)

/**
 * Masks first <count> less significant bits.
 * <count> must be an integer literal without uU/lL/llLL in range [0:64]
 */
#define ECSL_MASK(count) ECSL_MASK_##count

/**
 * Masks first <count> bits towards most significant bits
 * starting with bit in <pos> position (counted from 0).
 * <count> must be an integer literal without uU/lL/llLL in range [0:64]
 */
#define ECSL_MASK_POS(count, pos) ((ECSL_MASK(count)) << (pos))

/**
 * This macro generates mask constant of type unsigned long long
 * <count> may be any number but anything above 64 is expected to generate ~0ull
 */
#define ECSL_MASK_ULL(count) ((1ull << count) - 1)

#if defined(__cplusplus)
// This macro generates mask constant of type TYPE
#   define ECSL_MASK_TYPED(count, TYPE) ((static_cast<TYPE>(1) << count) - 1)
#else
// This macro generates mask constant of type TYPE
#   define ECSL_MASK_TYPED(count, TYPE) ((((TYPE)(1)) << count) - 1)
#endif /* __cplusplus */

#endif /* ECSL_BITS_MASKS_H_ */