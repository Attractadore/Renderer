#pragma once
#include <memory>

namespace R1 {
namespace Detail {
template<typename H, typename D>
using HandleBase = std::unique_ptr<std::remove_pointer_t<H>, D>;
}

template<typename H, typename D>
struct Handle: Detail::HandleBase<H, D> {
    using Detail::HandleBase<H, D>::HandleBase;
};

template<typename H, typename D>
Handle(H, D&&) -> Handle<H, std::remove_cvref_t<D>>;
}
