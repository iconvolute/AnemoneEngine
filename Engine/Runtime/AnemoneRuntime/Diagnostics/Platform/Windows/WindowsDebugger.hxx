#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

namespace Anemone::Diagnostics
{
    void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers);
}
