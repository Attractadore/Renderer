#pragma once
#include <string>
#include <vector>

namespace R1 {
enum class DeviceType;

struct QueueCapabilities {
    bool graphics: 1;
    bool compute: 1;
    bool transfer: 1;

    bool operator==(const QueueCapabilities&) const = default;
};

struct QueueFamily {
    enum ID: unsigned { Unknown = static_cast<unsigned>(-1) };
    ID id;
    QueueCapabilities capabilities;
    unsigned count;

    bool operator==(const QueueFamily&) const = default;
};

struct DeviceDescription {
    std::string name;
    DeviceType type;
    std::vector<QueueFamily> queue_families;
    bool wsi: 1;
};
}
