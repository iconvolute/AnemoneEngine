#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/System/Platform/Windows/WindowsClipboard.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/System/Platform/Linux/LinuxClipboard.hxx"
#else
#error "Not implemented"
#endif
