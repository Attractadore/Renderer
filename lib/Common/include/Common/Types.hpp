#pragma once
#include "R1DeviceID.hpp"
#include "R1PresentModes.hpp"
#include "Context.hpp"
#include "Instance.hpp"
#include "Pipeline.hpp"

#include <functional>

namespace R1 {
using SizeCallback = std::function<std::tuple<unsigned, unsigned>()>;
}
