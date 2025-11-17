#pragma once
#include "AnemoneInterop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneInterop/Windows/Headers.hxx"
#endif

#if ANEMONE_PLATFORM_LINUX
#include <X11/Xlib-xcb.h>
#endif
#undef None
#undef Success


#define VK_NO_PROTOTYPES
#define VK_ENABLE_BETA_EXTENSIONS

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


#if !ANEMONE_BUILD_SHIPPING
#define ANEMONE_VULKAN_VALIDATION true
#endif

#ifndef ANEMONE_VULKAN_VALIDATION
#define ANEMONE_VULKAN_VALIDATION false
#endif

#ifndef ANEMONE_VULKAN_FEATURE_API_DUMP
#define ANEMONE_VULKAN_FEATURE_API_DUMP false
#endif

#ifndef ANEMONE_VULKAN_FEATURE_GFXRECONSTRUCT
#define ANEMONE_VULKAN_FEATURE_GFXRECONSTRUCT false
#endif 

#ifndef ANEMONE_VULKAN_FEATURE_VKTRACE
#define ANEMONE_VULKAN_FEATURE_VKTRACE false
#endif

#ifndef ANEMONE_VULKAN_FEATURE_CRASH_DIAGNOSTIC
#define ANEMONE_VULKAN_FEATURE_CRASH_DIAGNOSTIC false
#endif

#if ANEMONE_VULKAN_VALIDATION
#include <vulkan/vk_enum_string_helper.h>
#endif
