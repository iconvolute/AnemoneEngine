#pragma once
#include "AnemoneInterop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneInterop/Windows/Headers.hxx"
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#if ANEMONE_PLATFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#endif

#include <volk.h>
#include <vk_mem_alloc.h>


#if ANEMONE_BUILD_SHIPPING
#define ANEMONE_VULKAN_VALIDATION false
#else
#define ANEMONE_VULKAN_VALIDATION true
#endif
