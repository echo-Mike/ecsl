#ifndef ECSL_MACRO_LOG_HPP_
#define ECSL_MACRO_LOG_HPP_

/**
 * @file Log.hpp
 * Defines small logger library
 * Library is based on std stream
 * May be defined thread safe using std::mutex
 * The logging code may be removed if some level or type is not needed
 */

/// STD
#if defined($log_blocking_enabled)
#   include <mutex>
#endif
/**
 * @def $log_no_ios
 * If defined no std iostream headers are used
 */
#if !defined($log_no_ios)
#   include <iostream>
#endif
/// ECSL
#include <ecsl/macro/Core.h>
#include <ecsl/macro/Text.h>
#include <ecsl/macro/Build.h>

/// Log types for various build settings

#define $log_type_COMMON    $yes
#define $log_type_DEBUG     $debug
#define $log_type_RELEASE   $release

/// Default log levels

#define $log_level_SUPPRESS $no  // Always $no
#define $log_level_STEADY   $yes // Always $yes

#ifndef $log_level_INFO
#   define $log_level_INFO $no
#endif
#ifndef $log_level_WARNING
#   define $log_level_WARNING $no
#endif
#ifndef $log_level_ERROR
#   define $log_level_ERROR $yes
#endif

/// Default log category

#ifndef $log_category_GENERAL
#   define $log_category_GENERAL $no
#endif

/// Streams

/**
 * @def $log_custom_framing_enabled
 * Accessor for log stream object
 */
#define $log_stream(TYPE) \
    $log_stream_ ## TYPE

#if !defined($log_no_ios)
#   ifndef $log_stream_COMMON
#       define $log_stream_COMMON ::std::clog
#   endif
#   ifndef $log_stream_DEBUG
#       define $log_stream_DEBUG ::std::clog
#   endif
#   ifndef $log_stream_RELEASE
#       define $log_stream_RELEASE ::std::clog
#   endif
#endif

/**
 * @def $clog
 * Accessor for log stream object in COMMON log type
 */
#define $clog $log_stream(COMMON)
/**
 * @def $dlog
 * Accessor for log stream object in DEBUG log type
 */
#define $dlog $log_stream(DEBUG)
/**
 * @def $rlog
 * Accessor for log stream object in RELEASE log type
 */
#define $rlog $log_stream(RELEASE)

/// Log prefix setup

/**
 * @def $log_custom_framing_enabled
 * Enables usage of user defined (custom) log prefix and suffix
 */
#ifdef $log_custom_framing_enabled
#   define $log_custom_framing_ $yes
#else
#   define $log_custom_framing_ $no
#endif

/**
 * @def $log_prefix
 * Customization point for user defined log prefix
 * Single param is provided:
 *  LOG_STREAM - the stream to write log to
 */
#ifndef $log_prefix
#   define $log_prefix(LOG_STREAM)
#endif

/**
 * @def $log_suffix
 * Customization point for user defined log suffix
 * Single param is provided:
 *  LOG_STREAM - the stream to write log to
 */
#ifndef $log_suffix
#   define $log_suffix(LOG_STREAM)
#endif

/// Code info for logging

/**
 * @def $log_strings_enabled
 * Enables usage of set of C-strings describing code place and log information
 */
#ifdef $log_strings_enabled
#   define $log_strings_ $yes
#else
#   define $log_strings_ $no
#endif

// Local name constructor
#define $log_data_name_(name) log_ ## name ## _c_str_

// Local name constructor
#define $log_define_c_str_(name, value) \
    $log_strings_(const char* $log_data_name_(name) = value;)

/**
 * @def $log_data
 * Accessor for log info C-strings
 * Known names:
 *  category - log category
 *  file - platform dependent source file name
 *  func - platform dependent enclosing function name
 *  level - log level
 *  line - line number of $log_begin directive
 *  type - log type
 */
#define $log_data(name) $cond($log_strings_, $log_data_name_(name), "")

/// Thread safety

/**
 * @def $log_blocking_enabled
 * Enables blocking facility for logging
 * User must define the $log_mutex_* macro for each type of logging
 */
#if defined($log_blocking_enabled)
#   define $log_blocking_ $yes
#else 
#   define $log_blocking_ $no
#endif

#define $log_mu_name_(TYPE) $log_mutex_ ## TYPE

/**
 * @def $log_lockguard
 * Customization point for RAII synchronization of IO operations
 */
#if !defined($log_lockguard)
#   define $log_lockguard(TYPE) \
    $log_blocking_(std::lock_guard<std::mutex> log_lg_{$log_mu_name_(TYPE)};)
#endif

/// Logging

/**
 * @def $log_begin
 * Identifies the beginning of log operation
 * No exception thrown by io operation is modified
 */
#define $log_begin(TYPE, LEVEL, CATEGORY)                           \
$if ($log_type_ ## TYPE) {                                          \
$if ($log_level_ ## LEVEL) {                                        \
$if ($log_category_ ## CATEGORY) {                                  \
    $log_define_c_str_(category, #CATEGORY)                         \
    $log_define_c_str_(file, static_cast<const char*>(__FILE__))    \
    $log_define_c_str_(func, static_cast<const char*>(__func__))    \
    $log_define_c_str_(level, #LEVEL)                               \
    $log_define_c_str_(line, $stringify(__LINE__))                  \
    $log_define_c_str_(type, #TYPE)                                 \
    $log_lockguard(TYPE)                                            \
    $log_custom_framing_($log_prefix($log_stream(TYPE)))

/**
 * @def $log_end
 * Identifies the ending of log operation
 */
#define $log_end(TYPE)                                      \
    $log_custom_framing_($log_suffix($log_stream(TYPE)))    \
}}}

/// Logging category shorteners

/**
 * @def $clog_begin
 * Identifies the beginning of log operation for COMMON category
 */
#define $clog_begin(LEVEL, CATEGORY) \
    $log_begin(COMMON, LEVEL, CATEGORY)
/**
 * @def $clog_end
 * Identifies the ending of log operation for COMMON category
 */
#define $clog_end $log_end(COMMON)

/**
 * @def $dlog_begin
 * Identifies the beginning of log operation for DEBUG category
 */
#define $dlog_begin(LEVEL, CATEGORY) \
    $log_begin(DEBUG, LEVEL, CATEGORY)
/**
 * @def $dlog_end
 * Identifies the ending of log operation for DEBUG category
 */
#define $dlog_end $log_end(DEBUG)

/**
 * @def $rlog_begin
 * Identifies the beginning of log operation for RELEASE category
 */
#define $rlog_begin(LEVEL, CATEGORY) \
    $log_begin(RELEASE, LEVEL, CATEGORY)
/**
 * @def $rlog_end
 * Identifies the ending of log operation for RELEASE category
 */
#define $rlog_end $log_end(RELEASE)

/// Log formats

/**
 * @def $log_block_begin
 * Identifies beginning of logging block
 */
/**
 * @def $log_block_beginC
 * Identifies beginning of logging block optimized for single character IO operation
 */
/**
 * @def $log_block_end
 * Identifies ending of logging block
 */
/**
 * @def $log_block_endC
 * Identifies ending of logging block optimized for single character IO operation
 */
/**
 * @def $log_array_begin
 * Identifies beginning of logging an array
 */
/**
 * @def $log_array_beginC
 * Identifies beginning of logging an array optimized for single character IO operation
 */
/**
 * @def $log_array_end
 * Identifies ending of logging an array
 */
/**
 * @def $log_array_endC
 * Identifies ending of logging an array optimized for single character IO operation
 */
/**
 * @def $log_as_str
 * Identifies that string must be pushed to log
 */
/**
 * @def $log_as_strC
 * Identifies that string must be pushed to log
 * optimized for single character IO operation on the end of string
 */
/**
 * @def $log_as_strCC
 * Identifies that string must be pushed to log
 * optimized for single character IO operation on the both ends of string
 */
/**
 * @def $log_as_strM
 * Identifies that string must be pushed to log but it is a C-string
 * and cam be merged by compiler with nearby C-strings which redices IO operations
 */
/**
 * @def $log_entry
 * Identifies the log entry named name
 */
/**
 * @def $log_next
 * Identifies the transition to the nex log entry
 */
/**
 * @def $log_nextC
 * Identifies the transition to the nex log entry
 * optimized for single character IO operation
 */

/**
 * @def $log_jsonlike_enabled
 * Enables JSON like loging in terms of log format stated above
 */
#if defined($log_jsonlike_enabled)
// SEPARATOR

// Marks the separation between log entry name and content
#define $log_sep_ ":"
// Marks the separation between log entry name and content (optimized output)
#define $log_sepC_ ':'

// BLOCKS

#define $log_block_begin "{"
#define $log_block_beginC '{'
#define $log_block_end "}"
#define $log_block_endC '}'

// ARRAYS

#define $log_array_begin "["
#define $log_array_beginC '['
#define $log_array_end "]"
#define $log_array_endC ']'

// STRINGS

#define $log_as_str(...) "\"" << __VA_ARGS__ << "\""
#define $log_as_strC(...) "\"" << __VA_ARGS__ << '"'
#define $log_as_strCC(...) '"' << __VA_ARGS__ << '"'
#define $log_as_strM(...) $json_str(__VA_ARGS__)

// ENTRIES

#define $log_entry(name) $json_str(name) $log_sep_
#define $log_next ","
#define $log_nextC ','

#endif /* $log_jsonlike_enabled */

#endif /* ECSL_MACRO_LOG_HPP_ */