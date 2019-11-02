#ifndef ECSL_UTILITY_ENUM_CAST_HPP_
#define ECSL_UTILITY_ENUM_CAST_HPP_

/**
 * @file EnumCast.hpp
 * Adds a way to cast enum class values
 */

/// STD
#include <type_traits>

namespace ecsl {

/**
 * @brief Cast for C++11 enum class types
 * This function is a short-cut for long expression of enum class cast.
 * It must be used as all other cast expressions (static_cast, const_cast,
 * dynamic_cast and reinterpret_cast): enum_cast<to_type>(value of from_type)
 * @tparam T Any type to which that underlying_type of U may be cast using static_cast
 * @tparam U C++11 enum class type
 */
template<class T, class U>
constexpr T enum_cast(U enum_class_value)
{
    return static_cast<T>(static_cast<typename std::underlying_type<U>::type>(enum_class_value));
}

} // namespace ecsl
#endif /* ECSL_UTILITY_ENUM_CAST_HPP_ */