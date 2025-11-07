#pragma once
#include "AnemoneInterop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneInterop/Windows/Headers.hxx"
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_beta.h>

#if ANEMONE_PLATFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif

#if ANEMONE_PLATFORM_LINUX
#include <vulkan/vulkan_xcb.h>
#include <vulkan/vulkan_wayland.h>
#endif

#include <volk.h>
#include <vk_mem_alloc.h>


#if ANEMONE_BUILD_SHIPPING
#define ANEMONE_VULKAN_VALIDATION false
#else
#define ANEMONE_VULKAN_VALIDATION true
#endif
