#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/WindowsMemoryMappedFile.hxx"
#elif ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Android/AndroidMemoryMappedFile.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Linux/LinuxMemoryMappedFile.hxx"
#else
#error "Not Implemented"
#endif
