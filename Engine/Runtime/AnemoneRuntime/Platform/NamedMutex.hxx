#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsNamedMutex.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxNamedMutex.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidNamedMutex.hxx"
#else
#error "Unsupported platform"
#endif
