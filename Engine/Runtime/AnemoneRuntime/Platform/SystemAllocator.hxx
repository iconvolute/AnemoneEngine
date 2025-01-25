#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsSystemAllocator.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxSystemAllocator.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidSystemAllocator.hxx"
#else
#error "Unsupported platform"
#endif
