#ifndef ECSL_UTILITY_ANY_FUNCTION_HPP_
#define ECSL_UTILITY_ANY_FUNCTION_HPP_

/**
 * @file AnyFunction.hpp
 * Adds type erased storage for any function
 * call to which can be deferred and execute somewhere else
 */

/// STD
#include <cstdint>
#include <mutex>
#include <tuple>
#include <atomic>
#include <chrono>
#include <memory>
#include <utility>
#include <optional>
#include <exception>
#include <functional>
#include <type_traits>
#include <condition_variable>

namespace ecsl {
namespace detail {
namespace any_function {
namespace detail {

template<class T> struct alignas(T) type_t { char _[sizeof(T)]; };
template<> struct type_t<void> {};
template<class R, class... Args> struct type_t<R(Args...)> {};
template<class R, class... Args> struct type_t<R(&)(Args...)> {};

template<class... T>
struct type_id_
{
    /* ABI stability arguments */
    template<
        std::size_t = sizeof(std::tuple<type_t<T>...>),
        std::size_t = alignof(std::tuple<type_t<T>...>)
    >
    static void id_() {}
};

template<class T, class U>
constexpr typename std::enable_if<std::is_rvalue_reference<T>::value,
std::optional<U>&&>::type optional_forward(std::optional<U>& opt) noexcept
{
    return std::move(opt);
}
template<class T, class U>
constexpr typename std::enable_if<!std::is_rvalue_reference<T>::value,
std::optional<U>&>::type optional_forward(std::optional<U>& opt) noexcept
{
    return opt;
}

template <class F, class Tuple, class... Args, std::size_t... I>
constexpr decltype(auto) apply_impl(
    F& f,
    Tuple& t,
    detail::type_list<Args...> a,
    std::index_sequence<I...>
)
{
    return std::invoke(f, *optional_forward<Args>(std::get<I>(t))...);
}

template <class... Types, std::size_t... I>
constexpr bool has_values_impl(
    const std::tuple<std::optional<Types>...>& t,
    std::index_sequence<I...>
) noexcept
{
    bool result_ = true;
    char _[sizeof...(Types)] =
        {(result_ = result_ && static_cast<bool>(std::get<I>(t)), 0)...};
    static_cast<void>(_);
    return result_;
}

template<class... T> struct type_list{};

} // namespace detail

/**
 * Special type that holds unique type ID
 */
using type_id_t = void(*)();
/**
 * Special value of type_id_t that no type have
 */
constexpr static type_id_t null_id = nullptr;
/**
 * Obtains type_id_t value for type list T... (order is included)
 */
template<class... T> type_id_t type_id() noexcept { return &detail::type_id_<T...>::id_; }

/**
 * Trait for signature breakdown
 */
template<class T>
struct signature_trait;
template<class R, class... Args>
struct signature_trait<R(Args...)>
{
    using type = R(Args...);
    using result_type = R;
};
template<class C, class R, class... Args>
struct signature_trait<R(C::*)(Args...)>
{
    using type = R(C::*)(Args...);
    using result_type = R;
};
/**
 * signature_trait helpers
 */
template<class R, class... Args>
auto get_signature(R(*)(Args...)) -> signature_trait<R(Args...)>;
template<class C, class R, class... Args>
auto get_signature_(R(C::*)(Args...)) -> signature_trait<R(C::*)(Args...)>;

/**
 * Trait for signature translation from free-function
 * to member-function and vice-versa
 */
template<class T, class C>
struct translate_signature;
template<class C, class R, class... Args>
struct translate_signature<R(Args...), C>
{
    using type = signature_trait<R(C::*)(Args...)>;
};
template<class C, class R, class... Args>
struct translate_signature<R(C::*)(Args...), C>
{
    using type = signature_trait<R(Args...)>;
};

/**
 * Trait to determine that object is a functor (has operator() of some signature defined)
 */
template<class T>
struct is_callable
{
  private:
    struct yes { char _; };
    struct no { char _[2]; };

    struct fallback { void operator()(); };
    struct derived : T, fallback { };
    template<typename U, U> struct check;

    template<typename>
    static yes test(...);
    template<typename C>
    static no test(check<void (fallback::*)(), &C::operator()>*);

  public:
    static const bool value = sizeof(test<derived>(0)) == sizeof(yes);
};

/**
 * Calls callable with arguments from std::tuple<std::optional<T...>>
 */
template <class F, class Tuple, class... Args>
constexpr decltype(auto) apply(F& f, Tuple& t, detail::type_list<Args...> a)
{
    return detail::apply_impl(f, t, a, std::make_index_sequence<sizeof...(Args)>{});
}
/**
 * Checks that every std::optional from std::tuple<std::optional<T...>> has value
 */
template <class... Types>
constexpr bool has_values(const std::tuple<std::optional<Types>...>& t) noexcept
{
    return detail::has_values_impl(t, std::make_index_sequence<sizeof...(Types)>{});
}

namespace ith_element {
namespace detail {

    template <class... Types, std::size_t C, std::size_t... I>
    inline void* get_impl(
        std::size_t i,
        std::tuple<std::optional<Types>...>& t,
        std::index_sequence<C, I...>
    ) noexcept
    {
        if (i == C)
        {
            return &std::get<C>(t);
        }
        if constexpr (sizeof...(I) == 0)
        {
            return nullptr;
        }
        else
        {
            return get_impl(i, t, std::index_sequence<I...>{});
        }
    }

    template <class... T, std::size_t C, std::size_t... I>
    constexpr bool is_same_impl(
        std::size_t i,
        type_id_t id,
        std::index_sequence<C, I...>
    ) noexcept
    {
        if (i == C)
        {
            return type_id<typename std::tuple_element<C, std::tuple<T...>>::type>() == id;
        }
        if constexpr (sizeof...(I) == 0)
        {
            return false;
        }
        else
        {
            return is_same_impl<T...>(i, id, std::index_sequence<I...>{});
        }
    }

    template <class... Types, std::size_t C, std::size_t... I>
    constexpr bool has_value_impl(
        std::size_t i,
        const std::tuple<std::optional<Types>...>& t,
        std::index_sequence<C, I...>
    ) noexcept
    {
        if (i == C)
        {
            return std::get<C>(t).has_value();
        }
        if constexpr (sizeof...(I) == 0)
        {
            return false;
        }
        else
        {
            return has_value_impl(i, t, std::index_sequence<I...>{});
        }
    }

} // namespace detail

/**
 * Obtains i'th element of std::tuple<std::optional<T...>> via void*
 */
template<class... Types>
inline void* get(std::size_t i, std::tuple<std::optional<Types>...>& t) noexcept
{
    return detail::get_impl(i, t, std::make_index_sequence<sizeof...(Types)>{});
}
/**
 * Checks that type_id of i'th element of std::tuple<std::optional<T...>> is same as provided
 */
template<class... Types>
constexpr bool is_same(
    std::size_t i,
    type_id_t id,
    const std::tuple<std::optional<Types>...>&
) noexcept
{
    return detail::is_same_impl<Types...>(i, id, std::make_index_sequence<sizeof...(Types)>{});
}
/**
 * Returns result of has_value() of i'th element of std::tuple<std::optional<T...>>
 */
template<class... Types>
constexpr bool has_value(std::size_t i, const std::tuple<std::optional<Types>...>& t) noexcept
{
    return detail::has_value_impl(i, t, std::make_index_sequence<sizeof...(Types)>{});
}

} // ith_element

/**
 * Base class for any_function and any_function::future
 * Manages held state
 */
class af_ctx_manager
{
  protected:
    /**
     * Represents the set of operations possible to execute on context
     */
    enum class action_type
    {
        up_ref_counter,         /* noexcept: operation of context reference counter increment */
        /**
         * throw(Allocator<Context>::destroy(), Allocator<Context>::deallocate()):
         * operation of reference counter decrement
         */
        down_ref_counter,
        lock_call,              /* noexcept: context lock operation */
        try_lock_call,          /* noexcept: attempt to lock context */
        unlock_call,            /* noexcept: context unlock operation */
        get_argument_storage,   /* noexcept: obtain pointer to full argument storage */
        call,                   /* noexcept: call held Callable, context must be locked */
        has_result,             /* noexcept: check that context has a result ready */
        has_exception,          /* noexcept: check that context has an exception */
        has_argument,           /* noexcept: check that context has some argument initialized */
        check_result_type,      /* noexcept: check type of result object (the object may not be initialized) */
        check_argument_type,    /* noexcept: check type of some argument (may not be initialized) */
        get_result,             /* noexcept: obtain pointer to result storage */
        get_argument,           /* noexcept: obtain pointer to some argument storage */
        get_exception,          /* noexcept: obtain pointer to exception storage */
        get_waitable,           /* noexcept: obtain pointer to waitable object */
        get_argument_count,     /* noexcept: obtain count of arguments */
        has_all_arguments,      /* noexcept: informs if context has all arguments */
    };
    /**
     * Represents the signature of core manager function
     */
    using manager_function = void*(
        action_type /*action*/,
        void* /*context*/,
        type_id_t /*type_id*/,
        void* /*arg*/
    );

    /**
     * RAII type for context locking
     */
    struct call_guard
    {
        void* ctx;
        manager_function* f;
        bool locked;

        explicit operator bool() const noexcept { return locked; }

        call_guard(void* ctx_, manager_function* f_) noexcept :
            ctx{ctx_},
            f{f_},
            locked{f(action_type::lock_call, ctx, null_id, nullptr) != nullptr}
        {}

        ~call_guard() noexcept
        {
            if (locked)
            {
                f(action_type::unlock_call, ctx, null_id, nullptr);
            }
        }
    };

    /**
     * Represents rebindable lvalue reference.
     * @tparam T Must be a lvalue reference type, may be cv-qualified
     */
    template<class T>
    struct reference_storage
    {
        static_assert(std::is_lvalue_reference<T>::value, "Only for lvalue reference types");
        using nr_type = typename std::remove_reference<T>::type;

        reference_storage(nr_type& value) noexcept : ptr{&value} {}
        reference_storage& operator=(nr_type& value) noexcept
        {
            ptr = &value;
            return *this;
        }

        operator nr_type&() const noexcept { return *ptr; }

        typename std::add_pointer<nr_type>::type ptr;
    };

    /**
     * Represents void result type
     */
    struct as_void {};

    /**
     * Trait for function argument set storage representation.
     *  If T is lvalue reference -> the rebindable reference-like object is created
     *  If T is rvalue reference -> object is stored as is after std::decay
     *  If T is value -> object is stored as is after std::decay
     */
    template<class T>
    using function_argument = typename std::conditional<
        std::is_lvalue_reference<T>::value,
        reference_storage<T>,
        typename std::decay<T>::type
    >::type;
    /**
     * Trait for function result storage representation.
     *  If T is lvalue reference -> the rebindable reference-like object is created
     *  If T is rvalue reference -> object is stored as is after std::decay
     *  If T is value -> object is stored as is after std::decay
     *  If T is cv-void -> void replacement type is created
     */
    template<class T>
    using function_result = typename std::conditional<
        std::is_void<T>::value,
        as_void,
        typename std::conditional<
            std::is_lvalue_reference<T>::value,
            reference_storage<T>,
            typename std::decay<T>::type
        >::type
    >::type;

    void* call_with(action_type action, type_id_t id = null_id, void* arg = nullptr) const
    {
        return m_manager(action, m_context, id, arg);
    }

    /* Used for test suit */
    constexpr af_ctx_manager(void* context, manager_function* manager) noexcept :
        m_context{context}, m_manager{manager} {}

  public:
    constexpr af_ctx_manager() noexcept : m_context{nullptr}, m_manager{nullptr} {}

    af_ctx_manager(const af_ctx_manager& other) noexcept :
        m_context{other.m_context},
        m_manager{other.m_manager}
    {
        if (static_cast<bool>(other))
        {
            call_with(action_type::up_ref_counter);
        }
    }
    af_ctx_manager(af_ctx_manager&& other) noexcept :
        m_context{nullptr},
        m_manager{nullptr}
    {
        swap(other);
    }

    af_ctx_manager& operator=(const af_ctx_manager& other)
    {
        if (this == &other)
        {
            return *this;
        }
        if (static_cast<bool>(*this))
        {
            call_with(action_type::down_ref_counter);
        }
        *this = af_ctx_manager(other);
        return *this;
    }
    af_ctx_manager& operator=(af_ctx_manager&& other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        swap(other);
        return *this;
    }

    ~af_ctx_manager() // implicit noexcept
    {
        if (operator bool())
        {
            call_with(action_type::down_ref_counter);
        }
    }

    inline void swap(af_ctx_manager& other) noexcept
    {
        using std::swap;
        swap(m_context, other.m_context);
        swap(m_manager, other.m_manager);
    }
    inline friend void swap(af_ctx_manager& lhs, af_ctx_manager& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    explicit operator bool() const noexcept
    {
        return m_context && m_manager;
    }
    friend bool operator==(const af_ctx_manager& lhs, const af_ctx_manager& rhs) noexcept
    {
        return lhs.m_context == rhs.m_context && lhs.m_manager == rhs.m_manager;
    }

  protected:
    void* m_context;
    manager_function* m_manager;
};

/**
 * Callable that holds some callable with some signature and provides interfaces
 * for deferred call arguments storage and initiation.
 * Intended to be used as single time callable.
 * Copyable and Movable. Default constructiable in empty state.
 *
 * Known limitations:
 *  1) Works only with allocators with single template parameter
 */
class any_function :
    public af_ctx_manager
{
    /**
     * Context for wait operations
     */
    struct waitable
    {
        std::mutex m_mu;
        std::condition_variable m_cv;
    };

  public:
    /**
     * Thrown if before the call not all arguments are initialized
     */
    class missing_argument : public std::exception
    {
      public:
        missing_argument() noexcept = default;

        const char* what() const noexcept override
        {
            return "One of the arguments of called function is not initialized";
        }
    };
    /**
     * Thrown when reference version of casts are used
     */
    class bad_type_cast : public std::exception
    {
      public:
        bad_type_cast() noexcept = default;

        const char* what() const noexcept override
        {
            return "Type cast failed";
        }
    };

    /**
     * Provides the information about call result
     */
    enum class call_result
    {
        /* call is finished with some result */
        succeeded,
        /* call is finished with exception */
        exception,
         /* provided arguments are not match with function argument list */
        arguments_not_accepted,
    };

    /**
     * Used for creation of unsafe context.
     * Intended for single thread environments
     */
    struct unsafe_tag{};
    /**
     * Used for creation of shared context (like std::shared_ptr).
     * Intended for multithreaded environments where user
     * is responsible for context access synchronization.
     */
    struct shared_tag{};
    /**
     * Used for creation of spinlock context (atomically ref_conted).
     * Intended for multithreaded environments where context is locked
     * automaticaly via spinlock mutex.
     * Good for small computations.
     */
    struct spinlock_tag{};
    /**
     * Used for creation of waitable context (atomically ref_conted).
     * Intended for multithreaded environments where context is locked
     * automaticaly via std::mutex. std::condition_variable and std::mutex
     * is used for every instance of context.
     * Good for large computations.
     */
    struct waitable_tag{};

    /**
     * Same as std::future_status
     */
    enum class future_status
    {
        ready,
        timeout,
        deferred /* unused */
    };

    template<class T> friend class future;

    /**
     * Like std::future but for any_function facility
     * @tparam T Expected type of the result
     */
    template<class T>
    class future :
        public af_ctx_manager
    {
      public:
        constexpr future() noexcept : af_ctx_manager{} {}
        future(const future&) noexcept = default;
        future(future&&) noexcept = default;
        future& operator=(const future&) = default;
        future& operator=(future&&) noexcept = default;
        ~future() = default;

        /**
         * Constructs future from any_function object
         * @throw any_function::bad_type_cast if result type is not T
         */
        explicit future(const any_function& af) : af_ctx_manager{af}
        {
            if (static_cast<bool>(af) && !af.is_result_of_type<T>())
            {
                throw bad_type_cast{};
            }
        }

        /**
         * Obtains the result, possibly after waiting for some time.
         * @warning UB if valid() == false
         */
        template<class = typename std::enable_if<!std::is_void<T>::value>::type>
        T& get() const
        {
            wait();
            call_guard guard_{m_context, m_manager};
            if (call_with(action_type::has_exception))
            {
                auto& eptr_ = *reinterpret_cast<std::optional<std::exception_ptr>*>(
                    call_with(action_type::get_exception));
                std::rethrow_exception(*eptr_);
            }
            return reinterpret_cast<std::optional<T>*>(call_with(action_type::get_result))->value();
        }
        /**
         * Obtains the result, possibly after waiting for some time.
         * Overload for void returning functions
         * @warning UB if valid() == false
         */
        template<class = typename std::enable_if<std::is_void<T>::value>::type>
        void get() const
        {
            wait();
            call_guard guard_{m_context, m_manager};
            if (call_with(action_type::has_exception))
            {
                auto& eptr_ = *reinterpret_cast<std::optional<std::exception_ptr>*>(
                    call_with(action_type::get_exception));
                std::rethrow_exception(*eptr_);
            }
            call_with(action_type::get_result);
        }

        /**
         * Indefinitely waits for call complition.
         * Due to https://cplusplus.github.io/LWG/issue1518 if the call is prepared
         * can excute the call.
         * @warning UB if valid() == false
         */
        void wait() const // noexcept
        {
            {
                call_guard guard_{m_context, m_manager};
                if (call_with(action_type::has_all_arguments) &&
                    !(call_with(action_type::has_result) || call_with(action_type::has_exception)))
                {
                    call_with(action_type::call);
                    return;
                }
            }
            waitable& w_ =
                *reinterpret_cast<waitable*>(call_with(action_type::get_waitable));
            std::unique_lock<std::mutex> lk_(w_.m_mu);
            w_.m_cv.wait(
                lk_,
                [&]{ return call_with(action_type::has_result) || call_with(action_type::has_exception); }
            );
        }
        /**
         * Waits for associated function to be executed for specified amount of time.
         * Due to https://cplusplus.github.io/LWG/issue1518 will not attempt to excute function by itself.
         * @warning UB if valid() == false
         */
        template<class Rep, class Period>
        future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const // noexcept
        {
            waitable& w_ =
                *reinterpret_cast<waitable*>(call_with(action_type::get_waitable));
            std::unique_lock<std::mutex> lk_(w_.m_mu);
            return w_.m_cv.wait_for(
                lk_,
                timeout_duration,
                [&]{ return call_with(action_type::has_result) || call_with(action_type::has_exception); }
            ) ? future_status::ready : future_status::timeout;
        }
        /**
         * Waits until associated function is executed or specified time passes.
         * Due to https://cplusplus.github.io/LWG/issue1518 will not attempt to excute function by itself.
         * @warning UB if valid() == false
         */
        template<class Clock, class Duration>
        future_status wait_until(const std::chrono::time_point<Clock,Duration>& timeout_time) const // noexcept
        {
            waitable& w_ =
                *reinterpret_cast<waitable*>(call_with(action_type::get_waitable));
            std::unique_lock<std::mutex> lk_(w_.m_mu);
            return w_.m_cv.wait_until(
                lk_,
                timeout_time,
                [&]{ return call_with(action_type::has_result) || call_with(action_type::has_exception); }
            ) ? future_status::ready : future_status::timeout;
        }

        /**
         * Returns true if future is not empty.
         */
        bool valid() const noexcept { return static_cast<bool>(*this); }

        explicit operator bool() const noexcept
        {
            return static_cast<bool>(static_cast<const af_ctx_manager&>(*this));
        }
        friend bool operator==(const future& lhs, const future& rhs) noexcept
        {
            return static_cast<const af_ctx_manager&>(lhs) == rhs;
        }
    };

  private:
    /* Auxiliary classes */

    template<class ... Types>
    using optional_tuple = std::tuple<std::optional<Types>...>;

    template<class Counter, class Derived, class Allocator>
    class ref_counted : private Allocator
    {
        Counter m_counter;

        Allocator& get_alloc() noexcept
        {
            return static_cast<Allocator&>(*this);
        }
        const Allocator& get_alloc() const noexcept
        {
            return static_cast<const Allocator&>(*this);
        }

        using alloc_trait = std::allocator_traits<Allocator>;

      public:
        using value_type        = Derived;
        using allocator_type    = typename alloc_trait::allocator_type;
        using pointer           = typename std::add_pointer<value_type>::type;

        template<class = typename std::enable_if<
            std::is_default_constructible<allocator_type>::value
        >::type>
        explicit ref_counted()
            noexcept(std::is_nothrow_default_constructible<allocator_type>::value) :
            m_counter{1}
        {}
        template<class = typename std::enable_if<
            std::is_copy_constructible<allocator_type>::value
        >::type>
        explicit ref_counted(const allocator_type& a)
            noexcept(std::is_nothrow_copy_constructible<allocator_type>::value) :
            allocator_type(a),
            m_counter{1}
        {}
        template<class = typename std::enable_if<
            std::is_move_constructible<allocator_type>::value
        >::type>
        explicit ref_counted(allocator_type&& a)
            noexcept(std::is_nothrow_move_constructible<allocator_type>::value) :
            allocator_type(std::move(a)),
            m_counter{1}
        {}

        inline pointer acquire() noexcept
        {
            ++m_counter;
            return static_cast<pointer>(this);
        }

        inline void release() noexcept(
                std::is_nothrow_destructible<Derived>::value &&
                std::is_nothrow_copy_constructible<Allocator>::value
            )
        {
            if (!(--m_counter))
            {
                auto al_ = get_alloc();
                auto ptr_ = static_cast<pointer>(this);
                //? There is a bug in std::allocator and std::allocator_traits see:
                //? https://stackoverflow.com/questions/52978545/why-is-stdallocatordeallocate-not-noexcept
                alloc_trait::destroy(al_, ptr_);
                alloc_trait::deallocate(al_, ptr_, 1);
            }
        }
    };
    template<class Callable, class Result, class ... Args>
    struct ctx_storage
    {
        std::optional<std::exception_ptr> m_exception;
        std::optional<Callable> m_callable;
        std::optional<Result> m_result;
        optional_tuple<Args...> m_arguments;

        template<class Exception>
        void store_exception() noexcept
        {
            try { throw Exception{}; }
            catch (...)
            {
                if (m_exception.has_value())
                {   //? Here is the second uncought exception -> call terminate
                    throw;
                }
                m_exception = std::current_exception();
            }
        }

        void store_exception() noexcept
        {
            if (m_exception.has_value())
            {   //? Here is the second uncought exception -> call terminate
                throw;
            }
            m_exception = std::current_exception();
        }
    };

    /* Contexts */

    template<template<class> class Allocator, class Callable, class Result, class ... Args>
    struct unsafe_ctx :
        public ref_counted<
            std::size_t,
            unsafe_ctx<Allocator, Callable, Result, Args...>,
            Allocator<unsafe_ctx<Allocator, Callable, Result, Args...>>
            >,
        public ctx_storage<Callable, Result, Args...>
    {
        using ref_base = ref_counted<std::size_t, unsafe_ctx, Allocator<unsafe_ctx>>;
        using ctx_base = ctx_storage<Callable, Result, Args...>;
        using allocator_type = typename ref_base::allocator_type;

        template<class = typename std::enable_if<
            std::is_default_constructible<allocator_type>::value
        >::type>
        explicit unsafe_ctx()
            noexcept(std::is_nothrow_default_constructible<allocator_type>::value) :
            ref_base{},
            ctx_base{}
        {}
        template<class = typename std::enable_if<
            std::is_copy_constructible<allocator_type>::value
        >::type>
        explicit unsafe_ctx(const allocator_type& a)
            noexcept(std::is_nothrow_copy_constructible<allocator_type>::value) :
            ref_base{a},
            ctx_base{}
        {}
        template<class = typename std::enable_if<
            std::is_move_constructible<allocator_type>::value
        >::type>
        explicit unsafe_ctx(allocator_type&& a)
            noexcept(std::is_nothrow_move_constructible<allocator_type>::value) :
            ref_base{std::move(a)},
            ctx_base{}
        {}

        void lock() const noexcept {}
        bool try_lock() const noexcept { return true; }
        void unlock() const noexcept {}
    };
    template<template<class> class Allocator, class Callable, class Result, class ... Args>
    struct shared_ctx :
        public ref_counted<
            std::atomic_size_t,
            shared_ctx<Allocator, Callable, Result, Args...>,
            Allocator<shared_ctx<Allocator, Callable, Result, Args...>>
        >,
        public ctx_storage<Callable, Result, Args...>
    {
        using ref_base = ref_counted<std::atomic_size_t, shared_ctx, Allocator<shared_ctx>>;
        using ctx_base = ctx_storage<Callable, Result, Args...>;
        using allocator_type = typename ref_base::allocator_type;

        template<class = typename std::enable_if<
            std::is_default_constructible<allocator_type>::value
        >::type>
        explicit shared_ctx()
            noexcept(std::is_nothrow_default_constructible<allocator_type>::value) :
            ref_base{},
            ctx_base{}
        {}
        template<class = typename std::enable_if<
            std::is_copy_constructible<allocator_type>::value
        >::type>
        explicit shared_ctx(const allocator_type& a)
            noexcept(std::is_nothrow_copy_constructible<allocator_type>::value) :
            ref_base{a},
            ctx_base{}
        {}
        template<class = typename std::enable_if<
            std::is_move_constructible<allocator_type>::value
        >::type>
        explicit shared_ctx(allocator_type&& a)
            noexcept(std::is_nothrow_move_constructible<allocator_type>::value) :
            ref_base{std::move(a)},
            ctx_base{}
        {}

        void lock() const noexcept {}
        bool try_lock() const noexcept { return true; }
        void unlock() const noexcept {}
    };
    template<template<class> class Allocator, class Callable, class Result, class ... Args>
    struct spinlock_ctx :
        public ref_counted<
            std::atomic_size_t,
            spinlock_ctx<Allocator, Callable, Result, Args...>,
            Allocator<spinlock_ctx<Allocator, Callable, Result, Args...>>
        >,
        public ctx_storage<Callable, Result, Args...>
    {
        std::atomic_flag m_spinlock;

        using ref_base = ref_counted<std::atomic_size_t, spinlock_ctx, Allocator<spinlock_ctx>>;
        using ctx_base = ctx_storage<Callable, Result, Args...>;
        using allocator_type = typename ref_base::allocator_type;

        template<class = typename std::enable_if<
            std::is_default_constructible<allocator_type>::value
        >::type>
        explicit spinlock_ctx()
            noexcept(std::is_nothrow_default_constructible<allocator_type>::value) :
            ref_base{},
            ctx_base{},
            m_spinlock{}
        {}
        template<class = typename std::enable_if<
            std::is_copy_constructible<allocator_type>::value
        >::type>
        explicit spinlock_ctx(const allocator_type& a)
            noexcept(std::is_nothrow_copy_constructible<allocator_type>::value) :
            ref_base{a},
            ctx_base{},
            m_spinlock{}
        {}
        template<class = typename std::enable_if<
            std::is_move_constructible<allocator_type>::value
        >::type>
        explicit spinlock_ctx(allocator_type&& a)
            noexcept(std::is_nothrow_move_constructible<allocator_type>::value) :
            ref_base{std::move(a)},
            ctx_base{},
            m_spinlock{}
        {}

        void lock() noexcept
        {
            while (m_spinlock.test_and_set(std::memory_order_acquire))
                ;
        }
        bool try_lock() noexcept
        {
            return !m_spinlock.test_and_set(std::memory_order_acquire);
        }
        void unlock() noexcept
        {
            m_spinlock.clear(std::memory_order_release);
        }
    };
    template<template<class> class Allocator, class Callable, class Result, class ... Args>
    struct waitable_ctx :
        public ref_counted<
            std::atomic_size_t,
            waitable_ctx<Allocator, Callable, Result, Args...>,
            Allocator<waitable_ctx<Allocator, Callable, Result, Args...>>>,
        public ctx_storage<Callable, Result, Args...>
    {
        waitable m_waitable;

        using ref_base = ref_counted<std::atomic_size_t, waitable_ctx, Allocator<waitable_ctx>>;
        using ctx_base = ctx_storage<Callable, Result, Args...>;
        using allocator_type = typename ref_base::allocator_type;

        template<class = typename std::enable_if<
            std::is_default_constructible<allocator_type>::value
        >::type>
        explicit waitable_ctx()
            noexcept(std::is_nothrow_default_constructible<allocator_type>::value) :
            ref_base{},
            ctx_base{},
            m_waitable{}
        {}
        template<class = typename std::enable_if<
            std::is_copy_constructible<allocator_type>::value
        >::type>
        explicit waitable_ctx(const allocator_type& a)
            noexcept(std::is_nothrow_copy_constructible<allocator_type>::value) :
            ref_base{a},
            ctx_base{},
            m_waitable{}
        {}
        template<class = typename std::enable_if<
            std::is_move_constructible<allocator_type>::value
        >::type>
        explicit waitable_ctx(allocator_type&& a)
            noexcept(std::is_nothrow_move_constructible<allocator_type>::value) :
            ref_base{std::move(a)},
            ctx_base{},
            m_waitable{}
        {}

        void lock() noexcept
        {
            m_waitable.m_mu.lock();
        }
        bool try_lock() noexcept
        {
            return m_waitable.m_mu.try_lock();
        }
        void unlock()
        {
            m_waitable.m_mu.unlock();
        }
    };

    /**
     * Trait that defines wait policy for context
     */
    template<class Context>
    struct waitable_source
    {
        static waitable& get_waitable(Context&) noexcept
        {
            static waitable g_waitable{};
            return g_waitable;
        }
        static void lock_waitable(Context& ctx) noexcept
        {
            get_waitable(ctx).m_mu.lock();
        }
        static void unlock_waitable(Context& ctx) noexcept
        {
            get_waitable(ctx).m_mu.unlock();
        }
        static void notify(Context& ctx) noexcept
        {
            get_waitable(ctx).m_cv.notify_all();
        }
    };
    template<template<class> class Allocator, class Callable, class Result, class ... Args>
    struct waitable_source<unsafe_ctx<Allocator, Callable, Result, Args...>>
    {
        using Context = unsafe_ctx<Allocator, Callable, Result, Args...>;
        static waitable& get_waitable(Context&) noexcept
        {
            static waitable g_waitable{};
            return g_waitable;
        }
        static void lock_waitable(Context&) noexcept {}
        static void unlock_waitable(Context&) noexcept {}
        static void notify(Context&) noexcept {}
    };
    template<template<class> class Allocator, class Callable, class Result, class ... Args>
    struct waitable_source<waitable_ctx<Allocator, Callable, Result, Args...>>
    {
        using Context = waitable_ctx<Allocator, Callable, Result, Args...>;
        static waitable& get_waitable(Context& ctx) noexcept
        {
            return ctx.m_waitable;
        }
        static void lock_waitable(Context&) noexcept {}
        static void unlock_waitable(Context&) noexcept {}
        static void notify(Context& ctx) noexcept
        {
            ctx.m_waitable.m_cv.notify_all();
        }
    };

    /* Core manager function */

    /**
     * @tparam Context One of internal contexts is expected
     * @tparam Callable Pure type of callable object is expected
     * @tparam Result Type of result from signature
     * @tparam Args Types of arguments from signature (with qualification)
     */
    template<class Context, class Callable, class Result, class ... Args>
    static void* core_manager(
        action_type action,
        void* ctx,
        type_id_t id,
        void* arg
    )
    {
        using wait_source = waitable_source<Context>;
        auto& context = *reinterpret_cast<Context*>(ctx);
        switch (action)
        {
            case action_type::up_ref_counter:
            {
                context.acquire();
            } break;
            case action_type::down_ref_counter:
            {
                context.release();
            } break;
            case action_type::lock_call:
            {
                context.lock();
            } break;
            case action_type::try_lock_call:
            {
                if (!context.try_lock())
                {
                    return nullptr;
                }
            } break;
            case action_type::unlock_call:
            {
                context.unlock();
            } break;
            case action_type::get_argument_storage:
            {
                return type_id<optional_tuple<function_argument<Args>...>>() == id ?
                    &context.m_arguments : nullptr;
            } break;
            case action_type::call:
            {
                if (!has_values(context.m_arguments))
                {
                    context.template store_exception<missing_argument>();
                    return nullptr;
                }
                struct notifier
                {
                    Context& c;
                    notifier(Context& c_) noexcept : c{c_}
                    {
                        wait_source::lock_waitable(c);
                    }
                    ~notifier() noexcept
                    {
                        wait_source::unlock_waitable(c);
                        wait_source::notify(c);
                    }
                } notifier_{context};
                if (context.m_result.has_value() || context.m_exception.has_value())
                {   //? No call reentry
                    return ctx;
                }
                try {
                    if constexpr (std::is_void<Result>::value)
                    {
                        apply(*context.m_callable, context.m_args, detail::type_list<Args...>{});
                        context.m_result = as_void{};
                    }
                    else
                    {
                        context.m_result =
                            apply(*context.m_callable, context.m_args, detail::type_list<Args...>{});
                    }
                }
                catch (...)
                {
                    context.store_exception();
                    return nullptr;
                }
            } break;
            case action_type::has_result:
            {
                if (!context.m_result.has_value())
                {
                    return nullptr;
                }
            } break;
            case action_type::has_exception:
            {
                if (!context.m_exception.has_value())
                {
                    return nullptr;
                }
            } break;
            case action_type::has_argument:
            {
                std::size_t i_ = *reinterpret_cast<std::size_t*>(arg);
                if (!ith_element::has_value(i_, context.m_arguments))
                {
                    return nullptr;
                }
            } break;
            case action_type::check_result_type:
            {
                if (type_id<function_result<Result>>() != id)
                {
                    return nullptr;
                }
            } break;
            case action_type::check_argument_type:
            {
                std::size_t i_ = *reinterpret_cast<std::size_t*>(arg);
                if (!ith_element::is_same(i_, id, context.m_arguments))
                {
                    return nullptr;
                }
            } break;
            case action_type::get_result:
            {
                return &context.m_result;
            } break;
            case action_type::get_argument:
            {
                std::size_t i_ = *reinterpret_cast<std::size_t*>(arg);
                return ith_element::get(i_, context.m_arguments);
            } break;
            case action_type::get_exception:
            {
                return &context.m_exception;
            } break;
            case action_type::get_waitable:
            {
                return &wait_source::get_waitable(context);
            } break;
            case action_type::get_argument_count:
            {
                *reinterpret_cast<std::size_t*>(arg) = sizeof...(Args);
            } break;
            case action_type::has_all_arguments:
            {
                if (!has_values(context.m_arguments))
                {
                    return nullptr;
                }
            } break;
        }
        return ctx;
    }

    template<class Context, class Allocator>
    static Context* alloc_context(const Allocator& al)
    {
        using ctx_allocator_trait =
            std::allocator_traits<typename Context::allocator_type>;
        using al_t = typename ctx_allocator_trait::allocator_type;
        al_t al_{al};
        Context* st_ = ctx_allocator_trait::allocate(al_, 1);
        try {
            if constexpr (std::is_move_constructible<al_t>::value)
            {
                ctx_allocator_trait::construct(al_, st_, std::move(al_));
            }
            else if constexpr (std::is_copy_constructible<al_t>::value)
            {
                ctx_allocator_trait::construct(al_, st_, al_);
            }
            else if constexpr (std::is_default_constructible<al_t>::value)
            {
                ctx_allocator_trait::construct(al_, st_);
            }
            else
            {
                throw std::runtime_error{"Unusable allocator provided"};
            }
        }
        catch (...)
        {
            ctx_allocator_trait::deallocate(al_, st_, 1);
            throw;
        }
        return st_;
    }

    template<template<class> class Allocator, class Callable, class R, class... Args>
    static auto context_trait_impl(unsafe_tag) -> unsafe_ctx<Allocator, Callable, R, Args...>;
    template<template<class> class Allocator, class Callable, class R, class... Args>
    static auto context_trait_impl(shared_tag) -> shared_ctx<Allocator, Callable, R, Args...>;
    template<template<class> class Allocator, class Callable, class R, class... Args>
    static auto context_trait_impl(spinlock_tag) -> spinlock_ctx<Allocator, Callable, R, Args...>;
    template<template<class> class Allocator, class Callable, class R, class... Args>
    static auto context_trait_impl(waitable_tag) -> waitable_ctx<Allocator, Callable, R, Args...>;
    template<class Tag, template<class> class Allocator, class Callable, class R, class... Args>
    struct context_trait
    {
        using type = decltype(context_trait_impl<Allocator, Callable, R, function_argument<Args>...>(Tag{}));
    };

  protected:
    /* Tests only section */
    constexpr any_function(void* context, manager_function* manager) noexcept :
        af_ctx_manager{context, manager} {}

    static any_function any_function_fabric(void* context, manager_function* manager) noexcept
    {
        return {context, manager};
    }

  public:
    constexpr any_function() noexcept : af_ctx_manager{} {}
    any_function(const any_function&) noexcept = default;
    any_function(any_function&&) noexcept = default;
    any_function& operator=(const any_function&) = default;
    any_function& operator=(any_function&&) noexcept = default;
    ~any_function() = default;

    /**
     * Constructs any_function from function pointer using specified
     * allocator template for the context allocator.
     */
    template<class Tag, class T, template<class> class Allocator, class R, class... Args>
    any_function(Tag, R(*f)(Args...), const Allocator<T>& al) : any_function{}
    {
        using signature = decltype(get_signature(f));
        using context_type = typename context_trait<
            Tag,
            Allocator,
            decltype(f),
            typename signature::result_type,
            Args...
        >::type;
        context_type* storage_ = alloc_context<context_type>(al);
        storage_->m_callable = f;
        m_context = storage_;
        m_manager = &core_manager<
            context_type,
            decltype(f),
            typename signature::result_type,
            Args...
        >;
    }
    /**
     * Constructs any_function from any callable and member function pointer
     * using specified allocator template for the context allocator.
     */
    template<class Tag, class C, class T, template<class> class Allocator, class R, class... Args>
    any_function(Tag, C&& callable, R(C::*f)(Args...), const Allocator<T>& al) : any_function{}
    {
        using signature = decltype(get_signature(f));
        using context_type = typename context_trait<
            Tag,
            Allocator,
            typename std::remove_reference<C>::type,
            typename signature::result_type,
            Args...
        >::type;
        context_type* storage_ = alloc_context<context_type>(al);
        storage_->m_callable = std::forward<C>(callable);
        m_context = storage_;
        m_manager = &core_manager<
            context_type,
            typename std::remove_reference<C>::type,
            typename signature::result_type,
            Args...
        >;
    }
    /**
     * Constructs any_function from any_function::future
     */
    template<class T>
    explicit any_function(const future<T>& fut) noexcept : af_ctx_manager{fut} {}

    /**
     * Explicitly obtains any_function::future from any_function object
     * @throw any_function::bad_type_cast If specified T is not a type of result
     */
    template<class T>
    future<T> get_future() const { return future<T>{*this}; }

    inline void swap(any_function& other) noexcept
    {
        using std::swap;
        swap(static_cast<af_ctx_manager&>(*this), other);
    }
    inline friend void swap(any_function& lhs, any_function& rhs) noexcept
    {
        lhs.swap(rhs);
    }

    /* Result or Exception */

    /**
     * Tests if any_function context have a result ready or exception stored
     * @warning UB if any_function is empty
     */
    bool has_anything() const noexcept
    {
        call_guard guard_{m_context, m_manager};
        return  call_with(action_type::has_result) ||
                call_with(action_type::has_exception);
    }

    /* Result */

    /**
     * Tests if any_function context have a result ready
     * @warning UB if any_function is empty
     */
    bool has_result() const noexcept
    {
        call_guard guard_{m_context, m_manager};
        return call_with(action_type::has_result);
    }
    /**
     * Tests if any_function context have a result of specified type T
     * @warning UB if any_function is empty
     */
    template<class T>
    bool is_result_of_type() const noexcept
    {
        return call_with(action_type::check_result_type, type_id<function_result<T>>());
    }
    /**
     * Casts result to specified type T
     * @warning Not available for (possibly cv) void type
     * @throw any_function::bad_type_cast If specified T is not a type of result
     * @return reference to std::optional of some type, convertiable to T
     * @warning UB if any_function is empty
     */
    template<class T>
    friend typename std::enable_if<!std::is_void<T>::value,
    std::optional<function_result<T>>&>::type result_cast(const any_function& af)
    {
        if (!af.is_result_of_type<T>())
        {
            throw bad_type_cast{};
        }
        call_guard guard_{af.m_context, af.m_manager};
        return *reinterpret_cast<std::optional<function_result<T>>*>(
            af.call_with(action_type::get_result));
    }
    /**
     * Casts result to specified type T
     * @warning Not available for (possibly cv) void type
     * @return nullptr if type cast failed, or
     *      pointer to std::optional of some type, convertiable to T
     * @warning UB if any_function is empty
     */
    template<class T>
    friend typename std::enable_if<!std::is_void<T>::value,
    std::optional<function_result<T>>*>::type result_cast(const any_function* af) noexcept
    {
        if (!af->is_result_of_type<T>())
        {
            return nullptr;
        }
        call_guard guard_{af->m_context, af->m_manager};
        return reinterpret_cast<std::optional<function_result<T>>*>(
            af->call_with(action_type::get_result));
    }

    /* Exception */

    /**
     * Tests if any_function context have any exception stored
     * @warning UB if any_function is empty
     */
    bool has_exception() const noexcept
    {
        call_guard guard_{m_context, m_manager};
        return call_with(action_type::has_exception);
    }
    /**
     * Rethrows exception stored by any_function context
     * @warning UB if no exception is stored
     * @warning UB if any_function is empty
     */
    void rethrow() const
    {
        auto& eptr_ = *reinterpret_cast<std::optional<std::exception_ptr>*>(
                call_with(action_type::get_exception));
        std::rethrow_exception(*eptr_);
    }

    /* Arguments */

    /**
     * Tests if any_function context have all arguments setup for call to be executed
     * @warning UB if any_function is empty
     */
    bool has_arguments() const noexcept
    {
        return call_with(action_type::has_all_arguments);
    }
    /**
     * Tests if any_function context have n'th argument setup
     * @warning UB if any_function is empty
     */
    bool has_argument(std::size_t n) const noexcept
    {
        return call_with(action_type::has_argument, null_id, &n);
    }
    /**
     * Returns the number of arguments stored callabe have
     * @warning UB if any_function is empty
     */
    std::size_t argument_count() const noexcept
    {
        std::size_t n_{0};
        call_with(action_type::get_argument_count, null_id, &n_);
        return n_;
    }
    /**
     * Tests if any_function context have n'th argument of specified type T
     * @warning UB if any_function is empty
     */
    template<class T>
    bool is_argument_of_type(std::size_t n) const noexcept
    {
        return call_with(action_type::check_argument_type, type_id<function_argument<T>>(), &n);
    }
    /**
     * Casts n'th argument to specified type T
     * @throw any_function::bad_type_cast If specified T is not a type of n'th argument
     * @return reference to std::optional of some type, convertiable to T
     * @warning UB if any_function is empty
     */
    template<class T>
    friend std::optional<function_argument<T>>&
        argument_cast(const any_function& af, std::size_t n)
    {
        if (!af.is_argument_of_type<T>(n))
        {
            throw bad_type_cast{};
        }
        call_guard guard_{af.m_context, af.m_manager};
        return *reinterpret_cast<std::optional<function_argument<T>>*>(
            af.call_with(action_type::get_argument, null_id, &n));
    }
    /**
     * Casts n'th argument to specified type T
     * @return nullptr if type cast failed, or
     *      pointer to std::optional of some type, convertiable to T
     * @warning UB if any_function is empty
     */
    template<class T>
    friend std::optional<function_argument<T>>*
        argument_cast(const any_function* af, std::size_t n) noexcept
    {
        if (!af->is_argument_of_type<T>(n))
        {
            return nullptr;
        }
        call_guard guard_{af->m_context, af->m_manager};
        return reinterpret_cast<std::optional<function_argument<T>>*>(
            af->call_with(action_type::get_argument, null_id, &n));
    }

    /* Call operation */

    /**
     * Returns true if context can be called.
     * All arguments are ready and no result or exception is stored
     * @warning UB if any_function is empty
     */
    bool is_prepared() const noexcept
    {
        call_guard guard_{m_context, m_manager};
        return call_with(action_type::has_all_arguments) &&
            !(call_with(action_type::has_result) || call_with(action_type::has_exception));
    }
    /**
     * Call with provided parameters
     * @warning UB if any_function is empty
     */
    template<class ... Args>
    call_result operator()(Args&& ... args) const
    {
        using opt_args = optional_tuple<function_argument<Args>...>;
        call_guard guard_{m_context, m_manager};
        auto* arg_storage_ =
            call_with(action_type::get_argument_storage, type_id<opt_args>());
        if (!arg_storage_)
        {
            return call_result::arguments_not_accepted;
        }
        auto& args_ = *reinterpret_cast<opt_args*>(arg_storage_);
        args_ = std::make_tuple(std::forward<Args>(args)...);
        return call_with(action_type::call) ?
            call_result::succeeded : call_result::exception;
    }
    /**
     * Call with stored parameters
     * @warning UB if any_function is empty
     */
    call_result operator()() const noexcept
    {
        call_guard guard_{m_context, m_manager};
        return call_with(action_type::call) ?
            call_result::succeeded : call_result::exception;
    }

    /* Validity and Equality */

    /**
     * Returns true if any_function is not empty.
     */
    bool valid() const noexcept { return static_cast<bool>(*this); }

    explicit operator bool() const noexcept
    {
        return static_cast<bool>(static_cast<const af_ctx_manager&>(*this));
    }
    friend bool operator==(const any_function& lhs, const any_function& rhs) noexcept
    {
        return static_cast<const af_ctx_manager&>(lhs) == rhs;
    }
};

} // namespace any_function
} // namespace detail

using any_function = detail::any_function::any_function;

/**
 * Helper that constructs any_function from function pointer.
 * Rebinds provided allocator to handle type of the context.
 * @tparam Tag Defines the context type
 * @tparam Allocator Defines the type of allocator to be used
 */
template<class Tag, class Allocator, class R, class... Args>
auto make_function(R(*f)(Args...), const Allocator& al = Allocator())
    -> std::pair<any_function, any_function::future<R>>
{
    auto af_ = any_function(Tag{}, f, al);
    return {af_, af_.get_future()};
}

/**
 * Helper that constructs any_function from any callable and signature.
 * Obtains callable operator() from provided signature.
 * Rebinds provided allocator to handle type of the context.
 * @warning Defined only if Callable has operator()
 * @tparam Tag Defines the context type
 * @tparam Signature The function signature of form R(Args...)
 * @tparam Allocator Defines the type of allocator to be used
 * @todo Add detection of that callable have operator() with provided signature
 */
template<class Tag, class Signature, class Allocator, class Callable,
    class sig_trait = detail::any_function::signature_trait<Signature>,
    class = typename std::enable_if<detail::any_function::is_callable<Callable>::value>::type
>
auto make_function(Callable&& c, const Allocator& al = Allocator()) ->
    std::pair<
        any_function,
        any_function::future<typename sig_trait::resutl_type>
    >
{
    using mem_sig_trait =
        typename detail::any_function::translate_signature<typename sig_trait::type, Callable>::type;
    typename mem_sig_trait::type f_ = &Callable::operator();
    auto af_ = any_function(Tag{}, std::forward<Callable>(c), f_, al);
    return {af_, af_.get_future()};
}

} // namespace ecsl
#endif /* ECSL_UTILITY_ANY_FUNCTION_HPP_ */