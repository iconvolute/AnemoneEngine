#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

namespace Anemone::Interop::Windows
{
    // HARD REQUIREMENT: Buffer must be zero terminated!
    anemone_forceinline void OutputDebugStringLengthA(const char* s, size_t n)
    {
        ULONG_PTR const args[2] = {n, reinterpret_cast<ULONG_PTR>(s)};
        RaiseException(DBG_PRINTEXCEPTION_C, 0, 2, args);
    }
}
