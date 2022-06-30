#pragma once
#include "Context.hpp"
#include "R1Types.h"
#include "Swapchain.hpp"

namespace R1 {
using Scene = R1Scene;

struct ScenePresentInfo {
    GAL::Semaphore          semaphore;
    GAL::SemaphorePayload   wait_value;
    GAL::SemaphorePayload   signal_value;
};
}

class R1Scene {
protected:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

public:
    R1Scene(R1::Context& ctx);
    R1Scene(const R1Scene&) = delete;
    R1Scene(R1Scene&& other) = default;
    R1Scene& operator=(const R1Scene&) = delete;
    R1Scene& operator=(R1Scene&&) = default;
    ~R1Scene();

    void ConfigOutputImages(
        unsigned width, unsigned height, unsigned count,
        R1::GAL::ImageUsageFlags image_usage_flags
    );
    std::tuple<unsigned, unsigned> GetOutputImageSize() const noexcept;
    R1::GAL::Format GetOutputImageFormat() const noexcept;
    R1::GAL::ImageLayout GetOutputImageStartLayout() const noexcept;
    R1::GAL::ImageLayout GetOutputImageEndLayout() const noexcept;
    size_t GetOutputImageCount() const noexcept;
    R1::GAL::Image GetOutputImage(size_t idx) const noexcept;
    size_t GetCurrentOutputImage() const noexcept;

    R1::ScenePresentInfo Draw();
};
