#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#if ANEMONE_PLATFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif

#include <volk.h>
