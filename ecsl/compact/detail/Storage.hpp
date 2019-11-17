#ifndef ECSL_COMPACT_DETAIL_STORAGE_HPP_
#define ECSL_COMPACT_DETAIL_STORAGE_HPP_

/// STD
#include <cstring>
#include <utility>
#include <type_traits>
/// ECSL
#include <ecsl/type_traits/SimpleTypes.hpp>

namespace ecsl {
namespace detail {
namespace compact {

struct as_result {};
struct as_param {};

template<class T>
struct storage
{
    using value_type = T;

    template<class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args&&...>::value
    >::type store(Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args&&...>::value)
    {
        value_type tmp_(std::forward<Args>(args)...);
        std::memcpy(m_data, &tmp_, sizeof(m_data));
    }

    template<class = typename std::enable_if<
        std::is_default_constructible<value_type>::value
    >::type>
    inline value_type load() const
        noexcept(std::is_nothrow_default_constructible<value_type>::value)
    {
        value_type tmp_;
        std::memcpy(&tmp_, m_data, sizeof(m_data));
        return tmp_;
    }

    template<class F, class ... Args>
    inline void modify(as_param, F&& func, Args&&... args)
        // TODO: add noexcept clause
    {
        auto tmp_ = load();
        func(tmp_, std::forward<Args>(args)...);
        store(std::move(tmp_));
    }

    template<class F, class ... Args>
    inline void modify(as_result, F&& func, Args&&... args)
        // TODO: add noexcept clause
    {
        store(func(load(), std::forward<Args>(args)...));
    }

    types::memory_t m_data[sizeof(value_type)];
};

} // namespace compact
} // namespace detail
} // namespace ecsl
#endif /* ECSL_COMPACT_DETAIL_STORAGE_HPP_ */