#include "AnemoneRuntime/App/Dialogs.hxx"

namespace Anemone
{
    DialogResult Dialogs::ShowMessageDialog(
        IWindow const* window,
        std::string_view message,
        std::string_view title,
        MessageDialogType type)
    {
        (void)window;
        (void)message;
        (void)title;
        (void)type;
        return DialogResult::Cancel;
    }

    DialogResult Dialogs::ShowMessageDialog(
        IWindow const* window,
        std::string_view message,
        std::string_view title,
        MessageDialogType type,
        MessageDialogImage image)
    {
        (void)window;
        (void)message;
        (void)title;
        (void)type;
        (void)image;
        return DialogResult::Cancel;
    }

    DialogResult Dialogs::OpenFile(
        IWindow const* window,
        std::string& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        (void)window;
        (void)result;
        (void)title;
        (void)filters;
        return DialogResult::Cancel;
    }

    DialogResult Dialogs::OpenFiles(
        IWindow const* window,
        std::vector<std::string>& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        (void)window;
        (void)result;
        (void)title;
        (void)filters;
        return DialogResult::Cancel;
    }

    DialogResult Dialogs::SaveFile(
        IWindow const* window,
        std::string& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        (void)window;
        (void)result;
        (void)title;
        (void)filters;
        return DialogResult::Cancel;
    }
}
