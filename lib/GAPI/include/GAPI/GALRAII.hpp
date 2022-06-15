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
using RootHandle = RootHandle<H, RootHandleDeleter>;

template<class H>
struct InstanceHandle: ChildHandle<GAL::Instance, H, InstanceChildHandleDeleter> {
    using ChildHandle<GAL::Instance, H, InstanceChildHandleDeleter>::ChildHandle;

    GAL::Instance get_instance() const noexcept {
        return this->get_deleter().get_handle();
    }
};
}

template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Instance>      = GAL::DestroyInstance;
template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Context>       = GAL::DestroyContext;
template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Surface>       = GAL::DestroySurface;
template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Swapchain>     = GAL::DestroySwapchain;
using HInstance     = Detail::RootHandle<GAL::Instance>;
using HContext      = Detail::RootHandle<GAL::Context>;
using HSurface      = Detail::RootHandle<GAL::Surface>;
using HSwapchain    = Detail::RootHandle<GAL::Swapchain>;

}
