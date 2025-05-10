#pragma once
#include "AnemoneRuntime/Interop/Headers.hxx"

#include <string_view>
#include <string>
#include <span>
#include <vector>

namespace Anemone
{
    class IWindow;

    enum class MessageDialogType
    {
        Ok,
        OkCancel,
        YesNo,
        YesNoCancel,
        RetryCancel,
        AbortRetryIgnore,
    };

    enum class MessageDialogImage
    {
        None,
        Error,
        Warning,
        Information,
        Question,
    };

    enum class [[nodiscard]] DialogResult
    {
        Cancel,
        Ok,
        Yes,
        No,
        Abort,
        Retry,
        Ignore,
    };

    struct FileDialogFilter final
    {
        std::string_view Display;
        std::string_view Filter;
    };
}

namespace Anemone
{
    struct Dialogs final
    {
        Dialogs() = delete;

        RUNTIME_API static DialogResult ShowMessageDialog(
            IWindow const* window,
            std::string_view message,
            std::string_view title,
            MessageDialogType type);

        RUNTIME_API static DialogResult ShowMessageDialog(
            IWindow const* window,
            std::string_view message,
            std::string_view title,
            MessageDialogType type,
            MessageDialogImage image);

        RUNTIME_API static DialogResult OpenFile(
            IWindow const* window,
            std::string& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);

        RUNTIME_API static DialogResult OpenFiles(
            IWindow const* window,
            std::vector<std::string>& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);

        RUNTIME_API static DialogResult SaveFile(
            IWindow const* window,
            std::string& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);
    };
}
