#ifndef ECSL_TYPE_TRAITS_SIMPLE_TYPES_HPP_
#define ECSL_TYPE_TRAITS_SIMPLE_TYPES_HPP_

/**
 * @file SimpleTypes.hpp
 * Adds a set of integer type aliases usable for various purposes
 */

namespace ecsl {
namespace types {

/**
 * Type thru which the bit representation of any object is accessible
 */
using memory_t = unsigned char;
/**
 * Type of positive only quantity. Identity the absolute value of distance
 */
using length_t = unsigned long long int;
/**
 * Type that identifies distance
 */
using slength_t = signed long long int;
/**
 * Type that identifies position in range. Positives and 0 are cointed from the
 * beginning of a range and negatives are counted from the end of a range
 */
using position_t = signed long long int;

} // namespace types
} // namespace ecsl
#endif /* ECSL_TYPE_TRAITS_SIMPLE_TYPES_HPP_ */
