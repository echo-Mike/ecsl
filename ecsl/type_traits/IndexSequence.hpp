#ifndef ECSL_TYPE_TRAITS_INDEX_SEQUENCE_HPP_
#define ECSL_TYPE_TRAITS_INDEX_SEQUENCE_HPP_

/**
 * @file IndexSequence.hpp
 * Adds analog of std::integer_sequence for pre C++14 compilers
 */

/// STD
#include <cstdint>

namespace ecsl {

/**
 * @brief Holds compile-time sequence of indexes for usage
 * in argument pack expansion expressions during meta-programing
 */
template<std::size_t ... I>
struct index_sequence {};

namespace detail {
namespace index_seq {

template<std::size_t, std::size_t, std::size_t ... I>
struct index_builder;

template<std::size_t BEGIN, std::size_t ... I>
struct index_builder<BEGIN, BEGIN, I...>
{
    using type = index_sequence<I...>;
};

template<std::size_t BEGIN, std::size_t N, std::size_t ... I>
struct index_builder :
    public index_builder<BEGIN, N-1, N-1, I...>
{};

} // namespace index_seq
} // namespace detail

/**
 * @brief Meta-function makes ecsl::index_sequence in half-open
 * interval [BEGIN, END)
 */
template <std::size_t BEGIN, std::size_t END>
using make_index_sequence = typename detail::index_seq::index_builder<
    BEGIN, END>::type;


/**
 * @brief Helper meta-function that makes ecsl::index_sequence to unpack
 * std::tuple in std::get<>... expression
 */
template<class ... Args>
using tuple_unpack_sequence = make_index_sequence<0, sizeof...(Args)>;

} // namespace ecsl
#endif /* ECSL_TYPE_TRAITS_INDEX_SEQUENCE_HPP_ */
