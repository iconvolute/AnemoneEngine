#ifndef ANEMONE_PLATFORM_TYPES_HXX
#define ANEMONE_PLATFORM_TYPES_HXX
#pragma once

#if ANEMONE_PLATFORM_WINDOWS
#include "AnemoneRuntime/Platform/Windows/Types.hxx"
#elif ANEMONE_PLATFORM_ANDROID || ANEMONE_PLATFORM_LINUX
#include "AnemoneRuntime/Platform/Unix/Types.hxx"
#else
#error "Not implemented"
#endif

#endif // ANEMONE_PLATFORM_TYPES_HXX
