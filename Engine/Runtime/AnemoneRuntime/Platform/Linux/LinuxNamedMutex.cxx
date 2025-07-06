#include "AnemoneRuntime/Platform/NamedMutex.hxx"
#include "AnemoneRuntime/Platform/Linux/LinuxNamedMutex.hxx"
#include "AnemoneRuntime/Interop/Linux/SafeHandle.hxx"
#include "AnemoneRuntime/Diagnostics/Debug.hxx"

#include <fcntl.h>
#include <sys/stat.h>
#include <fmt/format.h>
#include <utility>

namespace Anemone
{
    std::expected<LinuxNamedMutex, Status> LinuxNamedMutex::Create(std::string_view name)
    {
        std::string formatted = fmt::format("/anemone-engine-named-mutex-{}", name);

        Interop::Linux::SafeNamedSemaphoreHandle handle{sem_open(formatted.c_str(), O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1)};

        if (handle)
        {
            return LinuxNamedMutex{std::move(handle)};
        }

        return std::unexpected(Status::InvalidOperation);
    }

    void LinuxNamedMutex::Lock()
    {
        AE_ASSERT(this->_handle);

        auto& error = errno;

        int rc;

        // TODO: Do some helper macro / function to handle this pattern
        do
        {
            rc = sem_wait(this->_handle.Get());
        } while (rc && (error == EINTR));

        if (rc)
        {
            AE_PANIC("Failed to lock named mutex: {}", error);
        }
    }

    bool LinuxNamedMutex::TryLock()
    {
        AE_ASSERT(this->_handle);

        return sem_trywait(this->_handle.Get()) == 0;
    }

    void LinuxNamedMutex::Unlock()
    {
        AE_ASSERT(this->_handle);

        if (sem_post(this->_handle.Get()))
        {
            AE_PANIC("Failed to unlock named mutex: {}", errno);
        }
    }
}
