#include "AnemoneRuntime/ErrorCode.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsHeaders.hxx"

#include <cerrno>

namespace Anemone::System::Private
{
    ErrorCode ErrorCodeFromErrno(int error)
    {
        (void)error;
        return ErrorCode::Unknown;
    }
}
