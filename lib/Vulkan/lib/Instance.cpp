#include "API.hpp"
#include "Common/Vector.hpp"
#include "Internal.hpp"
#include "VKUtil.hpp"

namespace R1::VK {
class ExtensionProperties {
    std::vector<VkExtensionProperties> m_properties;
    std::vector<std::string_view>      m_names;

    static std::vector<std::string_view> GetExtensionNames(std::span<const VkExtensionProperties> props) {
        auto v = std::views::transform(props, [] (const auto& ext_prop) {
            return std::string_view{ext_prop.extensionName};
        });
        auto ext_prop_name_svs = vec_from_range(v);
        std::ranges::sort(ext_prop_name_svs);
        return ext_prop_name_svs;
    }

public:
    bool ExtensionSupported(std::string_view ext) const {
        return std::ranges::binary_search(m_names, ext);
    }

protected:
    ExtensionProperties(std::vector<VkExtensionProperties>&& props):
        m_properties{std::move(props)},
        m_names{GetExtensionNames(m_properties)} {}
};

class InstanceExtensionProperties: public ExtensionProperties {
    static auto GetInstanceExtensionProperties() {
        return Enumerate<VkExtensionProperties> ([] (auto* cnt, auto* props) {
            return vkEnumerateInstanceExtensionProperties(nullptr, cnt, props);
        });
    }

public:
    InstanceExtensionProperties():
        ExtensionProperties{GetInstanceExtensionProperties()} {}
};

class DeviceExtensionProperties: public ExtensionProperties {
    static auto GetDeviceExtensionProperties(VkPhysicalDevice dev) {
        return Enumerate<VkExtensionProperties> (dev, [] (auto dev, auto* cnt, auto* props) {
            return vkEnumerateDeviceExtensionProperties(dev, nullptr, cnt, props);
        });
    }

public:
    DeviceExtensionProperties(VkPhysicalDevice dev):
        ExtensionProperties{GetDeviceExtensionProperties(dev)} {}
};

InstanceDescription GetInstanceDescription() {
    InstanceExtensionProperties ext_props;
    return {
        .wsi    = ext_props.ExtensionSupported(VK_KHR_SURFACE_EXTENSION_NAME),
        .xlib   = ext_props.ExtensionSupported(VK_KHR_XLIB_SURFACE_EXTENSION_NAME),
        .xcb    = ext_props.ExtensionSupported(VK_KHR_XCB_SURFACE_EXTENSION_NAME),
    };
};

namespace {
VkInstance CreateInstance(
    const InstanceConfig& config,
    std::span<const char* const> layers,
    std::span<const char* const> exts
) {
    VkApplicationInfo app_info = {
        .sType = sType(app_info),
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo create_info = {
        .sType = sType(create_info),
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(exts.size()),
        .ppEnabledExtensionNames = exts.data(),
    };

    VkInstance instance = VK_NULL_HANDLE;
    vkCreateInstance(&create_info, nullptr, &instance);

    return instance;
}

std::vector<QueueFamily> GetDeviceQueueFamilies(VkPhysicalDevice dev) {
    auto queue_families = Enumerate<VkQueueFamilyProperties>(dev, vkGetPhysicalDeviceQueueFamilyProperties);
    auto v = std::views::transform(queue_families, [] (const auto& qf) {
        return QueueFamily {
            .capabilities = QueueCapabilitiesFromVK(qf.queueFlags),
            .count = qf.queueCount,
        };
    });
    return vec_from_range(v);
}

DeviceDescription GetDeviceDescription(VkPhysicalDevice dev) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(dev, &props);
    DeviceExtensionProperties ext_props{dev};
    return {
        .name = props.deviceName,
        .type = static_cast<DeviceType>(props.deviceType),
        .queue_families = GetDeviceQueueFamilies(dev),
        .wsi = ext_props.ExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME),
    };
};

std::vector<DeviceImpl> EnumerateDevices(VkInstance instance) {
    auto pdevs = Enumerate<VkPhysicalDevice>(instance, vkEnumeratePhysicalDevices);
    auto v = std::views::transform(pdevs,
        [&] (VkPhysicalDevice pdev) {
            return DeviceImpl {
                .instance = instance,
                .physical_device = pdev,
                .description = GetDeviceDescription(pdev),
            };
        }
    );
    return vec_from_range(v);
}
}

namespace Detail {
Vk::Instance CreateVkInstance(
    const InstanceConfig& config, 
    std::span<const char* const> layers,
    std::span<const char* const> exts
) {
    return Vk::Instance {
        CreateInstance(config, layers, exts)
    };
}
}

Instance CreateInstanceFromVK(Vk::Instance handle) {
    auto instance = std::make_unique<InstanceImpl>(InstanceImpl{
        .instance = std::move(handle),
    });
    instance->devices = EnumerateDevices(instance->instance.get());
    return instance.release();
}
}
