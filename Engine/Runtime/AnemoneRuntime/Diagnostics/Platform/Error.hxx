#pragma once
#include "AnemoneRuntime/Diagnostics/Platform/Common/CommonError.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsError.hxx"
#elif ANEMONE_PLATFORM_LINUX
#else
#error "Not supported"
#endif
