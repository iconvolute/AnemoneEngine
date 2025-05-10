#include "AnemoneRuntime/Interop/Headers.hxx"

namespace Anemone::Input
{
    extern void PlatformInitialize();
    extern void PlatformFinalize();

    extern void Initialize()
    {
        PlatformInitialize();
    }

    extern void Finalize()
    {
        PlatformFinalize();
    }
}
