#pragma once
#include "AnemoneRuntime/Interop/SafeHandle.hxx"

#include <sys/eventfd.h>

namespace Anemone::Interop::Linux
{
    struct SafeEventFdHandleTraits final
    {
        static int Invalid() noexcept
        {
            return -1;
        }

        static bool IsValid(int value)
        {
            return value >= 0;
        }

        static bool Reset(int value)
        {
            return close(value) == 0;
        }
    };

    using EventFdHandle = SafeHandleT<int, SafeEventFdHandleTraits>;

    inline EventFdHandle CreateEventFd()
    {
        return EventFdHandle{eventfd(0, 0)};
    }

    inline void SetEventFd(EventFdHandle const& handle)
    {
        eventfd_t value{1};
        eventfd_write(handle.Get(), value);
    }

    inline bool WaitForEventFd(EventFdHandle const& handle)
    {
        eventfd_t value{};
        return eventfd_read(handle.Get(), &value) == 0;
    }

    inline bool WaitForEventFd(EventFdHandle const& handle, uint32_t milliseconds)
    {
        pollfd fds = {E, POLLIN, 0};

        if (poll(&fds, 1, milliseconds))
        {
            if (fds.revents & POLLIN)
            {
                eventfd_t value{};
                return eventfd_read(handle.Get(), &value) == 0;
            }
        }

        return false;
    }

    inline void ResetEventFd(EventFdHandle const& handle)
    {
        eventfd_t value{};
        eventfd_read(handle.Get(), &value);
    }
}
