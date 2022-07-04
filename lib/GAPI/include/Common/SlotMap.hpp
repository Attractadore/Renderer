#pragma once
#include "Attractadore/SlotMap.hpp"

namespace R1::GAPI {
template<typename T>
using SlotMap = Attractadore::SlotMap<T, 31, 1>;
}
