#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsFileHandle.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxFileHandle.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidFileHandle.hxx"
#else
#error "Unsupported platform"
#endif
