#include "Common/Declarations.hpp"
#include "Common/Vector.hpp"
#include "DeviceImpl.hpp"
#include "InstanceImpl.hpp"
#include "VKUtil.hpp"

namespace R1::GAL {
class ExtensionProperties {
    std::vector<VkExtensionProperties> m_properties;
    std::vector<std::string_view>      m_names;

    static std::vector<std::string_view> GetExtensionNames(std::span<const VkExtensionProperties> props) {
        auto v = ranges::views::transform(props, [] (const auto& ext_prop) {
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

namespace {
VkInstance CreateInstance(
    const VkInstanceCreateInfo* create_template,
    std::span<const char* const> req_layers,
    std::span<const char* const> req_exts
) {
    constexpr auto req_ver = VK_API_VERSION_1_3;
    VkApplicationInfo app_info = {
        .sType = SType(app_info),
        .pEngineName = "R1",
        .apiVersion = req_ver,
    };

    if (auto app_info_template = create_template->pApplicationInfo) {
        if (auto api_ver = app_info_template->apiVersion) {
            if (VK_API_VERSION_VARIANT(api_ver) != 0) {
                return VK_NULL_HANDLE;
            }
            if (VK_API_VERSION_MAJOR(api_ver) != VK_API_VERSION_MAJOR(req_ver)) {
                return VK_NULL_HANDLE;
            }
            app_info.apiVersion = std::max(app_info.apiVersion, api_ver);
        }

        app_info.applicationVersion = app_info_template->applicationVersion;
        app_info.pApplicationName = app_info_template->pApplicationName;
    }

    auto merge_spans = [] (
        std::span<const char* const> usr, std::span<const char* const> req
    ) {
        std::vector<const char*> result;
        result.reserve(usr.size() + req.size());
        result.insert(result.end(), usr.begin(), usr.end());
        result.insert(result.end(), req.begin(), req.end());
        return result;
    };

    std::span<const char* const> usr_layers{
        create_template->ppEnabledLayerNames, create_template->enabledLayerCount};
    auto layers = merge_spans(usr_layers, req_layers);
    std::span<const char* const> usr_exts{
        create_template->ppEnabledExtensionNames, create_template->enabledExtensionCount};
    auto exts = merge_spans(usr_exts, req_exts);

    VkInstanceCreateInfo create_info = {
        .sType = SType(create_info),
        .pNext = create_template->pNext,
        .flags = create_template->flags,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(exts.size()),
        .ppEnabledExtensionNames = exts.data(),
    };

    VkInstance instance = VK_NULL_HANDLE;
    ThrowIfFailed(
        vkCreateInstance(&create_info, nullptr, &instance),
        "Vulkan: Failed to create instance");

    return instance;
}

std::vector<QueueFamily> GetDeviceQueueFamilies(VkPhysicalDevice dev) {
    auto queue_families = Enumerate<VkQueueFamilyProperties>(dev, vkGetPhysicalDeviceQueueFamilyProperties);
    auto v = ranges::views::transform(queue_families, [&] (const auto& qf) {
        return QueueFamily {
            .id = static_cast<QueueFamily::ID>(&qf - &queue_families[0]),
            .capabilities = QueueCapabilitiesFromVK(qf.queueFlags),
            .count = qf.queueCount,
        };
    });
    return vec_from_range(v);
}

VKDeviceDescription GetDeviceDescription(VkPhysicalDevice dev) {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(dev, &props);
    DeviceExtensionProperties ext_props{dev};
    return {
        .common = {
            .name = props.deviceName,
            .type = static_cast<DeviceType>(props.deviceType),
            .queue_families = GetDeviceQueueFamilies(dev),
            .wsi = ext_props.ExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME),
        },
        .api_version = props.apiVersion,
    };
};

std::vector<DeviceImpl> EnumerateDevices(VkInstance instance) {
    // TODO: filter out devices that don't support required features and extensions
    auto pdevs = Enumerate<VkPhysicalDevice>(instance, vkEnumeratePhysicalDevices);
    auto v = ranges::views::transform(pdevs,
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

std::span<const char* const> GetRequiredInstanceLayers() {
    static constexpr auto layers = [] {
        if constexpr (R1::Debug) {
            return std::array {
                "VK_LAYER_KHRONOS_validation"
            };
        } else {
            return std::array<const char*, 0>{};
        }
    } ();
    return layers;
}

std::span<const char* const> GetRequiredInstanceExtensions() {
    static constexpr std::array<const char*, 0> exts;
    return exts;
}
}

Instance Vulkan::CreateInstanceFromTemplate(
    PFN_vkGetInstanceProcAddr,
    const VkInstanceCreateInfo* create_template
) {
    auto instance = std::make_unique<InstanceImpl>();
    instance->instance.reset(CreateInstance(
        create_template,
        GetRequiredInstanceLayers(),
        GetRequiredInstanceExtensions()));
    instance->devices = EnumerateDevices(instance->instance.get());
    // TODO: build a dispatch table
    return instance.release();
}

VkInstance Vulkan::GetVkInstance(Instance instance) {
    return instance->instance.get();
}

VkPhysicalDevice Vulkan::GetVkPhysicalDevice(Device device) {
    return device->physical_device;
}

void DestroyInstance(Instance instance) {
    delete instance;
}

size_t GetDeviceCount(Instance instance) {
    return instance->devices.size();
}

Device GetDevice(Instance instance, size_t idx) {
    assert(idx < GetDeviceCount(instance));
    return &instance->devices[idx];
}

const DeviceDescription& GetDeviceDescription(Device dev) {
    return dev->description.common;
}
}
