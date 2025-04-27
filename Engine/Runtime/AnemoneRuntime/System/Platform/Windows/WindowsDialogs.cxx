#include "AnemoneRuntime/System/Platform/Windows/WindowsDialogs.hxx"
#include "AnemoneRuntime/System/Platform/Windows/WindowsWindow.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"
#include "AnemoneRuntime/System/Dialogs.hxx"

#include <wrl/client.h>
#include <shobjidl.h>
#include <CommCtrl.h>

namespace Anemone::Internal
{
    UninitializedObject<WindowsDialogsStatics> GDialogsStatics;

    constexpr UINT CombineMessageDialogFlags(
        MessageDialogType type,
        MessageDialogImage image) noexcept
    {
        UINT result{};

        switch (type)
        {
        case MessageDialogType::Ok:
            result |= MB_OK;
            break;

        case MessageDialogType::OkCancel:
            result |= MB_OKCANCEL;
            break;

        case MessageDialogType::YesNo:
            result |= MB_YESNO;
            break;

        case MessageDialogType::YesNoCancel:
            result |= MB_YESNOCANCEL;
            break;

        case MessageDialogType::AbortRetryIgnore:
            result |= MB_CANCELTRYCONTINUE;
            break;

        case MessageDialogType::RetryCancel:
            result |= MB_RETRYCANCEL;
            break;
        }

        switch (image)
        {
        case MessageDialogImage::None:
            break;

        case MessageDialogImage::Error:
            result |= MB_ICONERROR;
            break;

        case MessageDialogImage::Warning:
            result |= MB_ICONWARNING;
            break;

        case MessageDialogImage::Information:
            result |= MB_ICONINFORMATION;
            break;

        case MessageDialogImage::Question:
            result |= MB_ICONQUESTION;
            break;
        }

        return result;
    }

    constexpr DialogResult ConvertDialogResult(int value) noexcept
    {
        switch (value)
        {
        case IDOK:
            return DialogResult::Ok;

        case IDYES:
            return DialogResult::Yes;

        case IDNO:
            return DialogResult::No;

        default:
        case IDCANCEL:
            return DialogResult::Cancel;

        case IDTRYAGAIN:
            return DialogResult::Retry;

        case IDCONTINUE:
            return DialogResult::Ignore;

        case IDABORT:
            return DialogResult::Abort;
        }
    }

    HRESULT SetDialogFilters(
        IFileDialog& self,
        std::span<FileDialogFilter const> filters)
    {
        std::vector<std::pair<std::wstring, std::wstring>> wfilters{};
        wfilters.reserve(filters.size());

        for (auto const& filter : filters)
        {
            auto& wfilter = wfilters.emplace_back();

            Interop::Windows::WidenString(wfilter.first, filter.Display);
            Interop::Windows::WidenString(wfilter.second, filter.Filter);
        }

        std::vector<COMDLG_FILTERSPEC> dialogFilters{};
        dialogFilters.reserve(wfilters.size());

        for (auto const& filter : wfilters)
        {
            dialogFilters.push_back({
                filter.first.c_str(),
                filter.second.c_str(),
            });
        }

        // It's safe to pass temporary data here because the dialog will make a copy
        return self.SetFileTypes(
            static_cast<UINT>(dialogFilters.size()),
            dialogFilters.data());
    }

    bool GetResult(IFileDialog& self, std::string& result)
    {
        Microsoft::WRL::ComPtr<IShellItem> item{};

        HRESULT hr = self.GetResult(item.GetAddressOf());

        wchar_t* wpath{};

        if (SUCCEEDED(hr))
        {
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, &wpath);

            if (SUCCEEDED(hr))
            {
                Interop::Windows::NarrowString(result, wpath);
                CoTaskMemFree(wpath);

                return true;
            }
        }
        else
        {
            result.clear();
        }

        return false;
    }

    bool GetResults(
        IFileOpenDialog& self,
        std::vector<std::string>& results)
    {
        Microsoft::WRL::ComPtr<IShellItemArray> items{};
        HRESULT hr = self.GetResults(items.GetAddressOf());

        DWORD dwCount{};

        if (SUCCEEDED(hr))
        {
            hr = items->GetCount(&dwCount);
        }

        for (DWORD i = 0; i < dwCount; ++i)
        {
            Microsoft::WRL::ComPtr<IShellItem> item{};
            wchar_t* wPath{};

            hr = items->GetItemAt(i, item.GetAddressOf());

            if (SUCCEEDED(hr))
            {
                hr = item->GetDisplayName(SIGDN_FILESYSPATH, &wPath);

                if (SUCCEEDED(hr))
                {
                    Interop::Windows::NarrowString(results.emplace_back(), wPath);
                    CoTaskMemFree(wPath);
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        if (FAILED(hr))
        {
            results.clear();
            return false;
        }

        return true;
    }

    DialogResult DialogResultFromHRESULT(HRESULT hr)
    {
        switch (hr)
        {
        case HRESULT_FROM_WIN32(ERROR_CANCELLED):
            {
                return DialogResult::Cancel;
            }
        case S_OK:
            {
                return DialogResult::Ok;
            }
        default:
            {
                break;
            }
        }
        return DialogResult::Cancel;
    }
}

namespace Anemone
{
    DialogResult Dialogs::ShowMessageDialog(
        Window const* window,
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

    DialogResult Dialogs::ShowMessageDialog(
        Window const* window,
        std::string_view message,
        std::string_view title,
        MessageDialogType type,
        MessageDialogImage image)
    {
        Internal::Windows::WindowImpl const* const native = static_cast<Internal::Windows::WindowImpl const*>(window);

        Interop::string_buffer<wchar_t, 128> wtitle{};
        Interop::string_buffer<wchar_t, 512> wmessage{};

        Interop::Windows::WidenString(wtitle, title);
        Interop::Windows::WidenString(wmessage, message);

        int const result = ::MessageBoxExW(
            (native != nullptr) ? native->GetHandle() : nullptr,
            wmessage.data(),
            wtitle.data(),
            Internal::CombineMessageDialogFlags(type, image),
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

        return Internal::ConvertDialogResult(result);
    }

    DialogResult Dialogs::OpenFile(
        Window const* window,
        std::string& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        Microsoft::WRL::ComPtr<IFileOpenDialog> dialog{};

        HRESULT hr = CoCreateInstance(
            CLSID_FileOpenDialog,
            nullptr,
            CLSCTX_ALL,
            IID_PPV_ARGS(dialog.GetAddressOf()));

        if (SUCCEEDED(hr))
        {
            hr = Internal::SetDialogFilters(*dialog.Get(), filters);
        }

        if (SUCCEEDED(hr) and not title.empty())
        {
            Interop::string_buffer<wchar_t, 128> wtitle{};
            Interop::Windows::WidenString(wtitle, title);

            hr = dialog->SetTitle(wtitle.data());
        }

        if (SUCCEEDED(hr))
        {
            Internal::Windows::WindowImpl const* native = static_cast<Internal::Windows::WindowImpl const*>(window); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            HWND const handle = (native != nullptr) ? native->GetHandle() : nullptr;

            hr = dialog->Show(handle);

            if (Internal::GetResult(*dialog.Get(), result))
            {
                return Internal::DialogResultFromHRESULT(hr);
            }
        }

        return DialogResult::Cancel;
    }

    DialogResult Dialogs::OpenFiles(
        Window const* window,
        std::vector<std::string>& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        Microsoft::WRL::ComPtr<IFileOpenDialog> dialog{};

        HRESULT hr = CoCreateInstance(
            CLSID_FileOpenDialog,
            nullptr,
            CLSCTX_ALL,
            IID_PPV_ARGS(dialog.GetAddressOf()));

        if (SUCCEEDED(hr))
        {
            hr = Internal::SetDialogFilters(*dialog.Get(), filters);
        }

        if (SUCCEEDED(hr) and not title.empty())
        {
            Interop::string_buffer<wchar_t, 128> wtitle{};
            Interop::Windows::WidenString(wtitle, title);
            hr = dialog->SetTitle(wtitle.data());
        }

        if (SUCCEEDED(hr))
        {
            FILEOPENDIALOGOPTIONS optoins{};
            hr = dialog->GetOptions(&optoins);

            if (SUCCEEDED(hr))
            {
                optoins |= FOS_ALLOWMULTISELECT;
                hr = dialog->SetOptions(optoins);
            }
        }

        if (SUCCEEDED(hr))
        {
            Internal::Windows::WindowImpl const* native = static_cast<Internal::Windows::WindowImpl const*>(window); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            HWND const handle = (native != nullptr) ? native->GetHandle() : nullptr;

            hr = dialog->Show(handle);

            if (Internal::GetResults(*dialog.Get(), result))
            {
                return Internal::DialogResultFromHRESULT(hr);
            }
        }

        return DialogResult::Cancel;
    }

    DialogResult Dialogs::SaveFile(
        Window const* window,
        std::string& result,
        std::string_view title,
        std::span<FileDialogFilter const> filters)
    {
        Microsoft::WRL::ComPtr<IFileSaveDialog> dialog{};

        HRESULT hr = CoCreateInstance(
            CLSID_FileSaveDialog,
            nullptr,
            CLSCTX_ALL,
            IID_PPV_ARGS(dialog.GetAddressOf()));

        if (SUCCEEDED(hr))
        {
            hr = Internal::SetDialogFilters(*dialog.Get(), filters);
        }

        if (SUCCEEDED(hr) and not title.empty())
        {
            Interop::string_buffer<wchar_t, 128> wtitle{};
            Interop::Windows::WidenString(wtitle, title);

            hr = dialog->SetTitle(wtitle.data());
        }

        if (SUCCEEDED(hr))
        {
            Internal::Windows::WindowImpl const* native = static_cast<Internal::Windows::WindowImpl const*>(window); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            HWND const handle = (native != nullptr) ? native->GetHandle() : nullptr;

            hr = dialog->Show(handle);

            if (Internal::GetResult(*dialog.Get(), result))
            {
                return Internal::DialogResultFromHRESULT(hr);
            }
        }

        return DialogResult::Cancel;
    }
}
