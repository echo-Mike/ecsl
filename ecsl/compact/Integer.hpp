#ifndef ECSL_COMPACT_INTEGER_HPP_
#define ECSL_COMPACT_INTEGER_HPP_

/**
 * @file Integer.hpp
 * Declares utility class for compact (non-aligned) representation of an integer
 */

/// STD
#include <type_traits>
/// ECSL
#include <ecsl/compact/detail/Storage.hpp>

namespace ecsl {

/**
 * @brief Compact (not-aligned) representation of any integral type
 * @tparam T Integral type
 */
template<class T>
class compact_integer
{
    detail::compact::storage<T> m_storage;

  public:
    using value_type    = T;
    using reference     = typename std::add_lvalue_reference<value_type>::type;

    compact_integer() noexcept { store(value_type(0)); }
    template<class U, class = typename std::enable_if<
        std::is_convertible<U, value_type>::value
    >::type>
    explicit compact_integer(U other) noexcept { store(other); }

    compact_integer& operator=(value_type value) noexcept
    {
        store(value);
        return *this;
    }

    inline void store(value_type value) noexcept
    {
        m_storage.store(value);
    }

    inline value_type load() const noexcept
    {
        return m_storage.load();
    }

    inline operator value_type() const noexcept { return load(); }

    /* Arithmetic operators */

    /* Inc/Dec */

    inline compact_integer& operator++() noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v) { ++v; });
        return *this;
    }
    inline compact_integer operator++(int) noexcept
    {
        compact_integer tmp(*this);
        operator++();
        return tmp;
    }

    inline compact_integer& operator--() noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v) { --v; });
        return *this;
    }
    inline compact_integer operator--(int) noexcept
    {
        compact_integer tmp(*this);
        operator--();
        return tmp;
    }

    /* Unary */

    inline compact_integer operator+() const noexcept
    {
        return compact_integer(+load());
    }
    inline compact_integer operator-() const noexcept
    {
        return compact_integer(-load());
    }

    /* Bitwise unary */

    inline compact_integer operator~() const noexcept
    {
        return compact_integer(~load());
    }

    /* Binary */

    inline friend compact_integer operator+(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs += rhs;
        return lhs;
    }
    inline friend compact_integer operator-(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs -= rhs;
        return lhs;
    }
    inline friend compact_integer operator*(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs *= rhs;
        return lhs;
    }
    inline friend compact_integer operator/(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs /= rhs;
        return lhs;
    }
    inline friend compact_integer operator%(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs %= rhs;
        return lhs;
    }

    /* Bitwise binary */

    inline friend compact_integer operator&(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs &= rhs;
        return lhs;
    }
    inline friend compact_integer operator|(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs |= rhs;
        return lhs;
    }
    inline friend compact_integer operator^(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs ^= rhs;
        return lhs;
    }
    inline friend compact_integer operator<<(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs <<= rhs;
        return lhs;
    }
    inline friend compact_integer operator>>(
        compact_integer lhs, const compact_integer& rhs) noexcept
    {
        lhs >>= rhs;
        return lhs;
    }

    /* Compound assignment += -= /= *= %= &= |= ^= <<= >>= */

    inline compact_integer& operator+=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v += n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator-=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v -= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator/=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v /= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator*=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v *= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator%=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v %= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator&=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v &= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator|=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v |= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator^=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v ^= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator<<=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v <<= n; },
            a
        );
        return *this;
    }
    inline compact_integer& operator>>=(value_type a) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](reference v, value_type n) { v >>= n; },
            a
        );
        return *this;
    }

    /* Comparison operators */

    inline friend bool operator==(
        const compact_integer& lhs, const compact_integer& rhs) noexcept
    {
        return lhs.load() == rhs.load();
    }
    inline friend bool operator!=(
        const compact_integer& lhs, const compact_integer& rhs) noexcept
    {
        return !(lhs == rhs);
    }
    inline friend bool operator<(
        const compact_integer& lhs, const compact_integer& rhs) noexcept
    {
        return lhs.load() < rhs.load();
    }
    inline friend bool operator>(
        const compact_integer& lhs, const compact_integer& rhs) noexcept
    {
        return rhs < lhs;
    }
    inline friend bool operator>=(
        const compact_integer& lhs, const compact_integer& rhs) noexcept
    {
        return !(lhs < rhs);
    }
    inline friend bool operator<=(
        const compact_integer& lhs, const compact_integer& rhs) noexcept
    {
        return !(lhs > rhs);
    }
};

/**
 * @brief make_compact overload for integral types
 */
template<class T>
inline typename std::enable_if<
    std::is_integral<T>::value,
    compact_integer<T>
>::type make_compact(T value) noexcept
{
    return compact_integer<T>{value};
}

} // namespace ecsl
#endif /* ECSL_COMPACT_INTEGER_HPP_ */