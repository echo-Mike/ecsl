#ifndef ECSL_MACRO_OPERATING_SYSTEM_H_
#define ECSL_MACRO_OPERATING_SYSTEM_H_

/**
 * @file OperatingSystem.h
 * Defines macro that specify current OS.
 * @todo Add support for MSDos and other OS.
 */

/// ECSL
#include <ecsl/macro/Core.h>
#include <ecsl/macro/Compiler.h>

#if defined($no)

/**
 * OS
 * Sources:
 *  https://sourceforge.net/p/predef/wiki/OperatingSystems/
 *  https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive
 *  https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
 *  http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
 */

/// System classes

// WINDOWS systems class
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(__WIN32__)
#   define $windows      $yes
#   define $notwindows   $no
#else
#   define $windows      $no
#   define $notwindows   $yes
#endif
// UNIX systems class
#if defined(unix) || defined(__unix__) || defined(__unix)
#   define $unix         $yes
#   define $notunix      $no
#else
#   define $unix         $no
#   define $notunix      $yes
#endif
// Linux based OSes: this macro will not be defined on Apple/BSD/Sun/Android based OSes
#if defined(__linux__) || defined(linux) || defined(__linux)
#   define $linux        $yes
#   define $notlinux     $no
#else
#   define $linux        $no
#   define $notlinux     $yes
#endif
// Apple systems class
#if defined(__APPLE__) || defined(__MACH__)
#   define $apple        $yes
#   define $notapple     $no
#else
#   define $apple        $no
#   define $notapple     $yes
#endif
// SUN system class
#if defined(sun) || defined(__sun)
#   define $sun          $yes
#   define $notsun       $no
#else
#   define $sun          $no
#   define $notsun       $yes
#endif
// BSD systems class
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
#   define $bsd          $yes
#   define $notbsd       $no
#else
#   define $bsd          $no
#   define $notbsd       $yes
#endif
// Android systems class
#if defined(__ANDROID_API__) || defined(__ANDROID__)
#   define $android    $yes
#   define $notandroid $no
// Reset linux macro
#   undef $linux
#   undef $notlinux
#   define $linux        $no
#   define $notlinux     $yes
#else
#   define $android    $no
#   define $notandroid $yes
#endif

/**
 * @def $windows
 * Defined as $yes on Windows OS family.
 */
/**
 * @def $notwindows
 * Defined as $yes on not Windows OS family.
 */
/**
 * @def $unix
 * Defined as $yes on Unix OS family.
 * @note This macro may be defined as $yes on Apple/BSD/Sun/Android OSes.
 */
/**
 * @def $notunix
 * Defined as $yes on not Unix OS family.
 */
/**
 * @def $apple
 * Defined as $yes on Apple OS family.
 */
/**
 * @def $notapple
 * Defined as $yes on not Apple OS family.
 */
/**
 * @def $linux
 * Defined as $yes on Linux based OS.
 * @note This macro will be defined as $no on Apple/BSD/Sun/Android OSes.
 */
/**
 * @def $notlinux
 * Defined as $yes on not Linux based OS.
 */
/**
 * @def $sun
 * Defined as $yes on SUN based OS.
 */
/**
 * @def $notsun
 * Defined as $yes on not SUN based OS.
 */
/**
 * @def $bsd
 * Defined as $yes on BSD based OS.
 */
/**
 * @def $notbsd
 * Defined as $yes on not BSD based OS.
 */
/**
 * @def $android
 * Defined as $yes on Android OSes.
 */
/**
 * @def $notandroid
 * Defined as $yes on not Android OSes.
 */

#if $is($windows) || $is($linux) || $is($apple) || $is($sun) || $is($bsd) || $is($android)
#   define $supported_os   $yes
#   define $unsupported_os $no
#   define $os $known
#else
#   define $supported_os   $no
#   define $unsupported_os $yes
#   define $os $unknown
#endif
/**
 * @def $supported_os
 * Defined as $yes on supported OS.
 */
/**
 * @def $unsupported_os
 * Defined as $yes on unsupported OS.
 */
/**
 * @def $os
 * Defined as $yes if OS is deduced anf $no otherwise.
 */

/// Apple systems

#if $is($apple)
#   include <TargetConditionals.h>
#   if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#       define $ios      $yes
#       define $notios   $no
#       define $osx      $no
#       define $notosx   $yes
#   elif defined(TARGET_OS_MAC)
#       define $osx      $yes
#       define $notosx   $no
#       define $ios      $no
#       define $notios   $yes
#   endif
// Reset linux macro
#   undef $linux
#   undef $notlinux
#   define $linux        $no
#   define $notlinux     $yes
#else
#   define $osx          $no
#   define $notosx       $yes
#   define $ios          $no
#   define $notios       $yes
#endif

/**
 * @def $osx
 * Defined as $yes on Apple OSX.
 */
/**
 * @def $notosx
 * Defined as $yes on not Apple OSX.
 */
/**
 * @def $ios
 * Defined as $yes on Apple IOS.
 */
/**
 * @def $notios
 * Defined as $yes on not Apple IOS.
 */

/// SUN systems

#if $is($sun)
#   if defined(__SVR4) || defined(__svr4__)
#       define $solaris    $yes
#       define $notsolaris $no
#       define $sunos      $no
#       define $notsunos   $yes
#   else
#       define $sunos      $yes
#       define $notsunos   $no
#       define $solaris    $no
#       define $notsolaris $yes
#   endif
// Reset linux macro
#   undef $linux
#   undef $notlinux
#   define $linux        $no
#   define $notlinux     $yes
#else
#   define $sunos        $no
#   define $notsunos     $yes
#   define $solaris      $no
#   define $notsolaris   $yes
#endif

/**
 * @def $sunos
 * Defined as $yes on SunOS.
 */
/**
 * @def $notsunos
 * Defined as $yes on not SunOS.
 */
/**
 * @def $solaris
 * Defined as $yes on Solaris OS.
 */
/**
 * @def $notsolaris
 * Defined as $yes on not Solaris OS.
 */

/// BSD systems

#if $is($bsd)
#   if defined(__FreeBSD__)
#       define $freebsd      $yes
#       define $notfreebsd   $no
#   else
#       define $freebsd      $no
#       define $notfreebsd   $yes
#   endif
#   if defined(__NetBSD__)
#       define $netbsd       $yes
#       define $notnetbsd    $no
#   else
#       define $netbsd       $no
#       define $notnetbsd    $yes
#   endif
#   if defined(__OpenBSD__)
#       define $openbsd      $yes
#       define $notopenbsd   $no
#   else
#       define $openbsd      $no
#       define $notopenbsd   $yes
#   endif
#   if defined(__bsdi__)
#       define $bsdos        $yes
#       define $notbsdos     $no
#   else
#       define $bsdos        $no
#       define $notbsdos     $yes
#   endif
#   if defined(__DragonFly__)
#       define $dragonfly    $yes
#       define $notdragonfly $no
#   else
#       define $dragonfly    $no
#       define $notdragonfly $yes
#   endif
// Reset linux macro
#   undef $linux
#   undef $notlinux
#   define $linux        $no
#   define $notlinux     $yes
#else
#   define $freebsd      $no
#   define $notfreebsd   $yes
#   define $netbsd       $no
#   define $notnetbsd    $yes
#   define $openbsd      $no
#   define $notopenbsd   $yes
#   define $bsdos        $no
#   define $notbsdos     $yes
#   define $dragonfly    $no
#   define $notdragonfly $yes
#endif

/**
 * @def $freebsd
 * Defined as $yes on FreeBSD.
 */
/**
 * @def $notfreebsd
 * Defined as $yes on not FreeBSD.
 */
/**
 * @def $netbsd
 * Defined as $yes on NetBSD.
 */
/**
 * @def $notnetbsd
 * Defined as $yes on not NetBSD.
 */
/**
 * @def $openbsd
 * Defined as $yes on OpenBSD.
 */
/**
 * @def $notopenbsd
 * Defined as $yes on not OpenBSD.
 */
/**
 * @def $bsdos
 * Defined as $yes on BSDOS.
 */
/**
 * @def $notbsdos
 * Defined as $yes on not BSDOS.
 */
/**
 * @def $dragonfly
 * Defined as $yes on Dragonfly OS.
 */
/**
 * @def $notdragonfly
 * Defined as $yes on not Dragonfly OS.
 */

#endif // $no
#endif /* ECSL_MACRO_OPERATING_SYSTEM_H_ */