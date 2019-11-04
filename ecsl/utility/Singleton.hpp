#ifndef ECSL_UTILITY_SINGLETON_HPP_
#define ECSL_UTILITY_SINGLETON_HPP_

/**
 * @file Singleton.hpp
 * Adds non-CRTP singleton implementation
 */

/// STD
#include <new>
#include <mutex>
#include <utility>
#include <stdexcept>
#include <type_traits>
/// ECSL
#include <ecsl/utility/Launder.hpp>
#include <ecsl/type_traits/DefaultTag.hpp>

namespace ecsl {

/**
 * Type of policy to be used in singleton template family
 */
enum class singleton_policy
{
    /**
     * Implements access to singleton object with proper checks of it's
     * lifetime. May not be sutable for overaligned types
     */
    SAFE,
    /**
     * Same as SAFE but throws when object is accessed not within it's lifetime
     */
    SAFE_THROWING,
    /**
     * Implements access to singleton object without any checks of it's
     * lifetime. Sutable for overaligned types
     */
    NOT_SAFE,
};

namespace detail {
namespace singleton {

/**
 * Simple trait to clean object type from various qualifiers and define
 * sutable storage for object binary representation
 */
template<class T>
struct value_trait
{
    using value_type    = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
    using reference     = typename std::add_lvalue_reference<value_type>::type;
    using pointer       = typename std::add_pointer<value_type>::type;
    using storage_type  = typename std::aligned_storage<sizeof(value_type), alignof(value_type)>::type;
};

/**
 * Storage that implements singleton_policy::NOT_SAFE policy.
 * Uses launder to access object and do no check that it is within it's lifetime
 */
template<class T, singleton_policy policy = singleton_policy::SAFE>
class storage
{
    using vt_t = value_trait<T>;
    typename vt_t::storage_type m_storage;

  public:
    using value_type    = typename vt_t::value_type;
    using reference     = typename vt_t::reference;
    using pointer       = typename vt_t::pointer;

    storage() = default;

    inline reference instance() noexcept
    {
        return *launder<pointer>(&m_storage);
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
 * Storage that implements singleton_policy::SAFE policy.
 * Uses pointer member to access object and checks lifetime of object when
 * constructed (explicit call to construct) and destructed
 */
template<class T>
class storage<T, singleton_policy::SAFE>
{
    using vt_t = value_trait<T>;
    typename vt_t::pointer m_pointer;
    typename vt_t::storage_type m_storage;

  public:
    using value_type    = typename vt_t::value_type;
    using reference     = typename vt_t::reference;
    using pointer       = typename vt_t::pointer;

    storage() noexcept : m_pointer{nullptr}, m_storage{} {}

    inline reference instance() noexcept
    {   //? This will trigger SIGSEGV if singleton was not initialized
        return *m_pointer;
    }

    template<class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type construct(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        if (!m_pointer)
            m_pointer = new(&m_storage) value_type(std::forward<Args>(args)...);
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
 * Storage that implements singleton_policy::SAFE_THROWING policy.
 * Same as storage with singleton_policy::SAFE, but throws when object is
 * accessed via instance and not within it's lifetime where
 * storage with singleton_policy::SAFE calls segfault.
 */
template<class T>
class storage<T, singleton_policy::SAFE_THROWING>
{
    using vt_t = value_trait<T>;
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

    inline reference instance()
    {
        if (!m_pointer)
            throw lifetime_error{"Stored object is not within it's lifetime"};
        return *m_pointer;
    }

    template<class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type construct(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        if (!m_pointer)
            m_pointer = new(&m_storage) value_type(std::forward<Args>(args)...);
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

} // namespace singleton
} // namespace detail

/**
 * @brief Non-CRTP singleton implementation.
 * Provides global storage for a single object of specified type T
 * @tparam T Type of object
 * @tparam TagType Tag type to be used where two or more instances of singleton
 *  with same object type T is needed
 * @tparam policy Storage policy
 */
template<
    class T,
    class TagType = default_tag,
    singleton_policy policy = singleton_policy::SAFE
>
class singleton
{
    using storeage_t = detail::singleton::storage<T, policy>;
    storeage_t m_storage;

    static singleton& get_instance() noexcept
    {
        static singleton g_singleton;
        return g_singleton;
    }

  public:
    using value_type    = typename storeage_t::value_type;
    using reference     = typename storeage_t::reference;
    using pointer       = typename storeage_t::pointer;
    using tag_type      = TagType;

    static reference instance()
        noexcept(noexcept(storeage_t().instance()))
    {
        return get_instance().m_storage.instance();
    }

    static void destroy()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        get_instance().m_storage.destroy();
    }

    template<class ... Args>
    static typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type initialize(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        return get_instance().m_storage.construct(std::forward<Args>(args)...);
    }
};

/**
 * @brief Non-CRTP singleton implementation that protects access to object
 * with mutex
 * Provides global storage for a single object of specified type T and a mutex
 * to serrialaze access to that object
 * @tparam T Type of object
 * @tparam Mutex Type of mutex to be used (must satisfy lockable concept)
 * @tparam TagType Tag type to be used where two or more instances of singleton
 *  with same object type T is needed
 * @tparam policy Storage policy
 */
template<
    class T,
    class Mutex = std::mutex,
    class TagType = default_tag,
    singleton_policy policy = singleton_policy::SAFE
>
class mutex_protected_singleton
{
    using storeage_t = detail::singleton::storage<T, policy>;
    Mutex m_mu;
    storeage_t m_storage;

    static mutex_protected_singleton& get_instance() noexcept
    {
        static mutex_protected_singleton g_singleton;
        return g_singleton;
    }

  public:
    using value_type    = typename storeage_t::value_type;
    using reference     = typename storeage_t::reference;
    using pointer       = typename storeage_t::pointer;
    using tag_type      = TagType;
    using mutex_type    = Mutex;
    using lock_type     = std::unique_lock<mutex_type>;
    using accessor_type = std::pair<reference, lock_type>;

    static accessor_type instance()
        noexcept(noexcept(storeage_t().instance()))
    {
        auto& s = get_instance();
        auto l = lock_type(s.m_mu);
        return {s.m_storage.instance(), std::move(l)};
    }

    template<class lock_policy = std::try_to_lock_t>
    static accessor_type instance(lock_policy lp)
        noexcept(noexcept(storeage_t().instance()))
    {
        auto& s = get_instance();
        auto l = lock_type(s.m_mu, lp);
        return {s.m_storage.instance(), std::move(l)};
    }

    static void destroy()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        auto& s = get_instance();
        auto l = lock_type(s.m_mu);
        s.m_storage.destroy();
    }

    template<class ... Args>
    static typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        accessor_type
    >::type initialize(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        auto& s = get_instance();
        auto l = lock_type(s.m_mu);
        auto& obj = s.m_storage.construct(std::forward<Args>(args)...);
        return {obj, std::move(l)};
    }
};

/**
 * @brief Non-CRTP singleton implementation that creates one singleton object
 * instance per program thread running
 * Provides thread local storage for a single object of specified type T
 * @tparam T Type of object
 * @tparam TagType Tag type to be used where two or more instances of singleton
 *  with same object type T is needed
 * @tparam policy Storage policy
 */
template<
    class T,
    class TagType = default_tag,
    singleton_policy policy = singleton_policy::SAFE
>
class thread_local_singleton
{
    using storeage_t = detail::singleton::storage<T, policy>;
    storeage_t m_storage;

    static thread_local_singleton& get_instance() noexcept
    {
        static thread_local thread_local_singleton tls_singleton;
        return tls_singleton;
    }

  public:
    using value_type    = typename storeage_t::value_type;
    using reference     = typename storeage_t::reference;
    using pointer       = typename storeage_t::pointer;
    using tag_type      = TagType;

    static reference instance()
        noexcept(noexcept(storeage_t().instance()))
    {
        return get_instance().m_storage.instance();
    }

    static void destroy()
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        get_instance().m_storage.destroy();
    }

    template<class ... Args>
    static typename std::enable_if<
        std::is_constructible<value_type, Args...>::value,
        reference
    >::type initialize(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args...>::value)
    {
        return get_instance().m_storage.construct(std::forward<Args>(args)...);
    }
};

} // namespace ecsl
#endif /* ECSL_UTILITY_SINGLETON_HPP_ */