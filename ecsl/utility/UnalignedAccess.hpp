#ifndef ECSL_UTILITY_UNALIGNED_ACCESS_HPP_
#define ECSL_UTILITY_UNALIGNED_ACCESS_HPP_

/**
 * @file UnalignedAccess.hpp
 * Adds a way to read/write trival objects bit representation to/from unaligned
 * memory locations. Additionally macro for accesing filed of structs are
 * defined. See ecsl_field_get_ptr.
 */

/// STD
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>

namespace ecsl {
/**
 * @brief Reads any trivially copyable type from unaligned memory location.
 * @tparam T Object type to read
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T is located
 * @return The object of type T
 */
template<class T>
inline typename std::enable_if<
    std::is_default_constructible<T>::value && (
        std::is_trivially_copy_constructible<T>::value ||
        std::is_trivially_copy_assignable<T>::value
    ),
    T
>::type load_unaligned(const void* ptr)
    noexcept(std::is_nothrow_default_constructible<T>::value)
{
    T result;
    std::memcpy(&result, ptr, sizeof(T));
    return result;
}

/**
 * @brief Reads any trivially copyable type from unaligned memory location.
 * @tparam T Deduced object type to read
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T is located
 */
template<class T>
inline typename std::enable_if<
    std::is_trivially_copy_constructible<T>::value ||
    std::is_trivially_copy_assignable<T>::value
>::type load_unaligned(T* dst, const void* src) noexcept
{
    std::memcpy(dst, src, sizeof(T));
}

/**
 * @brief Reads any trivially copyable type from unaligned memory location.
 * @tparam T Deduced object type to read
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T is located
 */
template<class T>
inline typename std::enable_if<
    std::is_trivially_copy_constructible<T>::value ||
    std::is_trivially_copy_assignable<T>::value
>::type load_unaligned(T& dst, const void* src) noexcept
{
    std::memcpy(&dst, src, sizeof(T));
}

/**
 * @brief Writes any trivially copyable type to unaligned memory location.
 * @tparam T Deduced object type to write
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T will be written
 */
template<class T>
inline typename std::enable_if<
    std::is_trivially_copy_constructible<T>::value ||
    std::is_trivially_copy_assignable<T>::value
>::type store_unaligned(void* dst, const T* src) noexcept
{
    std::memcpy(dst, src, sizeof(T));
}

/**
 * @brief Writes any trivially copyable type to unaligned memory location.
 * @tparam T Deduced object type to write
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T will be written
 */
template<class T>
inline typename std::enable_if<
    std::is_trivially_copy_constructible<T>::value ||
    std::is_trivially_copy_assignable<T>::value
>::type store_unaligned(void* dst, const T& src) noexcept
{
    std::memcpy(dst, &src, sizeof(T));
}

/**
 * @brief Reads any trivial type ignoring the memory alignment.
 * @warning May invoke undefined behavior
 * @tparam T Object type to read
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T is located
 * @return The object of type T
 */
template<class T>
inline typename std::enable_if<
    std::is_trivial<T>::value,
    T
>::type load_weak(const void* ptr)
{
    using TT = typename std::add_pointer<typename std::add_const<T>::type>::type;
    return *reinterpret_cast<TT>(ptr);
}

/**
 * @brief Reads any trivial type ignoring the memory alignment.
 * @tparam T Deduced object type to read
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T is located
 */
template<class T>
inline typename std::enable_if<
    std::is_trivial<T>::value
>::type load_weak(T& dst, const void* src) noexcept
{
    using TT = typename std::add_pointer<typename std::add_const<T>::type>::type;
    dst = *reinterpret_cast<TT>(src);;
}

/**
 * @brief Reads any trivial type ignoring the memory alignment.
 * @tparam T Deduced object type to read
 * @param[in] ptr Pointer to memory location at which the bit representation
 * of object of type T is located
 */
template<class T>
inline typename std::enable_if<
    std::is_trivial<T>::value
>::type load_weak(T* dst, const void* src) noexcept
{
    using TT = typename std::add_pointer<typename std::add_const<T>::type>::type;
    *dst = *reinterpret_cast<TT>(src);;
}

namespace detail {
namespace unaligned_access {

template<class memory_type>
inline memory_type* field_get_ptr(memory_type* ptr, std::size_t offset)
{
    return ptr + offset;
}

template<class memory_type, class T, class M>
inline M field_load_weak(memory_type* ptr, std::size_t offset, M T::*)
{
    return ecsl::load_weak<M>(ptr + offset);
}

//? This may be implemented without T and M as
//? the type of M may be deduced from dst
//? but it is left as is to do an additional type check
template<class memory_type, class T, class M>
inline void field_load_weak(M* dst, memory_type* src, std::size_t offset, M T::*)
{
    ecsl::load_weak(dst, src + offset);
}

template<class memory_type, class T, class M>
inline void field_load_weak(M& dst, memory_type* src, std::size_t offset, M T::*)
{
    ecsl::load_weak(dst, src + offset);
}

template<class memory_type, class T, class M>
inline M field_load_unaligned(memory_type* ptr, std::size_t offset, M T::*)
{
    return ecsl::load_unaligned<M>(ptr + offset);
}

template<class memory_type, class T, class M>
inline void field_load_unaligned(M* dst, memory_type* src, std::size_t offset, M T::*)
{
    ecsl::load_unaligned(dst, src + offset);
}

template<class memory_type, class T, class M>
inline void field_load_unaligned(M& dst, memory_type* src, std::size_t offset, M T::*)
{
    ecsl::load_unaligned(dst, src + offset);
}

template<class memory_type, class T, class M>
inline void field_store_unaligned(memory_type* dst, const M* src, std::size_t offset, M T::*)
{
    ecsl::store_unaligned(dst, src + offset);
}

template<class memory_type, class T, class M>
inline void field_store_unaligned(memory_type* dst, const M& src, std::size_t offset, M T::*)
{
    ecsl::store_unaligned(dst, src + offset);
}

} // namespace unaligned_access
} // namespace detail
} // namespace ecsl

/**
 * @brief Obtains a pointer to the field of the struct if provided
 * pointer points to struct's first byte
 * @param[in] SRC_PTR Pointer to first byte of struct
 * @param[in] STRUCT Struct type name
 * @param[in] FIELD Field name
 * See next discussions to understand that there is no better way of doing this
 * in modern C++ (all presented ways in some sense invoke undefined behavior):
 *  https://gist.github.com/graphitemaster/494f21190bb2c63c5516
 *  https://stackoverflow.com/questions/12811330/c-compile-time-offsetof-inside-a-template
 *  https://thecppzoo.blogspot.com/2016/10/constexpr-offsetof-practical-way-to.html
 * The C++ language PR to make other ways possible:
 *  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0908r0.html
 */
#define ecsl_field_get_ptr(SRC_PTR, STRUCT, FIELD) \
    ::ecsl::detail::unaligned_access::field_get_ptr(SRC_PTR, offsetof(STRUCT, FIELD))

#define ECSL_UTILITY_UNALIGNED_ACCESS_EMPTY_(...)
#define ECSL_UTILITY_UNALIGNED_ACCESS_GET_4_(_1, _2, _3, _4, M, ...) M
#define ECSL_UTILITY_UNALIGNED_ACCESS_EXPAND_(...) __VA_ARGS__

#define ECSL_UTILITY_UNALIGNED_ACCESS_field_load_weak_3_(SRC_PTR, STRUCT, FIELD) \
    ::ecsl::detail::unaligned_access::field_load_weak(                           \
        SRC_PTR, offsetof(STRUCT, FIELD), &STRUCT::FIELD)

#define ECSL_UTILITY_UNALIGNED_ACCESS_field_load_weak_4_(DST, SRC_PTR, STRUCT, FIELD) \
    ::ecsl::detail::unaligned_access::field_load_weak(                                \
        DST, SRC_PTR, offsetof(STRUCT, FIELD), &STRUCT::FIELD)

/**
 * @brief Reads any trivial type field of struct using the ecsl::load_weak
 * @param Expects 3 or 4 parameters: [dst,] src_ptr, struct_type, field_name
 * @return With 3 parameters: object of type of struct_type::field_name
 *  with 4 parameters: void, object is loaded to dst
 * @note see ecsl_field_get_ptr for additional information
 */
#define ecsl_field_load_weak(...)                               \
    ECSL_UTILITY_UNALIGNED_ACCESS_EXPAND_(                      \
        ECSL_UTILITY_UNALIGNED_ACCESS_GET_4_(                   \
            __VA_ARGS__,                                        \
            ECSL_UTILITY_UNALIGNED_ACCESS_field_load_weak_4_    \
            ECSL_UTILITY_UNALIGNED_ACCESS_field_load_weak_3_    \
            ECSL_UTILITY_UNALIGNED_ACCESS_EMPTY_                \
            ECSL_UTILITY_UNALIGNED_ACCESS_EMPTY_                \
        )(__VA_ARGS__)                                          \
    )

#define ECSL_UTILITY_UNALIGNED_ACCESS_field_load_unaligned_3_(SRC_PTR, STRUCT, FIELD) \
    ::ecsl::detail::unaligned_access::field_load_unaligned(                           \
        SRC_PTR, offsetof(STRUCT, FIELD), &STRUCT::FIELD)

#define ECSL_UTILITY_UNALIGNED_ACCESS_field_load_unaligned_4_(DST, SRC_PTR, STRUCT, FIELD) \
    ::ecsl::detail::unaligned_access::field_load_unaligned(                                \
        DST, SRC_PTR, offsetof(STRUCT, FIELD), &STRUCT::FIELD)

/**
 * @brief Reads any trivial type field of struct using the ecsl::load_unaligned
 * @param Expects 3 or 4 parameters: [dst,] src_ptr, struct_type, field_name
 * @return With 3 parameters: object of type of struct_type::field_name
 *  with 4 parameters: void, object is loaded to dst
 * @note see ecsl_field_get_ptr for additional information
 */
#define ecsl_field_load_unaligned(...)                              \
    ECSL_UTILITY_UNALIGNED_ACCESS_EXPAND_(                          \
        ECSL_UTILITY_UNALIGNED_ACCESS_GET_4_(                       \
            __VA_ARGS__,                                            \
            ECSL_UTILITY_UNALIGNED_ACCESS_field_load_unaligned_4_   \
            ECSL_UTILITY_UNALIGNED_ACCESS_field_load_unaligned_3_   \
            ECSL_UTILITY_UNALIGNED_ACCESS_EMPTY_                    \
            ECSL_UTILITY_UNALIGNED_ACCESS_EMPTY_                    \
        )(__VA_ARGS__)                                              \
    )

/**
 * @brief Writes any trivial type field of struct using the ecsl::store_unaligned
 * @param 4 parameters: dst_ptr, src, struct_type, field_name
 * @return void
 * @note see ecsl_field_get_ptr for additional information
 */
#define ecsl_field_store_unaligned(DST_PTR, SRC, STRUCT, FIELD) \
    ::ecsl::detail::unaligned_access::field_store_unaligned(    \
        DST_PTR, SRC, offsetof(STRUCT, FIELD), &STRUCT::FIELD)

#endif /* ECSL_UTILITY_UNALIGNED_ACCESS_HPP_ */