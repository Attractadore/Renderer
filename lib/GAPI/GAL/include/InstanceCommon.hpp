#pragma once

namespace R1::GAL {
struct InstanceDescription {
    bool wsi: 1;
    bool xlib: 1;
    bool xcb: 1;
};

struct InstanceConfig {};
}
