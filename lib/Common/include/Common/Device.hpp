#pragma once
#include <string>
#include <vector>

namespace R1 {
enum class DeviceType;

struct QueueCapabilities {
    bool graphics: 1;
    bool compute: 1;
    bool transfer: 1;
};

struct QueueFamily {
    enum ID: unsigned;
    ID id;
    QueueCapabilities capabilities;
    unsigned count;
};

struct DeviceDescription {
    std::string name;
    DeviceType type;
    std::vector<QueueFamily> queue_families;
    bool wsi: 1;
};
}
