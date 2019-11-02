#ifndef ECSL_TYPE_TRAITS_DISTINCT_INTEGER_HPP_
#define ECSL_TYPE_TRAITS_DISTINCT_INTEGER_HPP_

/**
 * @file DistinctInteger.hpp
 * Adds integer-like time not convertible to any other integer type
 */

/// STD
#include <type_traits>
/// ECSL
#include <ecsl/type_traits/DefaultTag.hpp>

namespace ecsl {

/**
 * Type that represents an integer type not implicitly convertible to any other
 * integer type except itself.
 * @tparam T Underlying integer type
 * @tparam TagType Tag type to make two different distinct_integer types with
 * same underlying integer type
 */
template<class T, class TagType = default_tag>
class distinct_integer
{
    static_assert(std::is_integral<T>::value, "T must be an integral type");
  public:
    using value_type    = typename std::remove_cv<T>::type;
    using tag_type      = TagType;

    constexpr distinct_integer() noexcept : m_integer{0} {}
    constexpr explicit distinct_integer(value_type value) noexcept :
        m_integer{value}
    {}
    template<class U,
        class = typename std::enable_if<
            !std::is_same<T,U>::value &&
            std::is_convertible<U, value_type>::value
        >::type
    >
    constexpr explicit distinct_integer(U other_value) noexcept :
        m_integer(other_value)
    {}

    constexpr distinct_integer& operator=(value_type value) noexcept
    {
        return (m_integer = value), *this;
    }

    constexpr value_type value() const noexcept
    {
        return m_integer;
    }

    /* Arithmetic operators */

    /* Inc/Dec */

    constexpr distinct_integer& operator++() noexcept
    {
        return ++m_integer, *this;
    }
    constexpr distinct_integer operator++(int) noexcept
    {
        return distinct_integer(m_integer++);
    }

    constexpr distinct_integer& operator--() noexcept
    {
        return --m_integer, *this;
    }
    constexpr distinct_integer operator--(int) noexcept
    {
        return distinct_integer(m_integer--);
    }

    /* Unary */

    constexpr distinct_integer operator+() const noexcept
    {
        return distinct_integer(+m_integer);
    }
    constexpr distinct_integer operator-() const noexcept
    {
        return distinct_integer(-m_integer);
    }

    /* Bitwise unary */

    constexpr distinct_integer operator~() const noexcept
    {
        return distinct_integer(~m_integer);
    }

    /* Logic unary */

    constexpr bool operator!() const noexcept
    {
        return !(!!m_integer);
    }

    /* Binary */

    constexpr friend distinct_integer operator+(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs += rhs;
    }
    constexpr friend distinct_integer operator-(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs -= rhs;
    }
    constexpr friend distinct_integer operator*(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs *= rhs;
    }
    constexpr friend distinct_integer operator/(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs /= rhs;
    }
    constexpr friend distinct_integer operator%(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs %= rhs;
    }

    /* Bitwise binary */

    constexpr friend distinct_integer operator&(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs &= rhs;
    }
    constexpr friend distinct_integer operator|(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs |= rhs;
    }
    constexpr friend distinct_integer operator^(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs ^= rhs;
    }
    constexpr friend distinct_integer operator<<(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs <<= rhs;
    }
    constexpr friend distinct_integer operator>>(
        distinct_integer lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs >>= rhs;
    }

    /* Compound assignment += -= /= *= %= &= |= ^= <<= >>= */

    constexpr distinct_integer& operator+=(distinct_integer rhs) noexcept
    {
        return (m_integer += rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator-=(distinct_integer rhs) noexcept
    {
        return (m_integer -= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator/=(distinct_integer rhs) noexcept
    {
        return (m_integer /= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator*=(distinct_integer rhs) noexcept
    {
        return (m_integer *= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator%=(distinct_integer rhs) noexcept
    {
        return (m_integer %= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator&=(distinct_integer rhs) noexcept
    {
        return (m_integer &= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator|=(distinct_integer rhs) noexcept
    {
        return (m_integer |= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator^=(distinct_integer rhs) noexcept
    {
        return (m_integer ^= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator<<=(distinct_integer rhs) noexcept
    {
        return (m_integer <<= rhs.m_integer), *this;
    }
    constexpr distinct_integer& operator>>=(distinct_integer rhs) noexcept
    {
        return (m_integer >>= rhs.m_integer), *this;
    }

    /* Comparison operators */

    constexpr friend bool operator==(
        const distinct_integer& lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs.m_integer == rhs.m_integer;
    }
    constexpr friend bool operator!=(
        const distinct_integer& lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return !(lhs == rhs);
    }
    constexpr friend bool operator<(
        const distinct_integer& lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return lhs.m_integer < rhs.m_integer;
    }
    constexpr friend bool operator>(
        const distinct_integer& lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return rhs < lhs;
    }
    constexpr friend bool operator>=(
        const distinct_integer& lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return !(lhs < rhs);
    }
    constexpr friend bool operator<=(
        const distinct_integer& lhs,
        const distinct_integer& rhs
    ) noexcept
    {
        return !(lhs > rhs);
    }

  private:
    value_type m_integer;
};

} // namespace ecsl
#endif /* ECSL_TYPE_TRAITS_DISTINCT_INTEGER_HPP_ */