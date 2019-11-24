#ifndef ECSL_CONTAINERS_DETAIL_VALUE_TRAIT_HPP_
#define ECSL_CONTAINERS_DETAIL_VALUE_TRAIT_HPP_

/// STD
#include <cstdint>
#include <type_traits>

namespace ecsl {
namespace containers {
namespace detail {

template<class T>
struct value_trait
{
    using value_type        = typename
        std::remove_reference<typename std::remove_cv<T>::type>::type;
    using reference         = typename
        std::add_lvalue_reference<value_type>::type;
    using const_reference   = typename
        std::add_lvalue_reference<typename std::add_const<value_type>::type>::type;
    using pointer           = typename
        std::add_pointer<value_type>::type;
    using const_pointer     = typename
        std::add_pointer<typename std::add_const<value_type>::type>::type;
    using size_type         = std::size_t;
};

} // namespace detail
} // namespace containers
} // namespace ecsl
#endif /* ECSL_CONTAINERS_DETAIL_VALUE_TRAIT_HPP_ */
