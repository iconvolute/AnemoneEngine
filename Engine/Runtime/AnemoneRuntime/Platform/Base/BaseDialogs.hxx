#pragma once
#include "AnemoneRuntime/Platform/Detect.hxx"

#include <string_view>

namespace Anemone
{
    class Window;

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
