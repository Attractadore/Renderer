#pragma once
#include "DeviceDefault.hpp"

namespace R1::GAL {
using QueueCapability = Detail::DefaultQueueCapability;
using DeviceType = Detail::DefaultDeviceType;

struct OpenGLInstance; 
using Instance = OpenGLInstance*;
struct OpenGLAdapter;
using Device = OpenGLAdapter*;
};
