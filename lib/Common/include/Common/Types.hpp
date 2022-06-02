#pragma once
#include "R1DeviceID.hpp"
#include "R1PresentModes.hpp"

#include <functional>
#include <string>

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
    QueueCapabilities capabilities;
    unsigned count;
};

struct DeviceDescription {
    std::string name = "Unknown device";
    DeviceType type = DeviceType(0);
    std::vector<QueueFamily> queue_families;
    bool wsi: 1 = false;
};

struct ContextConfig {};
}
