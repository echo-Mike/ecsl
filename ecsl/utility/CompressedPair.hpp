#ifndef ECSL_UTILITY_COMPRESSED_PAIR_HPP_
#define ECSL_UTILITY_COMPRESSED_PAIR_HPP_

/**
 * @file CompresedPair.hpp
 * Adds std::pair like type that compresses it's components using EBO
 */

/// STD
#include <cstdint>
#include <utility>
#include <type_traits>
/// ECSL
#include <ecsl/type_traits/IndexSequence.hpp>

namespace ecsl {

/**
 * @brief Compacts storage for two types if one (or both) are empty types
 * The basic principle is that deriving from empty (no value members only
 * function members) base class is costless, but storing same object as a value
 * member costs one byte plus padding and alignment. This types of objects
 * commonly are function objects.
 *
 * This class simply tests if provided types are empty and derives from them
 * instead of defining members. It has 4 specializations and interface
 * compatible with std::pair, except it is not an aggregate and access to
 * members is done by the means of member functions, not members themselves,
 * a.e. functions get_first/get_second are derfined.
 */
template<class T1, class T2,
    bool = std::is_empty<T1>::value,
    bool = std::is_empty<T2>::value>
class compressed_pair :
    private T1,
    private T2
{
  public:
    using first_type = T1;
    using second_type = T2;

  private:
    template<
        class ... Args1, std::size_t ... Index1
        class ... Args2, std::size_t ... Index2
    >
    inline compressed_pair(
        std::tuple<Args1...>& first_args,
        index_sequence<Index1...>,
        std::tuple<Args2...>& second_args,
        index_sequence<Index2...>
    ) :
        first_type(std::forward<Args1>(std::get<Index1>(first_args))...),
        second_type(std::forward<Args2>(std::get<Index2>(second_args))...)
    {}

  public:
    template<class = typename std::enable_if<
        std::is_default_constructible<first_type>::value &&
        std::is_default_constructible<second_type>::value
    >::type>
    compressed_pair() noexcept(
        std::is_nothrow_default_constructible<first_type>::value &&
        std::is_nothrow_default_constructible<second_type>::value
    ) :
        first_type{},
        second_type{}
    {}

    template<class = typename std::enable_if<
        std::is_copy_constructible<first_type>::value &&
        std::is_copy_constructible<second_type>::value
    >::type>
    compressed_pair(const first_type x, const second_type& y) noexcept(
        std::is_nothrow_copy_constructible<first_type>::value &&
        std::is_nothrow_copy_constructible<second_type>::value
    ) :
        first_type{x},
        second_type{y}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(U1&& x, U2&& y) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        first_type{std::forward<U1>(x)},
        second_type{std::forward<U2>(y)}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, const U1&>::value &&
            std::is_constructible<second_type, const U2&>::value
        >::type
    >
    compressed_pair(const compressed_pair<U1, U2>& p) noexcept(
        std::is_nothrow_constructible<first_type, const U1&>::value &&
        std::is_nothrow_constructible<second_type, const U2&>::value
    ) :
        first_type{p.get_first()},
        second_type{p.get_second()}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(compressed_pair<U1, U2>&& p) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        first_type{std::move(p.get_first())},
        second_type{std::move(p.get_second())}
    {}

    template<
        class ... Args1,
        class ... Args2,
        class = std::enable_if<
            std::is_constructible<first_type, Args1&&...>::value &&
            std::is_constructible<second_type, Args2&&...>::value
        >::type
    >
    compressed_pair(
        std::piecewise_construct_t,
        std::tuple<Args1...> first_args,
        std::tuple<Args2...> second_args,
    ) : compressed_pair(
        first_args, tuple_unpack_sequence<Args1...>(),
        second_args, tuple_unpack_sequence<Args2...>())
    {}

    compressed_pair(const compressed_pair&) = default;
    compressed_pair(compressed_pair&&) = default;

    template<bool =
        std::is_copy_assignable<first_type>::value &&
        std::is_copy_assignable<second_type>::value
    >
    compressed_pair& operator=(const compressed_pair& other)
        noexcept(
            std::is_nothrow_copy_assignable<first_type>::value &&
            std::is_nothrow_copy_assignable<second_type>::value
        )
    {
        get_first() = other.get_first();
        get_second() = other.get_second();
        return *this;
    }

    compressed_pair& operator=<false>(const compressed_pair&) = delete;

    template<class = typename std::enable_if<
        std::is_move_assignable<first_type>::value &&
        std::is_move_assignable<second_type>::value
    >::type>
    compressed_pair& operator=(compressed_pair&& other)
        noexcept(
            std::is_nothrow_move_assignable<first_type>::value &&
            std::is_nothrow_move_assignable<second_type>::value
        )
    {
        get_first() = std::move(other.get_first());
        get_second() = std::move(other.get_second());
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, const U1&>::value &&
        std::is_assignable<second_type, const U2&>::value,
        compressed_pair&
    >::type operator=(const compressed_pair<U1, U2>& p)
        noexcept(
            std::is_nothrow_assignable<first_type, const U1&>::value &&
            std::is_nothrow_assignable<second_type, const U2&>::value
        )
    {
        get_first() = p.get_first();
        get_second() = p.get_second();
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, U1&&>::value &&
        std::is_assignable<second_type, U2&&>::value,
        compressed_pair&
    >::type operator=(compressed_pair<U1, U2>&& p)
        noexcept(
            std::is_nothrow_assignable<first_type, U1&&>::value &&
            std::is_nothrow_assignable<second_type, U2&&>::value
        )
    {
        get_first() = std::move(p.get_first());
        get_second() = std::move(p.get_second());
        return *this;
    }

    first_type& get_first() { return *this; }
    const first_type& get_first() const { return *this; }

    second_type& get_second() { return *this; }
    const second_type& get_second() const { return *this; }
};

template<class T1, class T2>
class compressed_pair<T1, T2, true, false> :
    private T1
{
  public:
    using first_type = T1;
    using second_type = T2;

  private:
    template<
        class ... Args1, std::size_t ... Index1
        class ... Args2, std::size_t ... Index2
    >
    inline compressed_pair(
        std::tuple<Args1...>& first_args,
        index_sequence<Index1...>,
        std::tuple<Args2...>& second_args,
        index_sequence<Index2...>
    ) :
        first_type(std::forward<Args1>(std::get<Index1>(first_args))...),
        m_second(std::forward<Args2>(std::get<Index2>(second_args))...)
    {}

  public:
    template<class = typename std::enable_if<
        std::is_default_constructible<first_type>::value &&
        std::is_default_constructible<second_type>::value
    >::type>
    compressed_pair() noexcept(
        std::is_nothrow_default_constructible<first_type>::value &&
        std::is_nothrow_default_constructible<second_type>::value
    ) :
        first_type{},
        m_second{}
    {}

    template<class = typename std::enable_if<
        std::is_copy_constructible<first_type>::value &&
        std::is_copy_constructible<second_type>::value
    >::type>
    compressed_pair(const first_type x, const second_type& y) noexcept(
        std::is_nothrow_copy_constructible<first_type>::value &&
        std::is_nothrow_copy_constructible<second_type>::value
    ) :
        first_type{x},
        m_second{y}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(U1&& x, U2&& y) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        first_type{std::forward<U1>(x)},
        m_second{std::forward<U2>(y)}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, const U1&>::value &&
            std::is_constructible<second_type, const U2&>::value
        >::type
    >
    compressed_pair(const compressed_pair<U1, U2>& p) noexcept(
        std::is_nothrow_constructible<first_type, const U1&>::value &&
        std::is_nothrow_constructible<second_type, const U2&>::value
    ) :
        first_type{p.get_first()},
        m_second{p.get_second()}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(compressed_pair<U1, U2>&& p) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        first_type{std::move(p.get_first())},
        m_second{std::move(p.get_second())}
    {}

    template<
        class ... Args1,
        class ... Args2,
        class = std::enable_if<
            std::is_constructible<first_type, Args1&&...>::value &&
            std::is_constructible<second_type, Args2&&...>::value
        >::type
    >
    compressed_pair(
        std::piecewise_construct_t,
        std::tuple<Args1...> first_args,
        std::tuple<Args2...> second_args,
    ) : compressed_pair(
        first_args, tuple_unpack_sequence<Args1...>(),
        second_args, tuple_unpack_sequence<Args2...>())
    {}

    compressed_pair(const compressed_pair&) = default;
    compressed_pair(compressed_pair&&) = default;

    template<bool =
        std::is_copy_assignable<first_type>::value &&
        std::is_copy_assignable<second_type>::value
    >
    compressed_pair& operator=(const compressed_pair& other)
        noexcept(
            std::is_nothrow_copy_assignable<first_type>::value &&
            std::is_nothrow_copy_assignable<second_type>::value
        )
    {
        get_first() = other.get_first();
        get_second() = other.get_second();
        return *this;
    }

    compressed_pair& operator=<false>(const compressed_pair&) = delete;

    template<class = typename std::enable_if<
        std::is_move_assignable<first_type>::value &&
        std::is_move_assignable<second_type>::value
    >::type>
    compressed_pair& operator=(compressed_pair&& other)
        noexcept(
            std::is_nothrow_move_assignable<first_type>::value &&
            std::is_nothrow_move_assignable<second_type>::value
        )
    {
        get_first() = std::move(other.get_first());
        get_second() = std::move(other.get_second());
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, const U1&>::value &&
        std::is_assignable<second_type, const U2&>::value,
        compressed_pair&
    >::type operator=(const compressed_pair<U1, U2>& p)
        noexcept(
            std::is_nothrow_assignable<first_type, const U1&>::value &&
            std::is_nothrow_assignable<second_type, const U2&>::value
        )
    {
        get_first() = p.get_first();
        get_second() = p.get_second();
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, U1&&>::value &&
        std::is_assignable<second_type, U2&&>::value,
        compressed_pair&
    >::type operator=(compressed_pair<U1, U2>&& p)
        noexcept(
            std::is_nothrow_assignable<first_type, U1&&>::value &&
            std::is_nothrow_assignable<second_type, U2&&>::value
        )
    {
        get_first() = std::move(p.get_first());
        get_second() = std::move(p.get_second());
        return *this;
    }

    first_type& get_first() { return *this; }
    const first_type& get_first() const { return *this; }

    second_type& get_second() { return m_second; }
    const second_type& get_second() const { return m_second; }

  private:
    second_type m_second;
};

template<class T1, class T2>
class compressed_pair<T1, T2, false, true> :
    private T2
{
  public:
    using first_type = T1;
    using second_type = T2;

  private:
    template<
        class ... Args1, std::size_t ... Index1
        class ... Args2, std::size_t ... Index2
    >
    inline compressed_pair(
        std::tuple<Args1...>& first_args,
        index_sequence<Index1...>,
        std::tuple<Args2...>& second_args,
        index_sequence<Index2...>
    ) :
        m_first(std::forward<Args1>(std::get<Index1>(first_args))...),
        second_type(std::forward<Args2>(std::get<Index2>(second_args))...)
    {}

  public:
    template<class = typename std::enable_if<
        std::is_default_constructible<first_type>::value &&
        std::is_default_constructible<second_type>::value
    >::type>
    compressed_pair() noexcept(
        std::is_nothrow_default_constructible<first_type>::value &&
        std::is_nothrow_default_constructible<second_type>::value
    ) :
        m_first{},
        second_type{}
    {}

    template<class = typename std::enable_if<
        std::is_copy_constructible<first_type>::value &&
        std::is_copy_constructible<second_type>::value
    >::type>
    compressed_pair(const first_type x, const second_type& y) noexcept(
        std::is_nothrow_copy_constructible<first_type>::value &&
        std::is_nothrow_copy_constructible<second_type>::value
    ) :
        m_first{x},
        second_type{y}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(U1&& x, U2&& y) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        m_first{std::forward<U1>(x)},
        second_type{std::forward<U2>(y)}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, const U1&>::value &&
            std::is_constructible<second_type, const U2&>::value
        >::type
    >
    compressed_pair(const compressed_pair<U1, U2>& p) noexcept(
        std::is_nothrow_constructible<first_type, const U1&>::value &&
        std::is_nothrow_constructible<second_type, const U2&>::value
    ) :
        m_first{p.get_first()},
        second_type{p.get_second()}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(compressed_pair<U1, U2>&& p) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        m_first{std::move(p.get_first())},
        second_type{std::move(p.get_second())}
    {}

    template<
        class ... Args1,
        class ... Args2,
        class = std::enable_if<
            std::is_constructible<first_type, Args1&&...>::value &&
            std::is_constructible<second_type, Args2&&...>::value
        >::type
    >
    compressed_pair(
        std::piecewise_construct_t,
        std::tuple<Args1...> first_args,
        std::tuple<Args2...> second_args,
    ) : compressed_pair(
        first_args, tuple_unpack_sequence<Args1...>(),
        second_args, tuple_unpack_sequence<Args2...>())
    {}

    compressed_pair(const compressed_pair&) = default;
    compressed_pair(compressed_pair&&) = default;

    template<bool =
        std::is_copy_assignable<first_type>::value &&
        std::is_copy_assignable<second_type>::value
    >
    compressed_pair& operator=(const compressed_pair& other)
        noexcept(
            std::is_nothrow_copy_assignable<first_type>::value &&
            std::is_nothrow_copy_assignable<second_type>::value
        )
    {
        get_first() = other.get_first();
        get_second() = other.get_second();
        return *this;
    }

    compressed_pair& operator=<false>(const compressed_pair&) = delete;

    template<class = typename std::enable_if<
        std::is_move_assignable<first_type>::value &&
        std::is_move_assignable<second_type>::value
    >::type>
    compressed_pair& operator=(compressed_pair&& other)
        noexcept(
            std::is_nothrow_move_assignable<first_type>::value &&
            std::is_nothrow_move_assignable<second_type>::value
        )
    {
        get_first() = std::move(other.get_first());
        get_second() = std::move(other.get_second());
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, const U1&>::value &&
        std::is_assignable<second_type, const U2&>::value,
        compressed_pair&
    >::type operator=(const compressed_pair<U1, U2>& p)
        noexcept(
            std::is_nothrow_assignable<first_type, const U1&>::value &&
            std::is_nothrow_assignable<second_type, const U2&>::value
        )
    {
        get_first() = p.get_first();
        get_second() = p.get_second();
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, U1&&>::value &&
        std::is_assignable<second_type, U2&&>::value,
        compressed_pair&
    >::type operator=(compressed_pair<U1, U2>&& p)
        noexcept(
            std::is_nothrow_assignable<first_type, U1&&>::value &&
            std::is_nothrow_assignable<second_type, U2&&>::value
        )
    {
        get_first() = std::move(p.get_first());
        get_second() = std::move(p.get_second());
        return *this;
    }

    first_type& get_first() { return m_first; }
    const first_type& get_first() const { return m_first; }

    second_type& get_second() { return *this; }
    const second_type& get_second() const { return *this; }

  private:
    first_type m_first;
};

template<class T1, class T2>
class compressed_pair<T1, T2, false, false>
{
    template<
        class ... Args1, std::size_t ... Index1
        class ... Args2, std::size_t ... Index2
    >
    inline compressed_pair(
        std::tuple<Args1...>& first_args,
        index_sequence<Index1...>,
        std::tuple<Args2...>& second_args,
        index_sequence<Index2...>
    ) :
        m_first(std::forward<Args1>(std::get<Index1>(first_args))...),
        m_second(std::forward<Args2>(std::get<Index2>(second_args))...)
    {}

  public:
    using first_type  = T1;
    using second_type = T2;

    template<class = typename std::enable_if<
        std::is_default_constructible<first_type>::value &&
        std::is_default_constructible<second_type>::value
    >::type>
    compressed_pair() noexcept(
        std::is_nothrow_default_constructible<first_type>::value &&
        std::is_nothrow_default_constructible<second_type>::value
    ) :
        m_first{},
        m_second{}
    {}

    template<class = typename std::enable_if<
        std::is_copy_constructible<first_type>::value &&
        std::is_copy_constructible<second_type>::value
    >::type>
    compressed_pair(const first_type x, const second_type& y) noexcept(
        std::is_nothrow_copy_constructible<first_type>::value &&
        std::is_nothrow_copy_constructible<second_type>::value
    ) :
        m_first{x},
        m_second{y}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(U1&& x, U2&& y) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        m_first{std::forward<U1>(x)},
        m_second{std::forward<U2>(y)}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, const U1&>::value &&
            std::is_constructible<second_type, const U2&>::value
        >::type
    >
    compressed_pair(const compressed_pair<U1, U2>& p) noexcept(
        std::is_nothrow_constructible<first_type, const U1&>::value &&
        std::is_nothrow_constructible<second_type, const U2&>::value
    ) :
        m_first{p.get_first()},
        m_second{p.get_second()}
    {}

    template<
        class U1,
        class U2,
        class = typename std::enable_if<
            std::is_constructible<first_type, U1&&>::value &&
            std::is_constructible<second_type, U2&&>::value
        >::type
    >
    compressed_pair(compressed_pair<U1, U2>&& p) noexcept(
        std::is_nothrow_constructible<first_type, U1&&>::value &&
        std::is_nothrow_constructible<second_type, U2&&>::value
    ) :
        m_first{std::move(p.get_first())},
        m_second{std::move(p.get_second())}
    {}

    template<
        class ... Args1,
        class ... Args2,
        class = std::enable_if<
            std::is_constructible<first_type, Args1&&...>::value &&
            std::is_constructible<second_type, Args2&&...>::value
        >::type
    >
    compressed_pair(
        std::piecewise_construct_t,
        std::tuple<Args1...> first_args,
        std::tuple<Args2...> second_args,
    ) : compressed_pair(
        first_args, tuple_unpack_sequence<Args1...>(),
        second_args, tuple_unpack_sequence<Args2...>())
    {}

    compressed_pair(const compressed_pair&) = default;
    compressed_pair(compressed_pair&&) = default;

    template<bool =
        std::is_copy_assignable<first_type>::value &&
        std::is_copy_assignable<second_type>::value
    >
    compressed_pair& operator=(const compressed_pair& other)
        noexcept(
            std::is_nothrow_copy_assignable<first_type>::value &&
            std::is_nothrow_copy_assignable<second_type>::value
        )
    {
        get_first() = other.get_first();
        get_second() = other.get_second();
        return *this;
    }

    compressed_pair& operator=<false>(const compressed_pair&) = delete;

    template<class = typename std::enable_if<
        std::is_move_assignable<first_type>::value &&
        std::is_move_assignable<second_type>::value
    >::type>
    compressed_pair& operator=(compressed_pair&& other)
        noexcept(
            std::is_nothrow_move_assignable<first_type>::value &&
            std::is_nothrow_move_assignable<second_type>::value
        )
    {
        get_first() = std::move(other.get_first());
        get_second() = std::move(other.get_second());
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, const U1&>::value &&
        std::is_assignable<second_type, const U2&>::value,
        compressed_pair&
    >::type operator=(const compressed_pair<U1, U2>& p)
        noexcept(
            std::is_nothrow_assignable<first_type, const U1&>::value &&
            std::is_nothrow_assignable<second_type, const U2&>::value
        )
    {
        get_first() = p.get_first();
        get_second() = p.get_second();
        return *this;
    }

    template<class U1, class U2>
    typename std::enable_if<
        std::is_assignable<first_type, U1&&>::value &&
        std::is_assignable<second_type, U2&&>::value,
        compressed_pair&
    >::type operator=(compressed_pair<U1, U2>&& p)
        noexcept(
            std::is_nothrow_assignable<first_type, U1&&>::value &&
            std::is_nothrow_assignable<second_type, U2&&>::value
        )
    {
        get_first() = std::move(p.get_first());
        get_second() = std::move(p.get_second());
        return *this;
    }

    first_type& get_first() { return m_first; }
    const first_type& get_first() const { return m_first; }

    second_type& get_second() { return m_second; }
    const second_type& get_second() const { return m_second; }

  private:
    first_type m_first;
    second_type m_second;
};

} // namespace ecsl
#endif /* ECSL_UTILITY_COMPRESSED_PAIR_HPP_ */