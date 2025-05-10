#include "AnemoneRuntime/Input/Platform/Windows/WindowsInput.hxx"
#include "AnemoneRuntime/Input/Platform/Windows/GameInput.hxx"
#include "AnemoneRuntime/Input/Platform/Windows/XInput.hxx"

namespace Anemone::Input
{
    extern void PlatformInitialize()
    {
        WindowsInput::Initialize();
        GameInput::Initialize();
        XInput::Initialize();
    }

    extern void PlatformFinalize()
    {
        XInput::Finalize();
        GameInput::Finalize();
        WindowsInput::Finalize();
    }
}
