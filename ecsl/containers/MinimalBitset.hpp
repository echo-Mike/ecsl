#ifndef ECSL_CONTAINERS_MINIMAL_BITSET_HPP_
#define ECSL_CONTAINERS_MINIMAL_BITSET_HPP_

/// STD
#include <climits>
#include <iterator>
#include <stdexcept>
#include <type_traits>
/// ECSL
#include <ecsl/type_traits/SimpleTypes.hpp>

namespace ecsl {
namespace containers {
namespace detail {
namespace minimal_bitset {

/**
 * Reference-like object for a single bit
 */
struct bit_reference
{
    types::memory_t* m_byte;
    types::length_t m_bit;

    inline bit_reference& operator=(bool value) noexcept
    {
        if (value)
        {
            *m_byte |= (1 << m_bit);
        }
        else
        {
            *m_byte = *m_byte ^ (*m_byte | (1 << m_bit));
        }
        return *this;
    }

    inline bit_reference& operator=(const bit_reference& other) noexcept
    {
        auto value_ = static_cast<bool>(other);
        return *this = value_;
    }

    inline bit_reference& flip() noexcept
    {
        return *this = !(*this);
    }

    inline bool test() const noexcept
    {
        return static_cast<bool>(*this);
    }

    inline bool operator!() const noexcept
    {
        return !static_cast<bool>(*this);
    }

    inline operator bool() const noexcept
    {
        return *m_byte & (1 << m_bit);
    }
};

/**
 * Pointer-like object for a single bit
 */
struct bit_pointer
{
    types::memory_t* m_byte;
    types::length_t m_bit;

    inline bit_reference operator*() noexcept { return {m_byte, m_bit}; }
    inline const bit_reference operator*() const noexcept { return {m_byte, m_bit}; }

    inline bit_reference flip() noexcept
    {
        (**this).flip();
        return {m_byte, m_bit};
    }

    inline bool test() const noexcept
    {
        return (**this).test();
    }

    inline bit_pointer& operator->() noexcept { return *this; }
    inline const bit_pointer& operator->() const noexcept { return *this; }

    inline bit_reference operator[](std::size_t pos) noexcept
    {
        return {m_byte + pos / CHAR_BIT, m_bit + pos % CHAR_BIT};
    }
    inline const bit_reference operator[](std::size_t pos) const noexcept
    {
        return {m_byte + pos / CHAR_BIT, m_bit + pos % CHAR_BIT};
    }
};

constexpr std::size_t swar_1_(unsigned char i) noexcept
{
    i = i - ((i >> 1) & 0x55);
    i = (i & 0x33) + ((i >> 2) & 0x33);
    return ((i >> 4) + i) & 0x0F;
}

constexpr std::size_t swar_8_(unsigned long long i) noexcept
{
    i = i - (i >> 1 & 0x5555555555555555ULL);
    i = (i & 0x3333333333333333ULL) + (i >> 2 & 0x3333333333333333ULL);
    return ((((i >> 4) + i) & 0x0F0F0F0F0F0F0F0FULL) * 0x0101010101010101ULL) >> (64-8);
}

using ull_t = unsigned long long;

constexpr ull_t pack_(types::memory_t* data, std::size_t count = 8) noexcept
{
    static_assert(sizeof(ull_t) == 8, "");
    ull_t i_{0};
    switch (count)
    {
        default:
        case 8: i_ |= static_cast<ull_t>(data[7]) << (8 * 7);
        case 7: i_ |= static_cast<ull_t>(data[6]) << (8 * 6);
        case 6: i_ |= static_cast<ull_t>(data[5]) << (8 * 5);
        case 5: i_ |= static_cast<ull_t>(data[4]) << (8 * 4);
        case 4: i_ |= static_cast<ull_t>(data[3]) << (8 * 3);
        case 3: i_ |= static_cast<ull_t>(data[2]) << (8 * 2);
        case 2: i_ |= static_cast<ull_t>(data[1]) << (8 * 1);
        case 1: i_ |= static_cast<ull_t>(data[0]) << (8 * 0);
        case 0: break;
    }
    return i_;
}

constexpr void unpack_(ull_t i, types::memory_t* data) noexcept
{
    data[0] = (i >> (8 * 0)) & 0xFF;
    data[1] = (i >> (8 * 1)) & 0xFF;
    data[2] = (i >> (8 * 2)) & 0xFF;
    data[3] = (i >> (8 * 3)) & 0xFF;
    data[4] = (i >> (8 * 4)) & 0xFF;
    data[5] = (i >> (8 * 5)) & 0xFF;
    data[6] = (i >> (8 * 6)) & 0xFF;
    data[7] = (i >> (8 * 7)) & 0xFF;
}

} // namespace minimal_bitset
} // namespace detail

/**
 * @brief Constant length bit vector
 * Has a minimal possible storage size for BITS_COUNT bits
 * Intended to be operatable in constexpr context
 * (operations are not platform/instruction set optimized).
 * Like std::bitset but with iterators and more of std::array symantics.
 * @tparam BITS_COUNT Bit length of vector
 */
template<std::size_t BITS_COUNT>
class minimal_bitset
{
    static constexpr std::size_t BITS_IN_BYTE =
        CHAR_BIT;
    static constexpr std::size_t LAST_BIT =
        (BITS_COUNT - 1) % CHAR_BIT;
    static constexpr std::size_t CAPACITY =
        (BITS_COUNT + (BITS_IN_BYTE - 1)) / BITS_IN_BYTE;
    static constexpr auto BYTE_MASK =
        static_cast<types::memory_t>(~0);
    static constexpr auto LAST_MASK =
        static_cast<types::memory_t>((1 << (LAST_BIT + 1)) - 1);

    static_assert(CAPACITY != 0, "Can't create minimal_bitset of zero length");

    types::memory_t m_bytes[CAPACITY];

    template<bool IS_CONST>
    class iterator_impl
    {
        friend class minimal_bitset;
        using Container = minimal_bitset;

      public:
        using value_type        = typename std::conditional<IS_CONST,
            const detail::minimal_bitset::bit_reference,
            detail::minimal_bitset::bit_reference
        >::type;
        using pointer           = typename std::conditional<IS_CONST,
            const detail::minimal_bitset::bit_pointer,
            detail::minimal_bitset::bit_pointer
        >::type
        using reference         = value_type;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;

      private:
        constexpr iterator_impl(Container* container, difference_type bit) noexcept :
            m_container{container}, m_bit{bit}
        {}

        constexpr pointer get_pointer() const noexcept
        {
            return {m_container->m_bytes + m_bit / BITS_IN_BYTE, m_bit % BITS_IN_BYTE};
        }

      public:
        constexpr iterator_impl() noexcept : m_container{nullptr}, m_bit{0} {}
        constexpr iterator_impl(const iterator_impl&) = default;
        constexpr iterator_impl& operator=(const iterator_impl&) = default;

        constexpr iterator_impl& operator++() noexcept
        {
            ++m_bit;
            return *this;
        }
        constexpr iterator_impl operator++(int) noexcept
        {
            iterator_impl old{*this};
            ++(*this);
            return old;
        }

        constexpr iterator_impl& operator--() noexcept
        {
            --m_bit;
            return *this;
        }
        constexpr iterator_impl operator--(int) noexcept
        {
            iterator_impl old{*this};
            --(*this);
            return old;
        }

        constexpr iterator_impl& operator+=(difference_type n) noexcept
        {
            m_bit += n;
            return *this;
        }

        friend constexpr iterator_impl
            operator+(const iterator_impl& a, difference_type n) noexcept
        {
            iterator_impl tmp = a;
            return tmp += n;
        }
        friend constexpr iterator_impl
            operator+(difference_type n, const iterator_impl& a) noexcept
        {
            return a + n;
        }

        constexpr iterator_impl& operator-=(difference_type n) noexcept
        {
            return operator+=(-n);
        }

        friend constexpr iterator_impl
            operator-(const iterator_impl& a, difference_type n) noexcept
        {
            iterator_impl tmp = a;
            return tmp -= n;
        }
        friend constexpr difference_type
            operator-(const iterator_impl& b, const iterator_impl& a) noexcept
        {
            return b.m_bit - a.m_bit;
        }

        constexpr pointer operator->() const noexcept
        {
            return get_pointer();
        }
        constexpr reference operator*() const
        {
            return *get_pointer();
        }
        constexpr reference operator[](difference_type n) const
        {
            iterator_impl tmp_{*this};
            tmp_ += n;
            return *tmp_;
        }

        friend constexpr bool
            operator==(const iterator_impl& lhs, const iterator_impl& rhs) noexcept
        {   //? This operation must not be defined for different containers
            return lhs.m_bit == rhs.m_bit;
        }
        friend constexpr bool
            operator!=(const iterator_impl& lhs, const iterator_impl& rhs) noexcept
        {
            return !(lhs == rhs);
        }
        friend constexpr bool
            operator<(const iterator_impl& lhs, const iterator_impl& rhs) noexcept
        {   //? This operation must not be defined for different containers
            return lhs.m_bit < rhs.m_bit;
        }
        friend constexpr bool
            operator>(const iterator_impl& lhs, const iterator_impl& rhs) noexcept
        {
            return rhs < lhs;
        }
        friend constexpr bool
            operator>=(const iterator_impl& lhs, const iterator_impl& rhs) noexcept
        {
            return !(lhs < rhs);
        }
        friend constexpr bool
            operator<=(const iterator_impl& lhs, const iterator_impl& rhs) noexcept
        {
            return !(lhs > rhs);
        }

      private:
        Container* m_container;
        difference_type m_bit;
    };

  public:
    using iterator          = iterator_impl<false>;
    using const_iterator    = iterator_impl<true>;
    using pointer           = typename iterator::pointer;
    using const_pointer     = typename const_iterator::pointer;
    using reference         = typename iterator::reference;
    using const_reference   = typename const_iterator::reference;
    using size_type         = std::size_t;
    using ssize_type        = typename std::make_signed<size_type>::type;

    template<size_type N>
    constexpr minimal_bitset& operator=(const char(&val)[N]) noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            m_bytes[i] = i < N ? val[i] : 0;
        }
        m_bytes[CAPACITY-1] =
            ((CAPACITY-1 <= N ? val[CAPACITY-1] : 0) & LAST_MASK) |
            (m_bytes[CAPACITY-1] & ~LAST_MASK);
        return *this;
    }

    template<size_type N>
    constexpr minimal_bitset& operator=(const types::memory_t(&val)[N]) noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            m_bytes[i] = i < N ? val[i] : 0;
        }
        m_bytes[CAPACITY-1] =
            ((CAPACITY-1 <= N ? val[CAPACITY-1] : 0) & LAST_MASK) |
            (m_bytes[CAPACITY-1] & ~LAST_MASK);
        return *this;
    }

  private:
    static constexpr bool check_position_(
        size_type pos,
        size_type& byte,
        size_type& bit
    ) noexcept
    {
        byte = pos / BITS_IN_BYTE;
        bit = pos % BITS_IN_BYTE;
        return !((byte >= CAPACITY) || (byte == CAPACITY-1 && bit > LAST_BIT));
    }

  public:
    constexpr void set(size_type position) noexcept
    {
        const size_type byte_{}, bit_{};
        if (!check_position_(position, byte_, bit_))
        {
            return;
        }
        m_bytes[byte_] |= (1 << bit_);
    }

    constexpr void set() noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            m_bytes[i] = BYTE_MASK;
        }
        m_bytes[CAPACITY-1] = LAST_MASK;
    }

    constexpr void reset(size_type position) noexcept
    {
        const size_type byte_{}, bit_{};
        if (!check_position_(position, byte_, bit_))
        {
            return;
        }
        m_bytes[byte_] =
            m_bytes[byte_] ^ (m_bytes[byte_] | (1 << bit_));
    }

    constexpr void reset() noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            m_bytes[i] = 0;
        }
        m_bytes[CAPACITY-1] &= ~LAST_MASK;
    }

    constexpr void flip(size_type position) noexcept
    {
        const size_type byte_{}, bit_{};
        if (!check_position_(position, byte_, bit_))
        {
            return;
        }
        m_bytes[byte_] ^= (1 << bit_);
    }

    constexpr void flip() noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            m_bytes[i] ^= BYTE_MASK;
        }
        m_bytes[CAPACITY-1] ^= LAST_MASK;
    }

    constexpr bool test(size_type position) const noexcept
    {
        const size_type byte_{}, bit_{};
        if (!check_position_(position, byte_, bit_))
        {
            return false;
        }
        return m_bytes[byte_] & (1 << bit_);
    }

    constexpr bool any() const noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            if (m_bytes[i])
            {
                return true;
            }
        }
        return m_bytes[CAPACITY-1] & LAST_MASK;
    }

    constexpr bool all() const noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            if (m_bytes[i] != BYTE_MASK)
            {
                return false;
            }
        }
        return m_bytes[CAPACITY-1] == LAST_MASK;
    }

    constexpr size_type count() const noexcept
    {
        using namespace detail::minimal_bitset;
        //? Count of full bytes
        constexpr auto LENGTH_ = CAPACITY-1;
        constexpr auto ROUNDS_ = LENGTH_ / 8;
        constexpr auto OFFSET_ = LENGTH_ % 8;
        //? Count bits in last byte
        size_type r_ = swar_1_(m_bytes[LENGTH_] & LAST_MASK);
        auto round_ = ROUNDS_;
        while (round_-- > 0)
        {
            r_ += swar_8_(pack_(m_bytes + 8 * round_ + OFFSET_));
        }
        r_ += swar_8_(pack_(m_bytes, OFFSET_));
        return r_;
    }

    constexpr size_type size() const noexcept
    {
        return BITS_COUNT;
    }

    constexpr iterator begin() noexcept { return iterator(this, 0); }
    constexpr const_iterator begin() const noexcept { return const_iterator(this, 0); }
    constexpr const_iterator cbegin() const noexcept { return const_iterator(this, 0); }

    constexpr iterator end() noexcept { return iterator(this, BITS_COUNT); }
    constexpr const_iterator end() const noexcept { return const_iterator(this, BITS_COUNT); }
    constexpr const_iterator cend() const noexcept { return const_iterator(this, BITS_COUNT); }

    constexpr reference operator[](std::size_t position) noexcept
    {
        return *std::next(begin(), position);
    }
    constexpr const_reference operator[](std::size_t position) const noexcept
    {
        return *std::next(begin(), position);
    }

    constexpr reference at(size_type position)
    {
        if (position < CAPACITY)
        {
            return operator[](position);
        }
        throw std::out_of_range{"minimal_bitset range check failed"};
    }
    constexpr reference at(size_type position) const
    {
        if (position < CAPACITY)
        {
            return operator[](position);
        }
        throw std::out_of_range{"minimal_bitset range check failed"};
    }

    constexpr reference front() noexcept { return *begin(); }
    constexpr const_reference front() const noexcept { return *begin(); }

    constexpr reference back() noexcept { return *std::prev(end()); }
    constexpr const_reference back() const noexcept { return *std::prev(end()); }

  private:
    template<std::size_t N, class F>
    constexpr void swipe_(const minimal_bitset<N>& other, F&& func) noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            m_bytes[i] = i + 1 < minimal_bitset<N>::CAPACITY ?
                func(m_bytes[i], other.m_bytes[i]) :
                i < minimal_bitset<N>::CAPACITY ?
                    func(m_bytes[i], other.m_bytes[i] & minimal_bitset<N>::LAST_MASK) :
                    func(m_bytes[i], 0);
        }
        if (CAPACITY <= minimal_bitset<N>::CAPACITY)
        {
            m_bytes[CAPACITY-1] =
                (func(
                    m_bytes[CAPACITY-1],
                    other.m_bytes[CAPACITY-1] & (
                        CAPACITY == minimal_bitset<N>::CAPACITY ?
                            minimal_bitset<N>::LAST_MASK :
                            BYTE_MASK
                    )
                ) & LAST_MASK) |
                (m_bytes[CAPACITY-1] & ~LAST_MASK);
        }
        else
        {
            m_bytes[CAPACITY-1] =
                (func(m_bytes[CAPACITY-1], 0) & LAST_MASK) |
                (m_bytes[CAPACITY-1] & ~LAST_MASK);
        }
    }

  public:
    template<std::size_t N, class = std::enable_if<N != BITS_COUNT>::type>
    constexpr minimal_bitset& operator=(const minimal_bitset<N>& other) noexcept
    {
        struct assign_
        {
            constexpr types::memory_t
                operator()(types::memory_t, types::memory_t b) noexcept
            {
                return b;
            }
        };
        swipe_(other, assign_{});
        return *this;
    }

    template<std::size_t N>
    constexpr minimal_bitset& operator&=(const minimal_bitset<N>& other) noexcept
    {
        struct and_
        {
            constexpr types::memory_t
                operator()(types::memory_t a, types::memory_t b) noexcept
            {
                return a & b;
            }
        };
        swipe_(other, and_{});
        return *this;
    }

    template<std::size_t N>
    constexpr minimal_bitset& operator|=(const minimal_bitset<N>& other) noexcept
    {
        struct or_
        {
            constexpr types::memory_t
                operator()(types::memory_t a, types::memory_t b) noexcept
            {
                return a | b;
            }
        };
        swipe_(other, or_{});
        return *this;
    }

    template<std::size_t N>
    constexpr minimal_bitset& operator^=(const minimal_bitset<N>& other) noexcept
    {
        struct xor_
        {
            constexpr types::memory_t
                operator()(types::memory_t a, types::memory_t b) noexcept
            {
                return a ^ b;
            }
        };
        swipe_(other, xor_{});
        return *this;
    }

    constexpr minimal_bitset operator~() const noexcept
    {
        auto tmp_ = *this;
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            tmp_.m_bytes[i] ^= BYTE_MASK;
        }
        tmp_.m_bytes[CAPACITY-1] =
            (~tmp_.m_bytes[CAPACITY-1] & LAST_MASK) |
            (tmp_.m_bytes[CAPACITY-1] & ~LAST_MASK);
        return tmp_;
    }

    constexpr minimal_bitset& operator<<=(std::size_t position) noexcept
    {
        constexpr auto LENGTH_ = CAPACITY-1;
        if (position >= BITS_COUNT)
        {
            reset();
            return *this;
        }
        if (position == 0)
        {
            return *this;
        }
        const size_type shifts_[] =
            {position % BITS_IN_BYTE, BITS_IN_BYTE - (position % BITS_IN_BYTE)};
        const size_type offsets_[] =
            {position / BITS_IN_BYTE, (position + BITS_IN_BYTE-1) / BITS_IN_BYTE};
        //! For >> we assume that operation zero-fills bytes for unsigned types
        //! Otherwise the masking is needed
        static_assert(decltype(m_bytes[0]){0} << 1 == 0, "");
        static_assert((decltype(m_bytes[0]){~0} >> 1) != decltype(m_bytes[0]){~0}, "");
        //? Swiping from the end to beginning
        m_bytes[LENGTH_] =
            (((m_bytes[LENGTH_ - offsets_[1]] >> shifts_[1]) |
              (m_bytes[LENGTH_ - offsets_[0]] << shifts_[0])) & LAST_MASK) |
              (m_bytes[LENGTH_] & ~LAST_MASK);
        for (size_type i{LENGTH_-1}; i > offsets_[0]; --i)
        {
            m_bytes[i] =
                (m_bytes[i - offsets_[1]] >> shifts_[1]) |
                (m_bytes[i - offsets_[0]] << shifts_[0]);
        }
        m_bytes[offsets_[0]] = m_bytes[0] << offsets_[0];
        for (ssize_type i{offsets_[0] - 1}; i >= 0; --i)
        {
            m_bytes[i] = 0;
        }
        return *this;
    }

    constexpr minimal_bitset operator<<(std::size_t position) const noexcept
    {
        auto tmp_ = *this;
        return tmp_ <<= position;
    }

    constexpr minimal_bitset& operator>>=(std::size_t position) noexcept
    {
        constexpr auto LENGTH_ = CAPACITY-1;
        if (position >= BITS_COUNT)
        {
            reset();
            return *this;
        }
        if (position == 0)
        {
            return *this;
        }
        const size_type shifts_[] =
            {position % BITS_IN_BYTE, BITS_IN_BYTE - (position % BITS_IN_BYTE)};
        const size_type offsets_[] =
            {position / BITS_IN_BYTE, (position + BITS_IN_BYTE-1) / BITS_IN_BYTE};
        for (size_type i{0}; i + offsets_[1] < LENGTH_; ++i)
        {
            m_bytes[i] =
                (m_bytes[i + offsets_[0]] >> shifts_[0]) |
                (m_bytes[i + offsets_[1]] << shifts_[1]);
        }
        m_bytes[LENGTH_ - offsets_[1]] =
            (m_bytes[LENGTH_ - offsets_[1] + offsets_[0]] >> shifts_[0]) |
            ((m_bytes[LENGTH_] & LAST_MASK) << shifts_[1]);
        for (ssize_type i{LENGTH_ - offsets_[1] + 1}; i < LENGTH_; ++i)
        {
            m_bytes[i] = 0;
        }
        if (position > LAST_BIT)
        {
            m_bytes[LENGTH_] &= ~LAST_MASK;
        }
        return *this;
    }

    constexpr minimal_bitset operator>>(std::size_t position) const noexcept
    {
        auto tmp_ = *this;
        return tmp_ >>= position;
    }

    constexpr bool operator==(const minimal_bitset& rhs) noexcept
    {
        for (size_type i{0}; i + 1 < CAPACITY; ++i)
        {
            if (m_bytes[i] != rhs.m_bytes[i])
            {
                return false;
            }
        }
        return (m_bytes[CAPACITY-1] & LAST_MASK) == (rhs.m_bytes[CAPACITY-1] & LAST_MASK);
    }
    constexpr bool operator!=(const minimal_bitset& rhs) noexcept
    {
        return !(*this == rhs);
    }
};

template<std::size_t N>
minimal_bitset<N> operator&(const minimal_bitset<N>& lhs, const minimal_bitset<N>& rhs) noexcept
{
    auto tmp_ = lhs;
    return tmp_ &= rhs;
}
template<std::size_t N>
minimal_bitset<N> operator|(const minimal_bitset<N>& lhs, const minimal_bitset<N>& rhs) noexcept
{
    auto tmp_ = lhs;
    return tmp_ |= rhs;
}
template<std::size_t N>
minimal_bitset<N> operator^(const minimal_bitset<N>& lhs, const minimal_bitset<N>& rhs) noexcept
{
    auto tmp_ = lhs;
    return tmp_ ^= rhs;
}

} // namespace containers

template<std::size_t N>
using minimal_bitset_t = containers::minimal_bitset<N>;

} // namespace ecsl
#endif /* ECSL_CONTAINERS_MINIMAL_BITSET_HPP_ */