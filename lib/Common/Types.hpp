#include "R1DeviceID.hpp"
#include "R1PresentModes.hpp"

#include <functional>

namespace R1 {
using SizeCallback = std::function<std::tuple<unsigned, unsigned>()>;
}
