#pragma once
#include "R1DeviceID.hpp"
#include "R1PresentModes.hpp"

#include <functional>
#include <string>
#include <span>

namespace R1 {
using SizeCallback = std::function<std::tuple<unsigned, unsigned>()>;

struct InstanceDescription {
    bool wsi: 1 = false;
    bool xlib: 1 = false;
    bool xcb: 1 = false;
};

struct InstanceConfig {};

enum class DeviceType;

struct QueueCapabilities {
    bool graphics: 1 = false;
    bool compute: 1 = false;
    bool transfer: 1 = false;
};

struct QueueFamily {
    enum ID: unsigned;
    ID id;
    unsigned count;
    QueueCapabilities capabilities;
};

struct DeviceDescription {
    std::string name = "Unknown device";
    DeviceType type = DeviceType(0);
    std::vector<QueueFamily> queue_families;
    bool wsi: 1 = false;
};

struct QueueConfig {
    QueueFamily::ID id;
    unsigned count;
};

struct ContextConfig {
    std::span<const QueueConfig> queue_config;
    bool wsi: 1 = false;
};

struct ShaderModuleConfig {
    std::span<const std::byte> code;
};

struct PipelineLayoutConfig {};
}
