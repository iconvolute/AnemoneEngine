#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneApplication/HostDialogs.hxx"

#include <shobjidl.h>

namespace Anemone
{
    struct WindowsHostDialogs
    {
        static UINT CombineMessageDialogFlags(MessageDialogType type, MessageDialogImage image);

        static DialogResult ConvertDialogResult(int value);

        static HRESULT SetDialogFilters(IFileDialog& self, std::span<FileDialogFilter const> filters);

        static bool GetResult(IFileDialog& self, std::string& result);

        static bool GetResults(IFileOpenDialog& self, std::vector<std::string>& results);

        static DialogResult DialogResultFromHRESULT(HRESULT hr);
    };
}
