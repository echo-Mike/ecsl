#ifndef ECSL_COMPACT_POINTER_HPP_
#define ECSL_COMPACT_POINTER_HPP_

/**
 * @file Pointer.hpp
 * Declares utility class for compact (non-aligned) representation of a pointer
 */

/// STD
#include <type_traits>
/// ECSL
#include <ecsl/compact/detail/Storage.hpp>

namespace ecsl {

/**
 * @brief Compact (not-aligned) representation of any raw pointer
 * @tparam T Type, to point to
 * @warning compact_pointer<const T> and compact_pointer<T> may not successfully
 * interoperate (no implicit conversion operations are provided)
 */
template<class T>
class compact_pointer
{
    detail::compact::storage<T> m_storage;

  public:
    using value_type    = T;
    using pointer       = typename std::add_pointer<value_type>::type;
    using reference     = typename std::add_lvalue_reference<value_type>::type;

    compact_pointer() noexcept { store(pointer(nullptr)); }
    explicit compact_pointer(std::nullptr_t) noexcept : compact_pointer() {}
    explicit compact_pointer(pointer ptr) noexcept { store(ptr); }

    compact_pointer& operator=(pointer ptr) noexcept
    {
        store(ptr);
        return *this;
    }

    /* Make "/*" => "//*" to uncomment :
    /// This definitions privent generation of copy constructor
    /// and copy assignment operator for non-const value_type.
    /// Uncomment if You need implicit conversion behavior between const
    /// and non-const compact_pointer

    template<class U, class = typename std::enable_if<
        std::is_const<value_type>::value &&
        std::is_same<typename std::remove_const<value_type>::type, U>::value
    >::type>
        compact_pointer(const compact_pointer<U>& other) noexcept
    {
        store(other.load());
    }

    template<class U, class = typename std::enable_if<
        std::is_const<value_type>::value &&
        std::is_same<typename std::remove_const<value_type>::type, U>::value
    >::type>
    compact_pointer& operator=(const compact_pointer<U>& other) noexcept
    {
        store(other.load());
        return *this;
    }

    //*/

    inline void store(pointer ptr) noexcept
    {
        m_storage.store(ptr);
    }

    inline pointer load() const noexcept
    {
        return m_storage.load();
    }

    /* Arithmetic operators */

    /* Inc/Dec */

    inline compact_pointer& operator++() noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](pointer& ptr) { ++ptr; });
        return *this;
    }
    inline compact_pointer operator++(int) noexcept
    {
        compact_pointer tmp(*this);
        operator++();
        return tmp;
    }

    inline compact_pointer& operator--() noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](pointer& ptr) { --ptr; });
        return *this;
    }
    inline compact_pointer operator--(int) noexcept
    {
        compact_pointer tmp(*this);
        operator--();
        return tmp;
    }

    /* Compound assignment += -= */

    inline compact_pointer& operator+=(std::ptrdiff_t n) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](pointer& ptr, std::ptrdiff_t a) { ptr += a; },
            n
        );
        return *this;
    }
    inline compact_pointer& operator-=(std::ptrdiff_t n) noexcept
    {
        m_storage.modify(detail::compact::as_param{},
            [](pointer& ptr, std::ptrdiff_t a) { ptr -= a; },
            n
        );
        return *this;
    }

    /* Binary operators */

    inline friend std::ptrdiff_t operator-(
        compact_pointer lhs, const compact_pointer& rhs) noexcept
    {
        return lhs.load() - rhs.load();
    }

    inline pointer operator->() const noexcept { return load(); }

    inline reference operator*() const noexcept { return *load(); }

    inline operator pointer() const noexcept { return load(); }

    /**
     * operator[] and other binary operators (a + n, a - n)
     * are automatically implemented due to implicit conversion
     * to pointer type. That way:
     * operator bool, operator!, operator||, operator&&
     * saves their semantics and short circuiting
     */

    /* Comparison operators */

    inline friend bool operator==(
        const compact_pointer& lhs, const compact_pointer& rhs) noexcept
    {
        return lhs.load() == rhs.load();
    }
    inline friend bool operator!=(
        const compact_pointer& lhs, const compact_pointer& rhs) noexcept
    {
        return !(lhs == rhs);
    }
    inline friend bool operator<(
        const compact_pointer& lhs, const compact_pointer& rhs) noexcept
    {
        return lhs.load() < rhs.load();
    }
    inline friend bool operator>(
        const compact_pointer& lhs, const compact_pointer& rhs) noexcept
    {
        return rhs < lhs;
    }
    inline friend bool operator>=(
        const compact_pointer& lhs, const compact_pointer& rhs) noexcept
    {
        return !(lhs < rhs);
    }
    inline friend bool operator<=(
        const compact_pointer& lhs, const compact_pointer& rhs) noexcept
    {
        return !(lhs > rhs);
    }
};

/**
 * @brief make_compact overload for pointer types
 */
template<class T>
inline typename std::enable_if<
    std::is_pointer<T>::value,
    compact_pointer<T>
>::type make_compact(T ptr) noexcept
{
    return compact_pointer<T>{ptr};
}

} // namespace ecsl
#endif /* ECSL_COMPACT_POINTER_HPP_ */