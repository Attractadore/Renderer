#pragma once

#if R1_USE_VULKAN

#include "Vulkan/VKContext.hpp"
#include "Vulkan/VKInstance.hpp"
#include "Vulkan/VKSwapchain.hpp"
namespace R1 {
namespace GAPI = VK;
}

#elif R1_USE_OPENGL

#include "OpenGL/API.hpp"
namespace R1 {
namespace GAPI = GL;
}

#endif
