#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/System/Platform/Windows/WindowsSharedLibrary.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/System/Platform/Linux/LinuxSharedLibrary.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/System/Platform/Android/AndroidSharedLibrary.hxx"
#else
#error "Unsupported platform"
#endif
