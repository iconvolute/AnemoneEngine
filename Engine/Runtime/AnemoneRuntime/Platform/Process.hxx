#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsProcess.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxProcess.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidProcess.hxx"
#else
#error "Unsupported platform"
#endif
