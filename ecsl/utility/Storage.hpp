#ifndef ECSL_UTILITY_STORAGE_HPP_
#define ECSL_UTILITY_STORAGE_HPP_

/**
 * @file Storage.hpp
 * Adds class that handles single object lifetime in multiple manners
 */

/// STD
#include <new>
#include <utility>
#include <stdexcept>
#include <type_traits>
/// ECSL
#include <ecsl/utility/Launder.hpp>

namespace ecsl {
namespace detail {
namespace storage {

/**
 * Simple trait to clean object type from various qualifiers and define
 * sutable storage for object binary representation
 */
template<class T>
struct value_trait
{
    using value_type    = typename std::remove_reference<T>::type;
    using reference     = typename std::add_lvalue_reference<value_type>::type;
    using pointer       = typename std::add_pointer<value_type>::type;
    using storage_type  = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;
};

} // namespace storage
} // namespace detail

/**
 * Type of policy that defines behaviour of storage template family
 */
enum class storage_policy
{
    /**
     * Implements access to stored object with proper checks of it's
     * lifetime. May not be sutable for overaligned types
     */
    SAFE,
    /**
     * Same as SAFE but throws when object is accessed not within it's lifetime
     */
    SAFE_THROWING,
    /**
     * Implements access to stored object without any checks of it's
     * lifetime. Sutable for overaligned types
     */
    NOT_SAFE,
};

/**
 * @brief Storage that implements storage_policy::NOT_SAFE policy.
 * Uses launder to access object and do no check that it is within it's lifetime
 */
template<class T, storage_policy policy = storage_policy::SAFE>
class storage
{
    using vt_t = detail::storage::value_trait<T>;
    typename vt_t::storage_type m_storage;

  public:
    using value_type    = typename vt_t::value_type;
    using reference     = typename vt_t::reference;
    using pointer       = typename vt_t::pointer;

    storage() = default;

    inline void* get_raw() noexcept { return &m_storage; }
    inline const void* get_raw() const noexcept { return &m_storage; }

    inline pointer get_pointer() noexcept
    {
        return launder<pointer>(&m_storage);
    }

    inline reference get_reference() noexcept
    {
        return *get_pointer();
    }

    template<class U>
    inline typename std::enable_if<
        std::is_assignable<value_type, U>::value
        reference
    >::type assign(U&& arg)
        noexcept(std::is_nothrow_assignable<value_type, U>::value)
    {
        auto& obj = get_reference();
        obj = std::forward<U>(arg);
        return obj;
    }

    template<class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type construct(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        auto* obj_ptr = new(&m_storage) value_type(std::forward<Args>(args)...);
        return *obj_ptr;
    }

    inline void destroy()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        launder<pointer>(&m_storage)->~value_type();
    }

    ~storage()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        destroy();
    }
};

/**
 * @brief Storage that implements storage_policy::SAFE policy.
 * Uses pointer member to access object and checks lifetime of object when
 * constructed (explicit call to construct) and destructed
 */
template<class T>
class storage<T, storage_policy::SAFE>
{
    using vt_t = detail::storage::value_trait<T>;
    typename vt_t::pointer m_pointer;
    typename vt_t::storage_type m_storage;

  public:
    using value_type    = typename vt_t::value_type;
    using reference     = typename vt_t::reference;
    using pointer       = typename vt_t::pointer;

    storage() noexcept : m_pointer{nullptr}, m_storage{} {}

    inline void* get_raw() noexcept { return &m_storage; }
    inline const void* get_raw() const noexcept { return &m_storage; }

    inline pointer get_pointer() noexcept { return m_pointer; }

    inline reference get_reference() noexcept
    {   //? This will trigger SIGSEGV if object is not within it's lifetime
        return *get_pointer();
    }

    template<class U>
    inline typename std::enable_if<
        std::is_assignable<value_type, U>::value
        reference
    >::type assign(U&& arg)
        noexcept(std::is_nothrow_assignable<value_type, U>::value)
    {
        auto& obj = get_reference();
        obj = std::forward<U>(arg);
        return obj;
    }

    template<class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type construct(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        if (!m_pointer)
        {
            m_pointer = new(&m_storage) value_type(std::forward<Args>(args)...);
        }
        return *m_pointer;
    }

    inline void destroy()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        if (m_pointer)
        {
            m_pointer->~value_type();
            m_pointer = nullptr;
        }
    }

    ~storage()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        destroy();
    }
};

/**
 * @brief Storage that implements storage_policy::SAFE_THROWING policy.
 * Same as storage with storage_policy::SAFE, but throws when object is
 * accessed via instance and not within it's lifetime where
 * storage with storage_policy::SAFE calls segfault.
 */
template<class T>
class storage<T, storage_policy::SAFE_THROWING>
{
    using vt_t = detail::storage::value_trait<T>;
    typename vt_t::pointer m_pointer;
    typename vt_t::storage_type m_storage;

  public:
    using value_type    = typename vt_t::value_type;
    using reference     = typename vt_t::reference;
    using pointer       = typename vt_t::pointer;

    class lifetime_error : public std::runtime_error
    {
      public:
        explicit lifetime_error(const std::string& str) :
            std::runtime_error(str) {}
        explicit lifetime_error(const char* c_str) :
            std::runtime_error(c_str) {}
    };

    storage() noexcept : m_pointer{nullptr}, m_storage{} {}

    inline void* get_raw() noexcept { return &m_storage; }
    inline const void* get_raw() const noexcept { return &m_storage; }

    inline pointer get_pointer() noexcept { return m_pointer; }

    inline reference get_reference()
    {
        auto* obj_ptr = get_pointer();
        if (!obj_ptr)
            throw lifetime_error{"Stored object is not within it's lifetime"};
        return *obj_ptr;
    }

    template<class U>
    inline typename std::enable_if<
        std::is_assignable<value_type, U>::value
        reference
    >::type assign(U&& arg)
    {
        auto& obj = get_reference();
        obj = std::forward<U>(arg);
        return obj;
    }

    template<class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type construct(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        if (!m_pointer)
        {
            m_pointer = new(&m_storage) value_type(std::forward<Args>(args)...);
        }
        return *m_pointer;
    }

    inline void destroy()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        if (m_pointer)
        {
            m_pointer->~value_type();
            m_pointer = nullptr;
        }
    }

    ~storage()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        destroy();
    }
};

} // namespace ecsl
#endif /* ECSL_UTILITY_STORAGE_HPP_ */