#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"
#include "AnemoneRuntime/Platform/Types.hxx"

#include <string_view>
#include <string>

namespace Anemone::System
{
    class RUNTIME_API NamedMutex final
    {
    private:
        Platform::NativeNamedMutex m_native;
        std::string m_name{};

    public:
        explicit NamedMutex(std::string_view name);
        NamedMutex(NamedMutex const&) = delete;
        NamedMutex(NamedMutex&& other) noexcept;
        NamedMutex& operator=(NamedMutex const&) = delete;
        NamedMutex& operator=(NamedMutex&& other) noexcept;
        ~NamedMutex();

        void Lock();
        bool TryLock();
        void Unlock();
    };
}
