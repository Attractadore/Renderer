#pragma once
#include "Context.hpp"
#include "Instance.hpp"
#include "Scene.hpp"
#include "Swapchain.hpp"
#include "R1Types.h"

struct R1Instance: R1::Instance {
    using Instance::Instance;
};
struct R1Context: R1::Context {
    using Context::Context;
};
struct R1Surface: R1::Surface {
    using Surface::Surface;
};
struct R1Swapchain: R1::Swapchain {
    using Swapchain::Swapchain;
};
struct R1Scene: R1::Scene {
    using Scene::Scene;
};
