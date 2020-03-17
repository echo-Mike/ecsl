#ifndef ECSL_PLATFORM_COMPILER_HPP_
#define ECSL_PLATFORM_COMPILER_HPP_

/**
 * @file Compiler.hpp
 * Detects compiler used
 *
 * Supported compilers: clang, gcc, msvc, icc.
 *
 * Defines the compiler name as ECSL_COMPILER_NAME
 * and compier version as ECSL_COMPILER_VERSION in format
 * of decimal encoded version (2 decimal places for
 * each version point): major * 10000 + minor * 100 + patch
 *
 * Also the ECSL_COMPILER_* with compiler name in
 * place of * is defined
 */

#define ECSL_COMPILER_ENCODE_VERSION_3_(major_, minor_, patch_) \
    (major_ * 10000 + minor_ * 100 + patch_)
#define ECSL_COMPILER_ENCODE_VERSION_2_(major_, minor_) \
    (major_ * 10000 + minor_ * 100)
#define ECSL_COMPILER_ENCODE_VERSION_1_(...) \
    __VA_ARGS__

#define ECSL_COMPILER_GET_MACRO_3_(_1, _2, _3, M, ...) M
#define ECSL_COMPILER_EXPAND_(...) __VA_ARGS__
#define ECSL_COMPILER_ENCODE_VERSION_(...)              \
    ECSL_COMPILER_EXPAND_(ECSL_COMPILER_GET_MACRO_3_(   \
        __VA_ARGS__,                                    \
        ECSL_COMPILER_ENCODE_VERSION_3_,                \
        ECSL_COMPILER_ENCODE_VERSION_2_,                \
        ECSL_COMPILER_ENCODE_VERSION_1_                 \
    )(__VA_ARGS__))

#define ECSL_COMPILER_VERSION_CHECK_EQ_(...) \
    ECSL_COMPILER_VERSION == ECSL_COMPILER_ENCODE_VERSION_(__VA_ARGS__)
#define ECSL_COMPILER_VERSION_CHECK_LE_(...) \
    ECSL_COMPILER_VERSION <= ECSL_COMPILER_ENCODE_VERSION_(__VA_ARGS__)
#define ECSL_COMPILER_VERSION_CHECK_LT_(...) \
    ECSL_COMPILER_VERSION <  ECSL_COMPILER_ENCODE_VERSION_(__VA_ARGS__)
#define ECSL_COMPILER_VERSION_CHECK_GT_(...) \
    ECSL_COMPILER_VERSION  > ECSL_COMPILER_ENCODE_VERSION_(__VA_ARGS__)
#define ECSL_COMPILER_VERSION_CHECK_GE_(...) \
    ECSL_COMPILER_VERSION >= ECSL_COMPILER_ENCODE_VERSION_(__VA_ARGS__)
#define ECSL_COMPILER_VERSION_CHECK_NE_(...) \
    ECSL_COMPILER_VERSION != ECSL_COMPILER_ENCODE_VERSION_(__VA_ARGS__)

/**
 * Checks current compiler version against the provided.
 * OPERATION: EQ, LE, LT, GT, GE, NE
 * Expected 2 or 3 arguments: major, minor [, patch]
 */
#define ECSL_COMPILER_VERSION_CHECK(OPERATION, ...) \
    ECSL_COMPILER_VERSION_CHECK_ ## OPERATION ## _(__VA_ARGS__)

#if defined(__clang__)
/**
 * Compiler: clang
 * Sources:
 *  https://clang.llvm.org/cxx_status.html
 *  https://trac.macports.org/wiki/XcodeVersionInfo
 *  http://clang.llvm.org/docs/LanguageExtensions.html#feature-checking-macros
 */
#   define ECSL_COMPILER_CLANG
#   if defined(__clang_patchlevel__)
#       define ECSL_CLANG_VERSION \
            ECSL_COMPILER_ENCODE_VERSION_(__clang_major__, __clang_minor__, __clang_patchlevel__)
#   else
#       define ECSL_CLANG_VERSION \
            ECSL_COMPILER_ENCODE_VERSION_(__clang_major__, __clang_minor__)
#   endif

#   define ECSL_COMPILER_VERSION ECSL_CLANG_VERSION
#   define ECSL_COMPILER_NAME "clang"
#elif defined(__INTEL_COMPILER) || defined(__ICC)
/**
 * Compiler: icc
 * Sources:
 *  https://software.intel.com/en-us/node/524490
 *  https://software.intel.com/en-us/articles/c0x-features-supported-by-intel-c-compiler
 */
//? ICC have unique version scheme:
//? the version macro is defined as major * 100 + minor
#   define ECSL_COMPILER_ICC
#   if defined(__ICC) && !defined(__INTEL_COMPILER)
#       if defined(__INTEL_COMPILER_UPDATE)
#           define ECSL_ICC_VERSION \
                ECSL_COMPILER_ENCODE_VERSION_(__ICC * 100 + __INTEL_COMPILER_UPDATE)
#       else
#           define ECSL_ICC_VERSION \
                ECSL_COMPILER_ENCODE_VERSION_(__ICC * 100 + 0)
#       endif
#   else
#       if defined(__INTEL_COMPILER_UPDATE)
#           define ECSL_ICC_VERSION \
                ECSL_COMPILER_ENCODE_VERSION_(__INTEL_COMPILER * 100 + __INTEL_COMPILER_UPDATE)
#       else
#           define ECSL_ICC_VERSION \
                ECSL_COMPILER_ENCODE_VERSION_(__INTEL_COMPILER * 100 + 0)
#       endif
#   endif

#   define ECSL_COMPILER_VERSION ECSL_ICC_VERSION
#   define ECSL_COMPILER_NAME "icc"
#elif defined(_MSC_VER)
/**
 * Compiler: msvc
 * Sources:
 *  https://en.wikipedia.org/wiki/Microsoft_Visual_C++
 *  https://msdn.microsoft.com/en-us/library/hh567368.aspx#featurelist
 *  https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
 */
//? The scheme is near the same as for ICC
//? But MSVC have 2-level patch info which can't be encoded
//? in our decimal codding
#   define ECSL_COMPILER_MSVC
#   define ECSL_MSVC_VERSION \
    ECSL_COMPILER_ENCODE_VERSION_(_MSC_VER * 100)

#   define ECSL_COMPILER_VERSION ECSL_MSVC_VERSION
#   define ECSL_COMPILER_NAME "msvc"
#elif defined(__GNUC__)
/**
 * Compiler: gcc
 * Sources:
 *   https://gcc.gnu.org/projects/cxx-status.html
 */
#   define ECSL_COMPILER_GCC
#   if defined(__GNUC_PATCHLEVEL__)
#       define ECSL_GCC_VERSION \
            ECSL_COMPILER_ENCODE_VERSION_(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#   else
#       define ECSL_GCC_VERSION \
            ECSL_COMPILER_ENCODE_VERSION_(__GNUC__, __GNUC_MINOR__)
#   endif

#   define ECSL_COMPILER_VERSION ECSL_GCC_VERSION
#   define ECSL_COMPILER_NAME "gcc"
#else
#   define ECSL_COMPILER_UNKNOWN
#   define ECSL_COMPILER_VERSION \
        ECSL_COMPILER_ENCODE_VERSION_(0)
#   define ECSL_COMPILER_NAME "unknown"
#endif

#if defined(__cplusplus)

namespace ecsl {
namespace compiler {

enum class type
{
    compiler_unknown,
    compiler_clang,
    compiler_icc,
    compiler_msvc,
    compiler_gcc,

    /**
     * The strongly typed value of current compiler type.
     * Can be used as non type template parameter.
     * Must be accessed by: ecsl::compiler::type::value
     */
    value =
#if defined(ECSL_COMPILER_CLANG)
        type::compiler_clang,
#elif defined(ECSL_COMPILER_ICC)
        type::compiler_icc,
#elif defined(ECSL_COMPILER_MSVC)
        type::compiler_msvc,
#elif defined(ECSL_COMPILER_GCC)
        type::compiler_gcc,
#else
        type::compiler_unknown,
#endif
};

namespace version {

enum detail
{
    /**
     * Decimal encoded version of compiler in use.
     * Must be accessed by: ecsl::compiler::version::value
     */
    value = ECSL_COMPILER_VERSION
};

} // namespace version

/**
 * Reports the name of the compiler as C-string
 */
inline const char* name() noexcept
{
    return ECSL_COMPILER_NAME;
}

} // namespace compiler
} // namespace ecsl

#endif /* __cplusplus */
#endif /* ECSL_PLATFORM_COMPILER_HPP_ */