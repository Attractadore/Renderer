#pragma once
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <atomic>

namespace R1 {
namespace Detail {
template<typename T>
concept atomic_value = requires(const T v) {
    { v.is_lock_free() } -> std::same_as<bool>;
};

template<typename T>
struct remove_atomic {
    using type = T;
};

template<typename A>
    requires atomic_value<A>
struct remove_atomic<A> {
    using type = A::value_type;
};

template<typename T>
using remove_atomic_t = remove_atomic<T>::type;

template<typename T>
concept complete_type = requires(T) {
    sizeof(T);
};
}
    
template<class Derived, typename Counter = std::atomic<unsigned>>
    requires std::unsigned_integral<Detail::remove_atomic_t<Counter>>
class RefedBase;

template<typename T> requires
    (not Detail::complete_type<T>) or
    std::derived_from<T, RefedBase<T, typename T::counter_type>>
class Ref: private boost::intrusive_ptr<T> {
public:
    Ref() noexcept: Ref{nullptr} {}
    explicit Ref(T* p) noexcept: boost::intrusive_ptr<T>{p, /*add_ref=*/ false} {}

    Ref& operator=(T* p) noexcept {
        reset(p);
        return *this;
    }
    
    void reset(T* p = nullptr) noexcept {
        *this = Ref{p};
    }

    using boost::intrusive_ptr<T>::operator*;
    using boost::intrusive_ptr<T>::operator->;
    using boost::intrusive_ptr<T>::operator bool;
    using boost::intrusive_ptr<T>::get;
    using boost::intrusive_ptr<T>::swap;
};

template<class Derived, typename Counter>
    requires std::unsigned_integral<Detail::remove_atomic_t<Counter>>
class RefedBase {
    Counter m_counter = 1;

public:
    using counter_type = Counter;
    using counter_value_type = std::conditional_t<Detail::atomic_value<Counter>,
        typename Counter::value_type,
        Counter
    >;

private:
    auto Increment() noexcept {
        return ++m_counter;
    }

    auto Decrement() noexcept {
        return --m_counter;
    }

    friend void intrusive_ptr_add_ref(RefedBase* p) noexcept {
        p->Increment();
    }

    friend void intrusive_ptr_release(RefedBase* p) noexcept {
        if (p->Decrement() == 0) {
            delete static_cast<Derived*>(p);
        }
    }

public:
    template<typename... Args>
    static auto Create(Args... args) {
        struct EnableNew: Derived {
            EnableNew(Args... args): Derived{std::forward<Args>(args)...} {}
        };
        return Ref<Derived>(
            new EnableNew(std::forward<Args>(args)...)
        );
    }

    counter_value_type GetUseCount() const {
        return m_counter;
    }
};
}
