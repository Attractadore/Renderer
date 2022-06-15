#pragma once
#include "Common/Flags.hpp"

#include <string>
#include <vector>

namespace R1::GAL {
enum QueueFamilyID: unsigned { Unknown = static_cast<unsigned>(-1) };

namespace Detail {
template<typename Traits>
struct QueueFamilyBase {
    using ID = QueueFamilyID;
    ID                              id;
    Traits::QueueCapabilityFlags    capabilities;
    unsigned                        count;
};

template<typename Traits>
struct DeviceDescriptionBase {
    std::string                             name;
    Traits::DeviceType                      type;
    std::vector<QueueFamilyBase<Traits>>    queue_families;
    bool                                    wsi: 1;
};
}
}
