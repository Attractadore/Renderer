#include "VKContext.hpp"
#include "VKInstance.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <functional>
#include <ranges>

namespace R1::VK {
namespace {
VkInstance createInstance(std::span<const char* const> wsi_extensions) {
    VkInstance inst = VK_NULL_HANDLE;

    std::vector<const char*> layers;
    if constexpr (R1_DEBUG) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    std::vector<const char*> extensions(wsi_extensions.begin(), wsi_extensions.end());
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    VkApplicationInfo app_info = {
        .sType = sType(app_info),
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo create_info = {
        .sType = sType(create_info),
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    vkCreateInstance(&create_info, nullptr, &inst);
    return inst;
}

auto getDeviceQueueFamilies(VkPhysicalDevice dev) {
    uint32_t prop_cnt;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &prop_cnt, nullptr);
    std::vector<VkQueueFamilyProperties> props(prop_cnt);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &prop_cnt, props.data());

    return props;
}

template<std::input_iterator I>
int findGraphicsQueue(I first, I last) {
    int i = 0;
    for (; first != last; ++first, i++) {
        if (first->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }
    return QueueFamilies::NotFound;
}

QueueFamilies findDeviceQueueFamilies(VkPhysicalDevice dev) {
    auto props = getDeviceQueueFamilies(dev);
    return {
        .graphics = findGraphicsQueue(props.begin(), props.end())
    };
}
}

void Instance::init() {
    VkResult r = VK_SUCCESS;

    uint32_t dev_cnt;
    r = vkEnumeratePhysicalDevices(m_instance.get(), &dev_cnt, nullptr);
    if (r != VK_SUCCESS) {
        throw std::runtime_error{"Vulkan: Failed to retrieve device count"};
    }
    std::vector<VkPhysicalDevice> devices(dev_cnt);
    r = vkEnumeratePhysicalDevices(m_instance.get(), &dev_cnt, devices.data());
    if (r != VK_SUCCESS) {
        throw std::runtime_error{"Vulkan: Failed to retrieve devices"};
    }

    for (DeviceID i = 0; i < dev_cnt; i++) {
        auto device = devices[i];
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);
        auto queue_families = findDeviceQueueFamilies(device);

        bool can_use = [&] {
        auto checks = make_array<std::function<bool()>>(
            [&] {
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(device, &props);
                bool has_vulkan_13 = props.apiVersion >= VK_API_VERSION_1_3;
                return has_vulkan_13;
            },
            [&] {
                bool has_graphics = queue_families.graphics != queue_families.NotFound;
                return has_graphics;
            },
            [&] {
                VkBool32 can_present = false;
                r = vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_families.graphics, m_surface.get(), &can_present);
                if (r != VK_SUCCESS) {
                    throw std::runtime_error{"Vulkan: Failed to queury device present support"};
                }
                return can_present;
            },
            [&] {
                uint32_t ext_cnt;
                r = vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_cnt, nullptr);
                if (r != VK_SUCCESS) {
                    throw std::runtime_error{"Vulkan: Failed to retrieve device extension count"};
                }
                std::vector<VkExtensionProperties> exts(ext_cnt);
                r = vkEnumerateDeviceExtensionProperties(device, nullptr, &ext_cnt, exts.data());
                if (r != VK_SUCCESS) {
                    throw std::runtime_error{"Vulkan: Failed to retrieve device extensions"};
                }
                
                bool has_extensions = std::ranges::all_of(required_device_extensions,
                    [&] (const auto& ext) {
                        bool has_ext = std::ranges::find_if(exts,
                            [&] (const auto& prop) {
                                return std::strcmp(prop.extensionName, ext) == 0;
                            }
                        ) != exts.end();
                        return has_ext;
                    }
                );
                return has_extensions;
            }
        );
        return std::ranges::all_of(checks, [] (const auto& c) { return c(); });
        } ();

        if (can_use) {
            m_devices[i] = {
                .device = device,
                .device_properties = props,
                .device_queue_families = queue_families,
            };
        }
    }
}

Instance::Instance(::Display* dpy, Window win):
    m_instance {
        [] {
            static constexpr std::array wsi_extensions = {VK_KHR_XLIB_SURFACE_EXTENSION_NAME};
            auto instance = createInstance(wsi_extensions);
            if (!instance) {
                throw std::runtime_error{"Failed to create Vulkan instance"};
            }
            return instance;
        } ()
    },
    m_surface {
        m_instance.get(), dpy, win
    }
{
    init();
}

Instance::Instance(xcb_connection_t* c, xcb_window_t win):
    m_instance {
        [] {
            static constexpr std::array wsi_extensions = {VK_KHR_XCB_SURFACE_EXTENSION_NAME};
            auto instance = createInstance(wsi_extensions);
            if (!instance) {
                throw std::runtime_error{"Failed to create Vulkan instance"};
            }
            return instance;
        } ()
    },
    m_surface {
        m_instance.get(), c, win
    }
{
    init();
}

size_t Instance::deviceCount() const {
    return m_devices.size();
}

size_t Instance::devices(std::span<DeviceID> out) const {
    auto count = std::min(out.size(), deviceCount());
    auto v = std::views::transform(m_devices, [] (const auto& kv) { return kv.first; });
    std::ranges::copy_n(v.begin(), count, out.begin());
    return count;
}

const char* Instance::deviceName(DeviceID dev) const {
    return getDevice(dev).device_properties.deviceName;
}

Context* Instance::createContext(DeviceID dev) {
    const auto& device = getDevice(dev);
    return new Context(
        m_instance.get(),
        device.device, device.device_queue_families,
        m_surface.get()
    );
}

bool Instance::DeviceIDIsValid(DeviceID dev) const {
    auto it = m_devices.find(dev);
    return it != m_devices.end();
}

PhysicalDevice& Instance::getDevice(DeviceID dev) {
    assert(DeviceIDIsValid(dev));
    return m_devices.find(dev)->second;
}

const PhysicalDevice& Instance::getDevice(DeviceID dev) const {
    assert(DeviceIDIsValid(dev));
    return m_devices.find(dev)->second;
}
}
