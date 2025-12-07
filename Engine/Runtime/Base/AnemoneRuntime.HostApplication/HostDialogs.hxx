#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"

#include <string_view>
#include <string>
#include <span>
#include <vector>

namespace Anemone
{
    class HostWindow;

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

    struct HostDialogs final
    {
        ANEMONE_RUNTIME_BASE_API static DialogResult ShowMessageDialog(
            HostWindow const* window,
            std::string_view message,
            std::string_view title,
            MessageDialogType type);

        ANEMONE_RUNTIME_BASE_API static DialogResult ShowMessageDialog(
            HostWindow const* window,
            std::string_view message,
            std::string_view title,
            MessageDialogType type,
            MessageDialogImage image);

        ANEMONE_RUNTIME_BASE_API static DialogResult OpenFile(
            HostWindow const* window,
            std::string& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);

        ANEMONE_RUNTIME_BASE_API static DialogResult OpenFiles(
            HostWindow const* window,
            std::vector<std::string>& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);

        ANEMONE_RUNTIME_BASE_API static DialogResult SaveFile(
            HostWindow const* window,
            std::string& result,
            std::string_view title,
            std::span<FileDialogFilter const> filters);
    };
}
