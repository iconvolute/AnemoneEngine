#pragma once
#include "AnemoneRuntime/Platform/Windows/WindowsInterop.hxx"
#include "AnemoneRuntime/Platform/Windows/WindowsMemoryMappedFile.hxx"

#include <format>

namespace Anemone::Interop
{
    struct CrashDetails
    {
        CHAR BuildId[64];
        DWORD ProcessId;
        DWORD ThreadId;
        EXCEPTION_RECORD ExceptionRecord;
        CONTEXT Context;
        PVOID XmmRegisters;
        PVOID YmmRegisters;
        PVOID ZmmRegisters;
        PVOID ZmmhRegisters;
        DWORD XmmRegistersLength;
        DWORD YmmRegistersLength;
        DWORD ZmmRegistersLength;
        DWORD ZmmhRegistersLength;
    };

    struct Mapping final
    {
    private:
        Win32SafeMemoryMappedFileHandle _handle;
        Win32SafeMemoryMappedViewHandle _view;

    private:
        explicit Mapping(
            Win32SafeMemoryMappedFileHandle handle,
            Win32SafeMemoryMappedViewHandle view)
            : _handle{std::move(handle)}
            , _view{std::move(view)}
        {
        }

    public:
        static std::optional<Mapping> Create(DWORD processId, DWORD threadId)
        {
            wchar_t name[MAX_PATH + 1];
            auto nameFormatResult = std::format_to_n(
                name,
                MAX_PATH,
                L"AnemoneCrashDetails_pid{}_tid{}",
                processId,
                threadId);
            (*nameFormatResult.out) = L'\0';

            Win32SafeMemoryMappedFileHandle handle = win32_OpenMemoryMappedFile(
                Win32SafeFileHandle{},
                name,
                sizeof(CrashDetails),
                MemoryMappedFileAccess::ReadWrite);

            if (handle)
            {
                Win32SafeMemoryMappedViewHandle view = win32_CreateMemoryMappedView(
                    handle,
                    MemoryMappedFileAccess::ReadWrite);

                if (view)
                {
                    return Mapping{std::move(handle), std::move(view)};
                }
            }

            return std::nullopt;
        }

    public:
        void Write(CrashDetails const& crashDetails)
        {
            if (this->_view)
            {
                CopyMemory(this->_view.GetData(), &crashDetails, sizeof(CrashDetails));
            }

            win32_FlushMemoryMappedView(this->_view);
        }
    };
}
