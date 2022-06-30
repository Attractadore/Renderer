#pragma once
#include "GAL/GAL.hpp"
#include "Common/RAII.hpp"

namespace R1::GAPI {
namespace Detail {
template<class H>
constexpr int DoRootDeleteF = 0;

struct RootHandleDeleter {
    template<class H>
    static void DoDelete(H handle) noexcept {
        DoRootDeleteF<H>(handle);
    }
};

template<class H>
constexpr int DoInstanceDeleteF = 0;

struct InstanceChildHandleDeleter {
    template<class H>
    static void DoDelete(GAL::Instance instance, H handle) noexcept {
        DoInstanceDeleteF<H>(instance, handle);
    }
};

template<class H>
constexpr int DoContextDeleteF = 0;

struct ContextChildHandleDeleter {
    template<class H>
    static void DoDelete(GAL::Context ctx, H handle) noexcept {
        DoContextDeleteF<H>(ctx, handle);
    }
};

template<class H>
using RootHandle = RootHandle<H, RootHandleDeleter>;

template<class H>
struct InstanceHandle: ChildHandle<GAL::Instance, H, InstanceChildHandleDeleter> {
    using ChildHandle<GAL::Instance, H, InstanceChildHandleDeleter>::ChildHandle;

    GAL::Instance get_instance() const noexcept {
        return this->get_deleter().get_handle();
    }
};

template<class H>
struct ContextHandle: ChildHandle<GAL::Context, H, ContextChildHandleDeleter> {
    using ChildHandle<GAL::Context, H, ContextChildHandleDeleter>::ChildHandle;

    GAL::Context get_context() const noexcept {
        return this->get_deleter().get_handle();
    }
};
}

template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Instance>   = GAL::DestroyInstance;
template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Context>    = GAL::DestroyContext;
using HInstance = Detail::RootHandle<GAL::Instance>;
using HContext  = Detail::RootHandle<GAL::Context>;

template<> inline constexpr auto GAPI::Detail::DoContextDeleteF<GAL::Semaphore>     = GAL::DestroySemaphore;
template<> inline constexpr auto GAPI::Detail::DoContextDeleteF<GAL::CommandPool>   = GAL::DestroyCommandPool;
using HSemaphore    = Detail::ContextHandle<GAL::Semaphore>;
using HCommandPool  = Detail::ContextHandle<GAL::CommandPool>;
}
