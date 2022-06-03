#pragma once

namespace R1 {
struct InstanceDescription {
    bool wsi: 1 = false;
    bool xlib: 1 = false;
    bool xcb: 1 = false;
};

struct InstanceConfig {};
}
