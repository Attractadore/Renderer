#pragma once
#include "GAL/Sync.hpp"

namespace R1::GAL {
VkSemaphore CreateBinarySemaphore(Context ctx);
VkFence CreateFence(Context ctx, bool signaled = false);
}
