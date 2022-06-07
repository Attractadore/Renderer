#pragma once
#include "Common/Handle.hpp"

#include <cassert>
#include <functional>

namespace R1 {
// TODO
template<class D, typename... Args>
concept DeleterInterface = true;

namespace Detail {
template<class D>
struct RootHandleDeleter {
    template<class H>
        requires DeleterInterface<D, H>
    void operator()(H handle) const noexcept {
        std::invoke(D::template DoDelete<H>, handle);
    }
};

template<class P, DeleterInterface D>
class ChildHandleDeleter {
    P m_parent;

public:
    explicit ChildHandleDeleter(P parent):
        m_parent{parent} {}

    P get_parent() const noexcept { return m_parent; }

    template<class H>
        requires DeleterInterface<D, P, H>
    void operator()(H handle) const noexcept {
        if (handle) {
            assert(m_parent);
            std::invoke(D::template DoDelete<H>, m_parent, handle);
        }
    }
};
}

template<class H, DeleterInterface<H> D>
class RootHandle: Handle<H, Detail::RootHandleDeleter<D>> {
    using Deleter = Detail::RootHandleDeleter<D>;
    using Base = Handle<H, Deleter>;

public:
    explicit RootHandle(H handle):
        Base{handle} {}

    using Base::get;
    using Base::reset;
    using Base::release;
    using Base::operator bool;
};

template<class P, class H, DeleterInterface<P, H> D>
class ChildHandle: Handle<H, Detail::ChildHandleDeleter<P, D>> {
    using Deleter = Detail::ChildHandleDeleter<P, D>;
    using Base = Handle<H, Deleter>;

public:
    ChildHandle(P parent, H handle):
        Base{handle, Deleter{parent}} {}

    using Base::get;
    using Base::reset;
    using Base::release;
    using Base::operator bool;

protected:
    using Base::get_deleter;
};
}
