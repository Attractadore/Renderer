#pragma once
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    R1_PRESENT_MODE_NO_VSYNC = 0,
    R1_PRESENT_MODE_VSYNC,
    R1_PRESENT_MODE_ADAPTIVE_VSYNC,
    R1_PRESENT_MODE_MULTIBUFFERED_VSYNC
} R1PresentMode;

#ifdef __cplusplus
}
#endif
