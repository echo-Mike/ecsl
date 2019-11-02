#ifndef ECSL_TYPE_TRAITS_DEFAULT_TAG_HPP_
#define ECSL_TYPE_TRAITS_DEFAULT_TAG_HPP_

/**
 * @file DefaultTag.hpp
 * Adds special type that represents default tag used where tag type is needed
 * by ecsl library
 */

namespace ecsl {

/**
 * This distinct type identifies the default tag everywhere where tags are used
 */
struct default_tag {};

} // namespace ecsl
#endif /* ECSL_TYPE_TRAITS_DEFAULT_TAG_HPP_ */