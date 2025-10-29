#pragma once
#include "AnemoneInterop/Windows/Headers.hxx"
#include "AnemoneInterop/Windows/StringBuffer.hxx"

#include <cassert>
#include <string_view>
#include <string>

namespace Anemone::Interop::Windows
{
    constexpr UINT32 DecodeSurrogatePair(UINT16 high, UINT16 low)
    {
        assert(IS_SURROGATE_PAIR(high, low));
        UINT32 code = 0x10000u;
        code += (high & 0x03FFu) << 10u;
        code += (low & 0x03FFu);
        return code;
    }

    template <size_t Capacity>
    HRESULT WidenString(string_buffer<wchar_t, Capacity>& destination, std::string_view source)
    {
        destination.clear();

        if (source.size() > static_cast<size_t>(INT_MAX))
        {
            return E_INVALIDARG;
        }

        if (source.empty())
        {
            return S_OK;
        }

        const char* const sourceData = source.data();
        int const sourceSize = static_cast<int>(source.length());

        int const required = ::MultiByteToWideChar(
            CP_UTF8,
            0,
            sourceData,
            sourceSize,
            nullptr,
            0);

        if (required == 0)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        destination.resize_for_override(static_cast<size_t>(required));

        int const processed = ::MultiByteToWideChar(
            CP_UTF8,
            0,
            sourceData,
            sourceSize,
            destination.data(),
            required);

        if (processed != required)
        {
            destination.clear();
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return S_OK;
    }

    inline HRESULT WidenString(std::wstring& destination, std::string_view source)
    {
        destination.clear();

        if (source.size() > static_cast<size_t>(INT_MAX))
        {
            return E_INVALIDARG;
        }

        if (source.empty())
        {
            return S_OK;
        }

        const char* const sourceData = source.data();
        int const sourceSize = static_cast<int>(source.size());

        int const required = ::MultiByteToWideChar(
            CP_UTF8,
            0,
            sourceData,
            sourceSize,
            nullptr,
            0);

        if (required == 0)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        HRESULT hr = S_OK;

        destination.resize_and_overwrite(static_cast<size_t>(required), [&](wchar_t* destinationData, size_t destinationSize) -> size_t
        {
            int const processed = ::MultiByteToWideChar(
                CP_UTF8,
                0,
                sourceData,
                sourceSize,
                destinationData,
                static_cast<int>(destinationSize));

            if (processed != required)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                return 0uz;
            }

            return destinationSize;
        });

        return hr;
    }

    template <size_t Capacity>
    HRESULT NarrowString(string_buffer<char, Capacity>& destination, std::wstring_view source)
    {
        destination.clear();

        if (source.size() > static_cast<size_t>(INT_MAX))
        {
            return E_INVALIDARG;
        }

        if (source.empty())
        {
            return S_OK;
        }

        const wchar_t* const sourceData = source.data();
        int const sourceSize = static_cast<int>(source.size());

        int const required = ::WideCharToMultiByte(
            CP_UTF8,
            0,
            sourceData,
            sourceSize,
            nullptr,
            0,
            nullptr,
            nullptr);

        if (required == 0)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        destination.resize_for_override(static_cast<size_t>(required));

        int const processed = ::WideCharToMultiByte(
            CP_UTF8,
            0,
            sourceData,
            sourceSize,
            destination.data(),
            required,
            nullptr,
            nullptr);

        if (processed != required)
        {
            destination.clear();
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return S_OK;
    }

    inline HRESULT NarrowString(std::string& destination, std::wstring_view source)
    {
        destination.clear();

        if (source.size() > static_cast<size_t>(INT_MAX))
        {
            return E_INVALIDARG;
        }

        if (source.empty())
        {
            return S_OK;
        }

        const wchar_t* const sourceData = source.data();
        int const sourceSize = static_cast<int>(source.size());

        int const required = ::WideCharToMultiByte(
            CP_UTF8,
            0,
            sourceData,
            sourceSize,
            nullptr,
            0,
            nullptr,
            nullptr);

        if (required == 0)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        HRESULT hr = S_OK;

        destination.resize_and_overwrite(static_cast<size_t>(required), [&](char* destinationData, size_t destinationSize) -> size_t
        {
            int const processed = ::WideCharToMultiByte(
                CP_UTF8,
                0,
                sourceData,
                sourceSize,
                destinationData,
                static_cast<int>(destinationSize),
                nullptr,
                nullptr);

            if (processed != required)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                return 0uz;
            }

            return destinationSize;
        });

        return hr;
    }
}
