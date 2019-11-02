#ifndef ECSL_TYPE_TRAITS_DISTINCT_TYPES_HPP_
#define ECSL_TYPE_TRAITS_DISTINCT_TYPES_HPP_

/**
 * @file DistinctTypes.hpp
 * Adds a set of distinct integer type aliases usable for various purposes
 */

/// ECSL
#include <ecsl/type_traits/DefaultTag.hpp>
#include <ecsl/type_traits/DistinctInteger.hpp>

namespace ecsl {
namespace types {

/**
 * Type of positive only quantity. Identity the absolute value of distance
 */
template<class TagType = default_tag>
using dlength_t = distinct_integer<unsigned long long int, TagType>;
/**
 * Type that identifies distance
 */
template<class TagType = default_tag>
using dslength_t = distinct_integer<signed long long int, TagType>;
/**
 * Type that identifies position in range. Positives and 0 are cointed from the
 * beginning of a range and negatives are counted from the end of a range
 */
template<class TagType = default_tag>
using dposition_t = distinct_integer<signed long long int, TagType>;

} // namespace types
} // namespace ecsl
#endif /* ECSL_TYPE_TRAITS_DISTINCT_TYPES_HPP_ */
