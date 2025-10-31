#include "AnemoneApplication/Platform/Windows/WindowsHostDialogs.hxx"
#include "AnemoneApplication/Platform/Windows/WindowsHostWindow.hxx"
#include "AnemoneInterop/Windows/Text.hxx"
#include "AnemoneInterop/Windows/SafeHandle.hxx"

#include <wrl/client.h>
#include <CommCtrl.h>

namespace Anemone
{
    UINT WindowsHostDialogs::CombineMessageDialogFlags(MessageDialogType type, MessageDialogImage image)
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

    DialogResult WindowsHostDialogs::ConvertDialogResult(int value)
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

    HRESULT WindowsHostDialogs::SetDialogFilters(IFileDialog& self, std::span<FileDialogFilter const> filters)
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

    bool WindowsHostDialogs::GetResult(IFileDialog& self, std::string& result)
    {
        Microsoft::WRL::ComPtr<IShellItem> item{};

        HRESULT hr = self.GetResult(item.GetAddressOf());

        if (SUCCEEDED(hr))
        {
            Interop::Windows::SafeComTaskMemoryHandle<wchar_t> wPath{};
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, wPath.GetAddressOf());

            if (SUCCEEDED(hr))
            {
                Interop::Windows::NarrowString(result, wPath.Get());
                return true;
            }
        }
        else
        {
            result.clear();
        }

        return false;
    }

    bool WindowsHostDialogs::GetResults(IFileOpenDialog& self, std::vector<std::string>& results)
    {
        Microsoft::WRL::ComPtr<IShellItemArray> items{};
        HRESULT hr = self.GetResults(items.GetAddressOf());

        DWORD dwCount{};

        if (SUCCEEDED(hr))
        {
            hr = items->GetCount(&dwCount);
        }

        Interop::Windows::SafeComTaskMemoryHandle<wchar_t> wPath{};

        for (DWORD i = 0; i < dwCount; ++i)
        {
            Microsoft::WRL::ComPtr<IShellItem> item{};

            hr = items->GetItemAt(i, item.GetAddressOf());

            if (SUCCEEDED(hr))
            {
                hr = item->GetDisplayName(SIGDN_FILESYSPATH, wPath.ResetAndGetAddressOf());

                if (SUCCEEDED(hr))
                {
                    Interop::Windows::NarrowString(results.emplace_back(), wPath.Get());
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

    DialogResult WindowsHostDialogs::DialogResultFromHRESULT(HRESULT hr)
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
        HWND hWindow{};

        if (WindowsHostWindow const* const native = static_cast<WindowsHostWindow const*>(window))
        {
            hWindow = native->GetHandle();
        }

        Interop::string_buffer<wchar_t, 128> wTitle{};
        Interop::string_buffer<wchar_t, 512> wMessage{};

        Interop::Windows::WidenString(wTitle, title);
        Interop::Windows::WidenString(wMessage, message);

        int const result = ::MessageBoxExW(
            hWindow,
            wMessage.data(),
            wTitle.data(),
            WindowsHostDialogs::CombineMessageDialogFlags(type, image),
            MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

        return WindowsHostDialogs::ConvertDialogResult(result);
    }

    DialogResult HostDialogs::OpenFile(
        HostWindow const* window,
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
            hr = WindowsHostDialogs::SetDialogFilters(*dialog.Get(), filters);
        }

        if (SUCCEEDED(hr) and not title.empty())
        {
            Interop::string_buffer<wchar_t, 128> wtitle{};
            Interop::Windows::WidenString(wtitle, title);

            hr = dialog->SetTitle(wtitle.data());
        }

        if (SUCCEEDED(hr))
        {
            HWND hWindow{};

            if (WindowsHostWindow const* const native = static_cast<WindowsHostWindow const*>(window))
            {
                hWindow = native->GetHandle();
            }

            hr = dialog->Show(hWindow);

            if (WindowsHostDialogs::GetResult(*dialog.Get(), result))
            {
                return WindowsHostDialogs::DialogResultFromHRESULT(hr);
            }
        }

        return DialogResult::Cancel;
    }

    DialogResult HostDialogs::OpenFiles(
        HostWindow const* window,
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
            hr = WindowsHostDialogs::SetDialogFilters(*dialog.Get(), filters);
        }

        if (SUCCEEDED(hr) and not title.empty())
        {
            Interop::string_buffer<wchar_t, 128> wTitle{};
            Interop::Windows::WidenString(wTitle, title);
            hr = dialog->SetTitle(wTitle.data());
        }

        if (SUCCEEDED(hr))
        {
            FILEOPENDIALOGOPTIONS options{};
            hr = dialog->GetOptions(&options);

            if (SUCCEEDED(hr))
            {
                options |= FOS_ALLOWMULTISELECT;
                hr = dialog->SetOptions(options);
            }
        }

        if (SUCCEEDED(hr))
        {
            HWND hWindow{};

            if (WindowsHostWindow const* const native = static_cast<WindowsHostWindow const*>(window))
            {
                hWindow = native->GetHandle();
            }

            hr = dialog->Show(hWindow);

            if (WindowsHostDialogs::GetResults(*dialog.Get(), result))
            {
                return WindowsHostDialogs::DialogResultFromHRESULT(hr);
            }
        }

        return DialogResult::Cancel;
    }

    DialogResult HostDialogs::SaveFile(
        HostWindow const* window,
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
            hr = WindowsHostDialogs::SetDialogFilters(*dialog.Get(), filters);
        }

        if (SUCCEEDED(hr) and not title.empty())
        {
            Interop::string_buffer<wchar_t, 128> wTitle{};
            Interop::Windows::WidenString(wTitle, title);

            hr = dialog->SetTitle(wTitle.data());
        }

        if (SUCCEEDED(hr))
        {
            HWND hWindow{};

            if (WindowsHostWindow const* const native = static_cast<WindowsHostWindow const*>(window))
            {
                hWindow = native->GetHandle();
            }

            hr = dialog->Show(hWindow);

            if (WindowsHostDialogs::GetResult(*dialog.Get(), result))
            {
                return WindowsHostDialogs::DialogResultFromHRESULT(hr);
            }
        }

        return DialogResult::Cancel;
    }
}
