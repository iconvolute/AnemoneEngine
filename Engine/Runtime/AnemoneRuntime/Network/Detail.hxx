#pragma once
#include "AnemoneRuntime/Platform/Base/BaseHeaders.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Unix/UnixInterop.hxx"
#else
#error "Not implemented"
#endif

#include <utility>
