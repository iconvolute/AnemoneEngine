#include "AnemoneRuntime/ErrorCode.hxx"

#include <cerrno>

namespace Anemone::System::Private
{
    ErrorCode ErrorCodeFromErrno(int error)
    {
        (void)error;
        return ErrorCode::Unknown;
    }
}
