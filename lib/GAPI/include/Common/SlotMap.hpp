#pragma once
#include "Attractadore/SlotMap.hpp"

#include <vector>

namespace R1 {
namespace Detail {
template<typename T>
using StdVector = std::vector<T>;
}

template<typename T>
using SlotMap = Attractadore::SlotMap<T, unsigned, Detail::StdVector>;
}
