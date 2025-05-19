#pragma once
#include "AnemoneRuntime/Diagnostics/Platform/Common/Error.hxx"

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Diagnostics/Platform/Windows/Error.hxx"
#elif ANEMONE_PLATFORM_LINUX
#else
#error "Not supported"
#endif
