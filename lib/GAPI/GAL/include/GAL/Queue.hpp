#pragma once
#include "Instance.hpp"
#include "QueueCommon.hpp"

namespace R1::GAL {
using SemaphoreSubmitConfig = Detail::SemaphoreSubmitConfigBase<Detail::Traits>;
using QueueSubmitConfig = Detail::QueueSubmitConfigBase<Detail::Traits>;

Queue GetQueue(Context ctx, QueueFamily::ID family, unsigned idx);

void QueueSubmit(
    Queue queue, std::span<const QueueSubmitConfig> configs
);

void QueueWaitIdle(Queue queue);
}
