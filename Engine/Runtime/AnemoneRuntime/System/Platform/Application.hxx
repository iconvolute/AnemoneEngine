#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/System/Platform/Windows/WindowsApplication.hxx"
#elif ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/System/Platform/Linux/LinuxApplication.hxx"
#else
#error "Not implemented"
#endif
