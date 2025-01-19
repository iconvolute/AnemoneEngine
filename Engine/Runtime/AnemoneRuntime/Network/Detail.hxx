#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#if ANEMONE_PLATFORM_WINDOWS

#include "AnemoneRuntime/Platform/Windows/Functions.hxx"

#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
#include "AnemoneRuntime/Platform/Posix/Functions.hxx"
#else
#error "Not implemented"
#endif

#include <utility>
