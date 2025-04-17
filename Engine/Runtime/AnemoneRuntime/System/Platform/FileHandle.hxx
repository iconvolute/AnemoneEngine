#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/System/Platform/Windows/WindowsFileHandle.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/System/Platform/Linux/LinuxFileHandle.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/System/Platform/Android/AndroidFileHandle.hxx"
#else
#error "Unsupported platform"
#endif
