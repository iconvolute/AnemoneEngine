#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/System/Platform/Windows/WindowsProcess.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/System/Platform/Linux/LinuxProcess.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/System/Platform/Android/AndroidProcess.hxx"
#else
#error "Unsupported platform"
#endif
