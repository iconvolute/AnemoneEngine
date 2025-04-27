#pragma once
#include "AnemoneRuntime/Interop/Linux/Headers.hxx"

namespace Anemone::Interop::Linux
{
    inline pid_t GetThreadId()
    {
        return static_cast<pid_t>(syscall(SYS_gettid));
    }

    inline pid_t GetProcessId()
    {
        return static_cast<pid_t>(syscall(SYS_getpid));
    }

    inline bool IsProcessAlive(pid_t pid)
    {
        return (kill(pid, 0) == 0) or (errno == ESRCH);
    }

    inline bool WaitForProcess(pid_t pid)
    {
        int status;
        return waitpid(pid, &status, 0) >= 0;
    }

    inline pid_t WaitForProcess(pid_t pid, int& status, int flags)
    {
        pid_t rc;

        do
        {
            rc = waitpid(pid, &status, flags);
        } while ((rc < 0) and (errno == EINTR));

        return rc;
    }

    inline bool TerminateProcess(pid_t pid)
    {
        bool result = true;
        if (kill(pid, SIGKILL))
        {
            result = false;
        }

        // Wait without WNOHANG to avoid zombie processes.
        int status;
        waitpid(pid, &status, 0);

        return result;
    }
}
