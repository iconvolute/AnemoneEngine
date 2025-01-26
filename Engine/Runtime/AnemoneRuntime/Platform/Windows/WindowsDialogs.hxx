#pragma once
#include "AnemoneRuntime/Platform/Base/BaseDialogs.hxx"

#include <string>
#include <span>
#include <vector>

namespace Anemone
{
    struct WindowsDialogs final
    {
        RUNTIME_API static DialogResult ShowMessageDialog(
            Window const* window,
            std::string_view message,
            std::string_view title,
            MessageDialogType type);

        RUNTIME_API static DialogResult ShowMessageDialog(
            Window const* window,
            std::string_view message,
            std::string_view title,
            MessageDialogType type,
            MessageDialogImage image);

        RUNTIME_API static DialogResult OpenFile(
            Window const* window,
            std::string& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);

        RUNTIME_API static DialogResult OpenFiles(
            Window const* window,
            std::vector<std::string>& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);

        RUNTIME_API static DialogResult SaveFile(
            Window const* window,
            std::string& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);
    };

    using Dialogs = WindowsDialogs;
}
