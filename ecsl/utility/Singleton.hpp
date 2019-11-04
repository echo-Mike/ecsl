#ifndef ECSL_UTILITY_SINGLETON_HPP_
#define ECSL_UTILITY_SINGLETON_HPP_

/**
 * @file Singleton.hpp
 * Adds non-CRTP singleton implementation
 */

/// STD
#include <mutex>
#include <utility>
#include <type_traits>
/// ECSL
#include <ecsl/utility/Storage.hpp>
#include <ecsl/type_traits/DefaultTag.hpp>

namespace ecsl {

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
    storage_policy policy = storage_policy::SAFE
>
class singleton
{
    using storeage_t = storage<T, policy>;
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
        noexcept(noexcept(storeage_t().get_reference()))
    {
        return get_instance().m_storage.get_reference();
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
    storage_policy policy = storage_policy::SAFE
>
class mutex_protected_singleton
{
    using storeage_t = storage<T, policy>;
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
        noexcept(noexcept(storeage_t().get_reference()))
    {
        auto& s = get_instance();
        auto l = lock_type(s.m_mu);
        return {s.m_storage.get_reference(), std::move(l)};
    }

    template<class lock_policy = std::try_to_lock_t>
    static accessor_type instance(lock_policy lp)
        noexcept(noexcept(storeage_t().get_reference()))
    {
        auto& s = get_instance();
        auto l = lock_type(s.m_mu, lp);
        return {s.m_storage.get_reference(), std::move(l)};
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
    storage_policy policy = storage_policy::SAFE
>
class thread_local_singleton
{
    using storeage_t = storage<T, policy>;
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
        noexcept(noexcept(storeage_t().get_reference()))
    {
        return get_instance().m_storage.get_reference();
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