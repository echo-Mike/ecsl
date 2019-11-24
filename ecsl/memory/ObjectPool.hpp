#ifndef ECSL_MEMORY_OBJECT_POOL_HPP_
#define ECSL_MEMORY_OBJECT_POOL_HPP_

/// STD
#include <array>
#include <vector>
#include <type_traits>
#include <forward_list>
/// ECSL
#include <ecsl/utility/Storage.hpp>

namespace ecsl {
namespace memory {

/**
 * @brief Object pool for single type of objects
 * No checks of address validity is performed on deallocation.
 * The management of the lifetime of allocated object is the user's
 * responsibility. On destruction only the object storage is deallocated.
 * @tparam T Type of objects to manage
 * @tparam BLOCK_SIZE Size of block of objects to allocate at once
 */
template<class T, std::size_t BLOCK_SIZE = 512>
class object_pool
{
    using vt_t = detail::storage::value_trait<T>;
    using storage_type = typename vt_t::storage_type;
    using block_type = std::array<storage_type, BLOCK_SIZE>;

    inline void consume_block_(block_type& block)
    {
        m_free_to_use.reserve(m_free_to_use.size() + BLOCK_SIZE);
        for (auto& storage_ : block)
        {
            m_free_to_use.push_back(&storage_);
        }
        m_capacity += BLOCK_SIZE;
    }

  public:
    using value_type    = typename vt_t::value_type;
    using pointer       = typename vt_t::pointer;
    using size_type     = std::size_t;

    static constexpr bool is_thread_safe() noexcept { return false; }

    inline bool reserve(size_type object_count = BLOCK_SIZE)
    {
        const auto blocks_count_ = (object_count + (BLOCK_SIZE-1)) / BLOCK_SIZE;
        for (size_type i{0}; i < blocks_count_; ++i)
        {
            m_blocks.emplace_front();
            consume_block(*m_blocks.begin());
        }
        return true;
    }

    inline void* allocate()
    {
        if (empty())
        {
            reserve();
        }
        void* r = m_free_to_use.back();
        m_free_to_use.pop_back();
        return r;
    }

    inline void deallocate(void* ptr)
    {
        m_free_to_use.push_back(ptr);
    }

    template<class U, class ... Args>
    inline typename std::enable_if<
        std::is_constructible<value_type, Args&&...>::value,
        pointer
    >::type construct(U* ptr, Args&& ... args)
        noexcept(std::is_nothrow_constructible<value_type, Args&&...>::value)
    {
        return new(static_cast<void*>(ptr))
            value_type(std::forward<Args>(args)...);
    }

    inline void destroy(pointer ptr)
        noexcept(std::is_nothrow_destructible<value_type>::value)
    {
        ptr->~value_type();
    }

    size_type capacity() const noexcept
    {
        return m_capacity;
    }

    size_type size() const noexcept
    {
        return m_free_to_use.size();
    }

    bool empty() const noexcept
    {
        return m_free_to_use.empty();
    }

  private:
    size_type m_capacity{0};
    std::vector<void*> m_free_to_use;
    std::forward_list<block_type> m_blocks;
};

} // namespace memory
} // namespace ecsl
#endif /* ECSL_MEMORY_OBJECT_POOL_HPP_ */