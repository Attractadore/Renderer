#pragma once
#include "GAL/Vulkan/Swapchain.hpp"
#include "GAPI/GALRAII.hpp"

namespace R1::GAPI {
template<> inline constexpr auto Detail::DoRootDeleteF<GAL::Vulkan::Swapchain> = GAL::Vulkan::DestroySwapchain;
namespace Vulkan {
using HSwapchain = Detail::RootHandle<GAL::Vulkan::Swapchain>;
}

template<> inline constexpr auto Detail::DoContextDeleteF<GAL::Vulkan::Fence> = GAL::Vulkan::DestroyFence;
namespace Vulkan {
using HFence = Detail::ContextHandle<GAL::Vulkan::Fence>;
}
}
