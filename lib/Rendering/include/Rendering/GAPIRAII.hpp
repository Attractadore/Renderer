#pragma once
#include "GAPI.hpp"
#include "Common/RAII.hpp"

namespace R1::Rendering {
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
    static void DoDelete(GAPI::Instance instance, H handle) noexcept {
        DoInstanceDeleteF<H>(instance, handle);
    }
};

template<class H>
using RootHandle = RootHandle<H, RootHandleDeleter>;

template<class H>
struct InstanceHandle: ChildHandle<GAPI::Instance, H, InstanceChildHandleDeleter> {
    using ChildHandle<GAPI::Instance, H, InstanceChildHandleDeleter>::ChildHandle;

    GAPI::Instance get_instance() const noexcept {
        return this->get_deleter().get_handle();
    }
};
}

template<> inline constexpr auto Detail::DoRootDeleteF<GAPI::Instance>      = GAPI::DestroyInstance;
template<> inline constexpr auto Detail::DoRootDeleteF<GAPI::Context>       = GAPI::DestroyContext;
template<> inline constexpr auto Detail::DoRootDeleteF<GAPI::Swapchain>     = GAPI::DestroySwapchain;
using HInstance     = Detail::RootHandle<GAPI::Instance>;
using HContext      = Detail::RootHandle<GAPI::Context>;
using HSwapchain    = Detail::RootHandle<GAPI::Swapchain>;

template<> inline constexpr auto Detail::DoInstanceDeleteF<GAPI::Surface>   = GAPI::DestroySurface;
using HSurface      = Detail::InstanceHandle<GAPI::Surface>;
}
