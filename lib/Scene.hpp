#pragma once
#include <memory>

namespace R1 {
class Context;
class Swapchain;

class Scene {
    std::reference_wrapper<Swapchain>   m_swapchain;
    struct Impl;
    std::unique_ptr<Impl>               pimpl;

public:
    Scene(Context& ctx, Swapchain& swapchain);
    Scene(const Scene&) = delete;
    Scene(Scene&& other) = default;
    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&) = default;
    ~Scene();

    void Draw();
};
}
