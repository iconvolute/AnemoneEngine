#pragma once
#include "AnemoneInterop/Headers.hxx"

namespace Anemone
{
    class Module_Application
    {
    public:
        APPLICATION_API static void Initialize();
        APPLICATION_API static void Finalize();
    };
}
