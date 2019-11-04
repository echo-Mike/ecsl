#ifndef ECSL_UTILITY_VOID_UNIQUE_POINTER_HPP_
#define ECSL_UTILITY_VOID_UNIQUE_POINTER_HPP_

/**
 * @file VoidUniquePointer.hpp
 * Provides unique pointer type, usable when heterogeneous resources must be
 * stored in single container.
 * Effectively enables the void pointer to be stored inside std::unique_ptr
 */

/// STD
#include <memory>
#include <utility>

namespace ecsl {
namespace detail {
namespace void_uptr {

template<class T>
void default_delete(void* p)
{
    if (p)
    {
        auto* p_ = static_cast<T*>(p);
        delete p_;
    }
}

template<class T>
void default_delete<T[]>(void* p)
{
    if (p)
    {
        auto* p_ = static_cast<T*>(p);
        delete[] p_;
    }
}

template<class = void> void noop_delete(void*) {}

using void_uptr_t = std::unique_ptr<void, void(*)(void*)>;

} // namespace void_uptr
} // namespace detail

/**
 * The unique pointer that invokes stored cleanup function when been destructed
 */
using void_uptr_t = detail::void_uptr::void_uptr_t;

/**
 * Creates void unique pointer from pointer to single object
 */
template<class T>
auto make_void(T* ptr) -> void_uptr_t
{
    return void_uptr_t(ptr, detail::void_uptr::default_delete<T>);
}

/**
 * Creates void unique pointer from pointer to array of objects.
 * The case for explicit template type specification on caller side
 */
template<class T>
auto make_void<T[]>(T* ptr) -> void_uptr_t
{
    return void_uptr_t(ptr, detail::void_uptr::default_delete<T[]>);
}

/**
 * Creates void unique pointer that stores nothing.
 * Usable to force resource release for some void unique pointer and for direct
 * initialization
 */
template<class = void>
auto make_void(std::nullptr_t) -> void_uptr_t
{
    return void_uptr_t(static_cast<void*>(nullptr), detail::void_uptr::noop_delete);
}

/**
 * Creates void unique pointer from pointer to object and non-default cleanup
 * function (function-object)
 */
template<class T, class DeleterFunc>
auto make_void(T* ptr, DeleterFunc&& df) -> void_uptr_t
{
    return void_uptr_t(ptr, std::forward<DeleterFunc>(df));
}

/**
 * Creates void unique pointer from pointer to array of objects
 */
template<class T>
auto make_void_array(T* ptr) -> void_uptr_t
{
    return void_uptr_t(ptr, detail::void_uptr::default_delete<T[]>);
}
} // namespace ecsl
#endif /* ECSL_UTILITY_VOID_UNIQUE_POINTER_HPP_ */