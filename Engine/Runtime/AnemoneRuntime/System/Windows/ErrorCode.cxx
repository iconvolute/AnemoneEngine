#include "AnemoneRuntime/ErrorCode.hxx"

#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"

namespace Anemone::System::Private
{
    ErrorCode ErrorCodeFromWin32Error(uint32_t error)
    {
        (void)error;
        return ErrorCode::Unknown;
    }
}
