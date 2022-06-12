#pragma once
#include "Flags.hpp"

#include <string>
#include <vector>

namespace R1 {
enum class DeviceType;

enum class QueueCapability;
using QueueCapabilityFlags = Flags<QueueCapability>;

struct QueueFamily {
    enum ID: unsigned { Unknown = static_cast<unsigned>(-1) };
    ID id;
    QueueCapabilityFlags capabilities;
    unsigned count;
};

struct DeviceDescription {
    std::string name;
    DeviceType type;
    std::vector<QueueFamily> queue_families;
    bool wsi: 1;
};
}
