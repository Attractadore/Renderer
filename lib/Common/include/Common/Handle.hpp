#pragma once
#include <memory>

namespace R1 {
template<typename H, typename D>
using Handle = std::unique_ptr<std::remove_pointer_t<H>, D>;
}
