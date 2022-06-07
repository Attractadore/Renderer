#pragma once

namespace R1 {
struct FenceConfig {
    bool signaled: 1;
};

enum class FenceStatus {
    Ready,
    NotReady,
};
}
