#include "AnemoneRuntime/Tasks/Awaiter.hxx"
#include "AnemoneRuntime/Tasks/Task.hxx"
#include "AnemoneRuntime/Diagnostic/Trace.hxx"

#include <utility>
#include <type_traits>

#define ENABLE_LOGGING false

namespace Anemone::Tasks
{
    std::atomic<size_t> Awaiter::s_TotalAllocations{};

    void* Awaiter::operator new(std::size_t size, std::align_val_t al)
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            ++s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Awaiter::operator new(size={}, alignment={}, total={})", size, static_cast<std::size_t>(al), total);
#endif
        return ::operator new(size, al);
    }

    void* Awaiter::operator new(std::size_t size)
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            ++s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Awaiter::operator new(size={})", size, total);
#endif
        return ::operator new(size);
    }

    void Awaiter::operator delete(void* ptr, std::align_val_t al) noexcept
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            --s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Awaiter::operator delete(ptr={}, alignment={}, total={})", fmt::ptr(ptr), static_cast<std::size_t>(al), total);
#endif
        ::operator delete(ptr, al);
    }

    void Awaiter::operator delete(void* ptr) noexcept
    {
#if ENABLE_LOGGING
        auto const total =
#endif
            --s_TotalAllocations;
#if ENABLE_LOGGING
        AE_TRACE(Warning, "Awaiter::operator delete(ptr={}, total={})", fmt::ptr(ptr), total);
#endif
        ::operator delete(ptr);
    }
}

namespace Anemone::Tasks
{
    void Awaiter::AddWaitingTask(Task& task)
    {
        // TODO: How to verify if task is not on any list?
        Threading::UniqueLock lock{this->m_Lock};
        this->m_WaitList.PushBack(&task);
    }
}

#undef ENABLE_LOGGING
