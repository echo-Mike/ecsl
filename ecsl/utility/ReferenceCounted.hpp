#ifndef ECSL_UTILITY_REFERENCE_COUNTED_HPP_
#define ECSL_UTILITY_REFERENCE_COUNTED_HPP_

/**
 * @file ReferenceCounted.hpp
 * Adds support for non-atomically reference counted types using CRTP
 * and smart pointer for lifetime management
 */

/// STD
#include <cstdint>
#include <memory>
#include <utility>
#include <type_traits>
/// ECSL
#include <ecsl/utility/CompressedPair.hpp>

namespace ecsl {

/**
 * @brief Makes any class reference counted using CRTP
 * @tparam Derived Class to reference count
 * @tparam Deleter Class to dispose of managed object
 */
template<class Derived, class Deleter = std::default_delete<Derived>>
class intrusive_reference_counter
{
    using compressed_t = compressed_pair<std::size_t, Deleter>;
    mutable compressed_t m_data;

    std::size_t& get_counter_() const noexcept
    {
        return m_data.get_first();
    }

    Deleter& get_deleter_() const noexcept
    {
        return m_data.get_second();
    }

  public:
    using value_type = Derived;
    using deleter_type = Deleter;
    using pointer = typename std::add_pointer<value_type>::type;
    using const_pointer = typename std::add_const<
        typename std::add_pointer<value_type>::type>::type;

    template<class = std::enable_if<
        std::is_default_constructible<compressed_t>::value
    >::type>
    intrusive_reference_counter()
        noexcept(std::is_nothrow_default_constructible<compressed_t>::value) :
        m_data{}
    {
        get_counter_() = 0;
    }

    template<class = std::enable_if<
        std::is_copy_constructible<deleter_type>::value
    >::type>
    explicit intrusive_reference_counter(const deleter_type& d)
        noexcept(std::is_nothrow_copy_constructible<deleter_type>::value) :
        m_data{0, d}
    {}
    template<class = std::enable_if<
        std::is_move_constructible<deleter_type>::value
    >::type>
    explicit intrusive_reference_counter(deleter_type&& d)
        noexcept(std::is_nothrow_move_constructible<deleter_type>::value) :
        m_data{0, std::move(d)}
    {}

    template<class = std::enable_if<
        std::is_copy_constructible<compressed_t>::value
    >::type>
    intrusive_reference_counter(const intrusive_reference_counter& other)
        noexcept(std::is_nothrow_copy_constructible<compressed_t>::value) :
        m_data{other.m_data}
    {
        get_counter_() = 0;
    }
    template<class = std::enable_if<
        std::is_copy_assignable<compressed_t>::value
    >::type>
    intrusive_reference_counter& operator=(
        const intrusive_reference_counter& other
    ) noexcept(std::is_nothrow_copy_assignable<compressed_t>::value)
    {
        auto ref_count_ = get_counter_();
        m_data = other.m_data;
        get_counter_() = ref_count_;
        return *this;
    }

    template<class = std::enable_if<
        std::is_move_constructible<compressed_t>::value
    >::type>
    intrusive_reference_counter(intrusive_reference_counter&& other)
        noexcept(std::is_nothrow_move_constructible<compressed_t>::value) :
        m_data{std::move(other.m_data)}
    {
        get_counter_() = 0;
    }
    template<class = std::enable_if<
        std::is_move_assignable<compressed_t>::value
    >::type>
    intrusive_reference_counter& operator=(
        intrusive_reference_counter&& other
    ) noexcept(std::is_nothrow_move_assignable<compressed_t>::value)
    {
        auto ref_count_ = get_counter_();
        m_data = std::move(other.m_data);
        get_counter_() = ref_count_;
        return *this;
    }

    inline pointer acquire() noexcept
    {
        ++get_counter_();
        return static_cast<pointer>(this);
    }
    inline const_pointer acquire() const noexcept
    {
        ++get_counter_();
        return static_cast<const_pointer>(this);
    }

    inline void retain() const noexcept
    {
        ++get_counter_();
    }

    // TODO: add noexcept clause here
    inline void release()
    {
        auto& ref_count_ = get_counter_();
        if (!ref_count_ || !(--ref_count_))
        {
            get_deleter_()(static_cast<pointer>(this));
        }
    }

    // TODO: add noexcept clause here
    inline void release() const
    {
        auto& ref_count_ = get_counter_();
        if (!ref_count_ || !(--ref_count_))
        {
            get_deleter_()(static_cast<const_pointer>(this));
        }
    }

    inline friend void swap(
        intrusive_reference_counter& lhs,
        intrusive_reference_counter& rhs
    ) noexcept
    {
        using std::swap;
        swap(lhs.get_deleter_(), rhs.get_deleter_());
    }
};

/**
 * @brief Tag type to signify adoption semantics
 */
struct adopt_reference {};

/**
 * @brief Smart pointer for reference counted types
 */
template<class T>
class reference_counter_pointer
{
  public:
    using value_type = T;
    using pointer = typename std::add_pointer<value_type>::type;
    using const_pointer = typename std::add_pointer<
        typename std::add_const<value_type>::type>::type;
    using reference = typename std::add_lvalue_reference<value_type>::type;
    using const_reference = typename std::add_lvalue_reference<
        typename std::add_const<value_type>::type>::type;

    reference_counter_pointer() noexcept :
        m_object{nullptr}
    {}
    explicit reference_counter_pointer(std::nullptr_t) noexcept :
        m_object{nullptr}
    {}
    explicit reference_counter_pointer(pointer obj) noexcept :
        m_object{obj ? obj->acquire() : nullptr}
    {}
    reference_counter_pointer(pointer obj, adopt_reference) noexcept :
        m_object{obj}
    {}
    reference_counter_pointer(const reference_counter_pointer& other) noexcept :
        m_object{!other ? nullptr : other.m_object->acquire()}
    {}
    reference_counter_pointer(reference_counter_pointer&& other) noexcept :
        m_object{other.m_object}
    {
        other.m_object = nullptr;
    }

    reference_counter_pointer& operator=(const reference_counter_pointer& other)
    {
        if (m_object)
        {
            m_object->release();
        }
        if (!!other)
        {
            m_object = other.m_object->acquire();
        }
        return *this;
    }
    reference_counter_pointer& operator=(reference_counter_pointer&& other)
    {
        if (m_object)
        {
            m_object->release();
        }
        m_object = other.m_object;
        other.m_object = nullptr;
        return *this;
    }

    ~reference_counter_pointer()
    {
        if (m_object)
        {
            m_object->release();
        }
    }

    inline bool operator!() const noexcept { return !m_object; }

    inline pointer operator->() noexcept { return m_object; }
    inline const_pointer operator->() const noexcept { return m_object; }

    inline reference operator*() noexcept { return *m_object; }
    inline const_reference operator*() const noexcept { return *m_object; }

    inline pointer release() noexcept
    {
        auto tmp_ = m_object;
        m_object = nullptr;
        return tmp_;
    }

    inline void swap(reference_counter_pointer& other) noexcept
    {
        using std::swap;
        swap(m_object, other.m_object);
    }

    inline friend void swap(
        reference_counter_pointer& lhs,
        reference_counter_pointer& rhs
    ) noexcept
    {
        lhs.swap(rhs);
    }

  private:
    pointer m_object;
};

namespace detail {
namespace ref_counted {

struct true_type { char a; };
struct false_type { char a[2]; };

auto check_ref_derived_(...) -> false_type;
template<class D, class Del>
auto check_ref_derived_(intrusive_reference_counter<D, Del>*) -> true_type;

template<class T, class TP = typename std::add_pointer<T>::type>
using is_derived_from_irc_ = typename std::conditional<
    sizeof(check_ref_derived_(TP{nullptr})) == sizeof(true_type),
    std::true_type,
    std::false_type
>::type;

} // namespace ref_counted
} // namespace detail

/**
 * @brief Obtains smart pointer to reference counted type
 */
template<class T>
typename std::enable_if<
    detail::ref_counted::is_derived_from_irc_<T>::value,
    reference_counter_pointer<T>
>::type make_ref_count_ptr(T* obj) noexcept
{
    return reference_counter_pointer<T>{obj};
}

/**
 * @brief Obtains smart pointer to reference counted type
 */
template<class T>
typename std::enable_if<
    detail::ref_counted::is_derived_from_irc_<T>::value,
    reference_counter_pointer<T>
>::type make_ref_count_ptr(T& obj) noexcept
{
    return reference_counter_pointer<T>{&obj};
}

/**
 * @brief Obtains smart pointer to reference counted type
 * Adopts reference
 */
template<class T>
typename std::enable_if<
    detail::ref_counted::is_derived_from_irc_<T>::value,
    reference_counter_pointer<T>
>::type make_ref_count_ptr(T* obj, adopt_reference) noexcept
{
    return reference_counter_pointer<T>{obj, adopt_reference{}};
}

/**
 * @brief Obtains smart pointer to reference counted type
 * Adopts reference
 */
template<class T>
typename std::enable_if<
    detail::ref_counted::is_derived_from_irc_<T>::value,
    reference_counter_pointer<T>
>::type make_ref_count_ptr(T& obj, adopt_reference) noexcept
{
    return reference_counter_pointer<T>{&obj, adopt_reference{}};
}

} // namespace ecsl
#endif /* ECSL_UTILITY_REFERENCE_COUNTED_HPP_ */