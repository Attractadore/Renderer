#include "GAL/GALWin32.hpp"
#include "InstanceImpl.hpp"
#include "SwapchainImpl.hpp"
#include "VKUtil.hpp"

#include <vulkan/vulkan_win32.h>

namespace R1::GAL::Win32 {
Instance CreateInstance(HDC, const InstanceConfig& config) {
    static constexpr std::array exts = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    };
    auto instance = CreateVkInstanceWithExtensions(config, exts);
    if (!instance) {
        throw std::runtime_error{"Vulkan: Failed to create Win32 instance"};
    }
    return CreateInstanceFromVK(std::move(instance));
}

Surface CreateSurface(
    Instance instance,
    HWND hWnd
) {
    VkWin32SurfaceCreateInfoKHR create_info = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hinstance = reinterpret_cast<HINSTANCE>(
            GetClassLongPtr(hWnd, GWLP_HINSTANCE)),
        .hwnd = hWnd,
    };
    VkSurfaceKHR surf;
    ThrowIfFailed(vkCreateWin32SurfaceKHR(
        instance->instance.get(), &create_info, nullptr, &surf),
        "Vulkan: Failed to create Win32 surface");
    return CreateSurfaceFromHandle(instance, {instance->instance.get(), surf});
}
}
