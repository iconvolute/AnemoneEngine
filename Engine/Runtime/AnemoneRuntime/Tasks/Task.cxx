#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Diagnostics/Trace.hxx"

#include <utility>
#include <type_traits>

#define ENABLE_LOGGING false

namespace Anemone::Tasks
{
    std::atomic<size_t> Task::s_TotalAllocations{};

    void* Task::operator new(std::size_t size, std::align_val_t al)
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            ++s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator new(size={}, alignment={}, total={})", size, static_cast<std::size_t>(al), total);
#endif
        return ::operator new(size, al);
    }

    void* Task::operator new(std::size_t size)
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            ++s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator new(size={}, total={})", size, total);
#endif
        return ::operator new(size);
    }

    void Task::operator delete(void* ptr, std::align_val_t al) noexcept
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            --s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator delete(ptr={}, alignment={}, total={})", fmt::ptr(ptr), static_cast<std::size_t>(al), total);
#endif
        ::operator delete(ptr, al);
    }

    void Task::operator delete(void* ptr) noexcept
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            --s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Task::operator delete(ptr={}, total={})", fmt::ptr(ptr), total);
#endif
        ::operator delete(ptr);
    }
}

#undef ENABLE_LOGGING
