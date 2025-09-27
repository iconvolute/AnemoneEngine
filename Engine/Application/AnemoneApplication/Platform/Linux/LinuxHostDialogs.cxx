#include "AnemoneApplication/Platform/Linux/LinuxHostDialogs.hxx"
#include "AnemoneApplication/Platform/Linux/LinuxHostWindow.hxx"

namespace Anemone
{
    DialogResult HostDialogs::ShowMessageDialog(
        HostWindow const* window,
        std::string_view message,
        std::string_view title,
        MessageDialogType type)
    {
        return ShowMessageDialog(
            window,
            message,
            title,
            type,
            MessageDialogImage::Information);
    }

    DialogResult HostDialogs::ShowMessageDialog(
        HostWindow const* window,
        std::string_view message,
        std::string_view title,
        MessageDialogType type,
        MessageDialogImage image)
    {
        AE_PANIC("Not implemented");
        (void)window;
        (void)message;
        (void)title;
        (void)type;
        (void)image;
        return {};
    }

    DialogResult HostDialogs::OpenFile(
        HostWindow const* window,
        std::string& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        AE_PANIC("Not implemented");
        (void)window;
        (void)result;
        (void)title;
        (void)filters;
        return {};
    }

    DialogResult HostDialogs::OpenFiles(
        HostWindow const* window,
        std::vector<std::string>& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        AE_PANIC("Not implemented");
        (void)window;
        (void)result;
        (void)title;
        (void)filters;
        return {};
    }

    DialogResult HostDialogs::SaveFile(
        HostWindow const* window,
        std::string& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        AE_PANIC("Not implemented");
        (void)window;
        (void)result;
        (void)title;
        (void)filters;
        return {};
    }
}
