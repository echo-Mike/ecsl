#ifndef ECSL_BITS_STORAGE_H_
#define ECSL_BITS_STORAGE_H_

/**
 * @file Storage.h
 * Macro definitions for binary constants for storage
 *
 * Binary sizes are named according to SI (a.e. kibi/mimi/gibi etc.).
 * Decimal sizes are named with *_BYTE on end
 */

#include <ecsl/bits/Constants.h>

/// BINARY

// BYTES

#define ECSL_0B   ECSL_BIT_NUL
#define ECSL_1B   ECSL_BIT_0
#define ECSL_2B   ECSL_BIT_1
#define ECSL_4B   ECSL_BIT_2
#define ECSL_8B   ECSL_BIT_3
#define ECSL_16B  ECSL_BIT_4
#define ECSL_32B  ECSL_BIT_5
#define ECSL_64B  ECSL_BIT_6
#define ECSL_128B ECSL_BIT_7
#define ECSL_256B ECSL_BIT_8
#define ECSL_512B ECSL_BIT_9

// KIBI BYTES (1024)

#define ECSL_1KiB   ECSL_BIT_10
#define ECSL_2KiB   ECSL_BIT_11
#define ECSL_4KiB   ECSL_BIT_12
#define ECSL_8KiB   ECSL_BIT_13
#define ECSL_16KiB  ECSL_BIT_14
#define ECSL_32KiB  ECSL_BIT_15
#define ECSL_64KiB  ECSL_BIT_16
#define ECSL_128KiB ECSL_BIT_17
#define ECSL_256KiB ECSL_BIT_18
#define ECSL_512KiB ECSL_BIT_19

// MEBI BYTES (1024^2)

#define ECSL_1MiB   ECSL_BIT_20
#define ECSL_2MiB   ECSL_BIT_21
#define ECSL_4MiB   ECSL_BIT_22
#define ECSL_8MiB   ECSL_BIT_23
#define ECSL_16MiB  ECSL_BIT_24
#define ECSL_32MiB  ECSL_BIT_25
#define ECSL_64MiB  ECSL_BIT_26
#define ECSL_128MiB ECSL_BIT_27
#define ECSL_256MiB ECSL_BIT_28
#define ECSL_512MiB ECSL_BIT_29

// GIBI BYTES (1024^3)

#define ECSL_1GiB   ECSL_BIT_30
#define ECSL_2GiB   ECSL_BIT_31
#define ECSL_4GiB   ECSL_BIT_32
#define ECSL_8GiB   ECSL_BIT_33
#define ECSL_16GiB  ECSL_BIT_34
#define ECSL_32GiB  ECSL_BIT_35
#define ECSL_64GiB  ECSL_BIT_36
#define ECSL_128GiB ECSL_BIT_37
#define ECSL_256GiB ECSL_BIT_38
#define ECSL_512GiB ECSL_BIT_39

// TEBI BYTES (1024^4)

#define ECSL_1TiB   ECSL_BIT_40
#define ECSL_2TiB   ECSL_BIT_41
#define ECSL_4TiB   ECSL_BIT_42
#define ECSL_8TiB   ECSL_BIT_43
#define ECSL_16TiB  ECSL_BIT_44
#define ECSL_32TiB  ECSL_BIT_45
#define ECSL_64TiB  ECSL_BIT_46
#define ECSL_128TiB ECSL_BIT_47
#define ECSL_256TiB ECSL_BIT_48
#define ECSL_512TiB ECSL_BIT_49

// PEBI BYTES (1024^5)

#define ECSL_1PiB   ECSL_BIT_50
#define ECSL_2PiB   ECSL_BIT_51
#define ECSL_4PiB   ECSL_BIT_52
#define ECSL_8PiB   ECSL_BIT_53
#define ECSL_16PiB  ECSL_BIT_54
#define ECSL_32PiB  ECSL_BIT_55
#define ECSL_64PiB  ECSL_BIT_56
#define ECSL_128PiB ECSL_BIT_57
#define ECSL_256PiB ECSL_BIT_58
#define ECSL_512PiB ECSL_BIT_59

// EXBI BYTES (1024^6)

#define ECSL_1EiB ECSL_BIT_60
#define ECSL_2EiB ECSL_BIT_61
#define ECSL_4EiB ECSL_BIT_62
#define ECSL_8EiB ECSL_BIT_63

/// DECIMAL

// 10^3
#define ECSL_1K_BYTE (0x3e8)
// 10^6
#define ECSL_1M_BYTE (0xf4240)
// 10^9
#define ECSL_1G_BYTE (0x3b9aca00)
// 10^12
#define ECSL_1T_BYTE (0xe8d4a51000ll)
// 10^15
#define ECSL_1P_BYTE (0x38d7ea4c68000ll)
// 10^18
#define ECSL_1E_BYTE (0xde0b6b3a7640000ll)

#endif /* ECSL_BITS_STORAGE_H_ */