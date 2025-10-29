#pragma once
#include "AnemoneInterop/Windows/Registry.hxx"

namespace Anemone::Interop::Windows
{
    template <typename Char, size_t Capacity>
    bool GetWindowsSdkVersion(string_buffer<Char, Capacity>& result)
    {
        if (RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Microsoft SDKs\Windows\v10.0)"))
        {
            return key.ReadString(L"ProductVersion", result);
        }

        return false;
    }

    template <typename Char, size_t Capacity>
    bool GetWindowsSdkLocation(string_buffer<Char, Capacity>& result)
    {
        if (RegistryKey key = RegistryKey::Open(HKEY_LOCAL_MACHINE, LR"(SOFTWARE\Microsoft\Microsoft SDKs\Windows\v10.0)"))
        {
            return key.ReadString(L"InstallationFolder", result);
        }

        return false;
    }
}
