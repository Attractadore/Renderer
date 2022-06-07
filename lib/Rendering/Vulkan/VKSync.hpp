#pragma once
#include "Common/Ref.hpp"
#include "VKRAII.hpp"

#include <chrono>

// Thanks XOrg
#undef Status

namespace R1::VK {
struct Fence: RefedBase<Fence> {
    Vk::Fence m_fence;

protected:
    Fence(Vk::Fence fence):
        m_fence{std::move(fence)} {}

public:
    struct Config {
        bool signaled = false;   
    };

    enum class Status {
        NotReady,
        Ready,
    };

    static constexpr auto WaitNoTimeout = std::chrono::nanoseconds{UINT64_MAX};
};
using FenceRef = Ref<Fence>;


struct Semaphore: RefedBase<Semaphore> {
    Vk::Semaphore m_semaphore;

protected:
    Semaphore(Vk::Semaphore sem):
        m_semaphore{std::move(sem)} {}

public:
    struct Config {};
};
using SemaphoreRef = Ref<Semaphore>;
}
