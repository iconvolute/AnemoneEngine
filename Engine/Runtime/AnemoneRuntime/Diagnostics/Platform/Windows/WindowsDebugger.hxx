#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"

namespace Anemone::Internal::Windows
{
    void HandleCrash(PEXCEPTION_POINTERS pExceptionPointers);
}
