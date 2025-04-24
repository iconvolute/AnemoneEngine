#include "AnemoneRuntime/System/Platform/Linux/LinuxDialogs.hxx"
#include "AnemoneRuntime/System/Dialogs.hxx"

namespace Anemone::Internal
{
    UninitializedObject<LinuxDialogsStatics> GDialogsStatics;
}

namespace Anemone
{
    DialogResult Dialogs::ShowMessageDialog(
        Window const* window,
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
        Window const* window,
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
        Window const* window,
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
        Window const* window,
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
        Window const* window,
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
