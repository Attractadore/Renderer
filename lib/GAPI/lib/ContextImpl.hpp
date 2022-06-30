#pragma once
#include "Context.hpp"

namespace R1::GAPI {
struct ContextConfig {
    std::vector<GAL::QueueConfig>   queue_configs;
    GAL::ContextConfig              config;
};

ContextConfig ConfigureContext(Device& device);
}
