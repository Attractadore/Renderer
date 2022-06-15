#pragma once

namespace R1 {
inline GAL::SurfaceSizeCallback makeSizeCB(R1SurfaceSizeCallback size_cb, void* usrptr) {
    return [=] {
        int w = -1, h = -1;
        size_cb(usrptr, &w, &h);
        if (w < 0 or h < 0) {
            throw std::runtime_error{"Failed to get surface size"};
        }
        return std::tuple<unsigned, unsigned>{w, h};
    };
}
}
