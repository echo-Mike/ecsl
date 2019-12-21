#ifndef ECSL_UTILITY_COMPILER_BARRIER_HPP_
#define ECSL_UTILITY_COMPILER_BARRIER_HPP_

/**
 * @file CompilerBarrier.hpp
 * Adds compiler instuction reorder barrier
 */

/// STD
#include <atomic>

namespace ecsl {

/**
 * @brief Prevents compiler from reordering of instructions higher and lower
 * than call to this function during the optimizations.
 * Usable with ecsl::prefetch functions
 * Is same as ecsl::compiler_barrier and ecsl::privent_reorder
 */
inline void reorder_barrier() noexcept
{
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

/**
 * @brief Prevents compiler from reordering of instructions higher and lower
 * than call to this function during the optimizations.
 * Usable with ecsl::prefetch functions
 * Is same as ecsl::reorder_barrier and ecsl::privent_reorder
 */
inline void compiler_barrier() noexcept
{
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

/**
 * @brief Prevents compiler from reordering of instructions higher and lower
 * than call to this function during the optimizations.
 * Usable with ecsl::prefetch functions
 * Is same as ecsl::compiler_barrier and ecsl::reorder_barrier
 */
inline void privent_reorder() noexcept
{
    std::atomic_signal_fence(std::memory_order_seq_cst);
}

} // namespace ecsl
#endif /* ECSL_UTILITY_COMPILER_BARRIER_HPP_ */