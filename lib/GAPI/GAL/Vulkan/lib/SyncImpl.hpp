#pragma once
#include "GAL/Sync.hpp"

namespace R1::GAL {
VkSemaphore CreateBinarySemaphore(VkDevice dev);
VkFence CreateFence(VkDevice dev, bool signaled = false);
}
