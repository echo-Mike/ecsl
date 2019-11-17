#ifndef ECSL_UTILITY_STATE_POINTER_HPP_
#define ECSL_UTILITY_STATE_POINTER_HPP_

/**
 * @file StatePointer.hpp
 * Adds the compact pointer/integer pair where integer is limited by alignment
 * of type of an object the pointer points to
 */

/// STD
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <type_traits>
/// ECSL
#include <ecsl/type_traits/SimpleTypes.hpp>
#include <ecsl/type_traits/MinimalInteger.hpp>

namespace ecsl {

/**
 * @brief Class that holds pointer and some amount of associated state.
 * The state held is limited by the alignment of T as it is stored inside empty
 * bits of pointer. It is assumed that pointer has a value representation that
 * will respect the alignment requirements of T
 *
 * State must be representable by the integer in range [0, alignof(T))
 *
 * If missalign pointer or out of range state is provided the values are
 * corrected automatically
 *
 * @tparam T The type to point to
 */
template<class T>
class state_pointer
{
    using memory_t = types::memory_t;
    static_assert(has_unsigned_minimal_integer<T*>::value,
        "Pointer can't be represented as unsigned integer on Your platform");

  public:
    using value_type    = T;
    using pointer       = typename std::add_pointer<value_type>::type;
    using reference     = typename std::add_lvalue_reference<value_type>::type;
    using size_type     = std::size_t;

  private:
    using uintptr_type  = typename unsigned_minimal_integer<pointer>::type;
    static_assert(sizeof(uintptr_type) == sizeof(pointer),
        "Mask missalignment will happen");
    static constexpr uintptr_type STATE_MASK = alignof(value_type) - 1;

    struct storage
    {
        memory_t m_storage[sizeof(uintptr_type)];

        inline uintptr_type load_state() const noexcept
        {
            uintptr_type tmp_;
            std::memcpy(&tmp_, m_storage, sizeof(tmp_));
            return tmp_ & STATE_MASK;
        }

        inline pointer load_pointer() const noexcept
        {
            uintptr_type tmp_;
            pointer tmp_ptr_;
            std::memcpy(&tmp_, m_storage, sizeof(tmp_));
            tmp_ &= ~STATE_MASK;
            std::memcpy(&tmp_ptr_, &tmp_, sizeof(tmp_ptr_));
            return tmp_ptr_;
        }

        inline void store(size_type state) noexcept
        {
            uintptr_type tmp_;
            std::memcpy(&tmp_, m_storage, sizeof(tmp_));
            tmp_ = (tmp_ & (~STATE_MASK)) | (state & STATE_MASK);
            std::memcpy(m_storage, &tmp_, sizeof(tmp_));
        }

        inline void store(pointer ptr) noexcept
        {
            uintptr_type tmp_;
            std::memcpy(&tmp_, &ptr, sizeof(ptr));
            tmp_ = (tmp_ & (~STATE_MASK)) | load_state();
            std::memcpy(m_storage, &tmp_, sizeof(tmp_));
        }

        inline void store(pointer ptr, size_type state) noexcept
        {
            uintptr_type tmp_;
            std::memcpy(&tmp_, &ptr, sizeof(tmp_));
            tmp_ = (tmp_ & (~STATE_MASK)) | (state & STATE_MASK);
            std::memcpy(m_storage, &tmp_, sizeof(tmp_));
        }
    };

    storage m_storage;

  public:
    state_pointer() noexcept
    {
        m_storage.store(pointer(nullptr), 0);
    }

    explicit state_pointer(std::nullptr_t) noexcept : state_pointer() {}

    explicit state_pointer(size_type state) noexcept
    {
        m_storage.store(pointer(nullptr), state);
    }

    explicit state_pointer(pointer ptr) noexcept
    {
        m_storage.store(ptr, 0);
    }

    state_pointer(pointer ptr, size_type state) noexcept
    {
        m_storage.store(ptr, state);
    }

    state_pointer& operator=(size_type state) noexcept
    {
        m_storage.store(state);
        return *this;
    }

    state_pointer& operator=(pointer ptr) noexcept
    {
        m_storage.store(ptr);
        return *this;
    }

    static constexpr size_type state_max() noexcept { return STATE_MASK; }

    inline size_type get_state() const noexcept
    {
        return m_storage.load_state();
    }

    inline void set_state(size_type state) noexcept { m_storage.store(state); }

    inline pointer get_pointer() const noexcept
    {
        return m_storage.load_pointer();
    }

    inline void set_pointer(pointer ptr) noexcept { m_storage.store(ptr); }

    inline pointer operator->() const noexcept { return get_pointer(); }

    inline reference operator*() const noexcept { return *get_pointer(); }

    inline operator pointer() const noexcept { return get_pointer(); }

    /* Comparison operators */

    inline friend bool operator==(
        const state_pointer& lhs, const state_pointer& rhs) noexcept
    {
        return std::memcmp(&lhs.m_storage, &rhs.m_storage,
            sizeof(state_pointer::m_storage)) == 0;
    }
    inline friend bool operator!=(
        const state_pointer& lhs, const state_pointer& rhs) noexcept
    {
        return !(lhs == rhs);
    }
};

} // namespace ecsl
#endif /* ECSL_UTILITY_STATE_POINTER_HPP_ */