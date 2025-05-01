#pragma once
#include "AnemoneRuntime/Interop/Windows/Headers.hxx"
#include "AnemoneRuntime/Interop/StringBuffer.hxx"

#include <cassert>
#include <string_view>
#include <string>

namespace Anemone::Interop::Windows
{
    constexpr UINT32 DecodeSurrogatePair(UINT16 high, UINT16 low)
    {
        UINT32 code = 0x10000u;
        code += (high & 0x03FFu) << 10u;
        code += (low & 0x03FFu);
        return code;
    }

    anemone_forceinline bool WidenString(std::wstring& result, std::string_view value) noexcept
    {
        int const length = static_cast<int>(value.size());

        if (length > 0)
        {
            int const required = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0);

            if (required == 0)
            {
                result.clear();
                return false;
            }

            result.resize_and_overwrite(static_cast<size_t>(required), [&](wchar_t* ptr, size_t size) -> size_t
            {
                int const processed = MultiByteToWideChar(
                    CP_UTF8,
                    0,
                    value.data(),
                    static_cast<int>(value.size()),
                    ptr,
                    static_cast<int>(size));

                assert(required == processed);
                return static_cast<size_t>(processed);
            });
            return true;
        }

        result.clear();
        return true;
    }

    template <size_t CapacityT>
    anemone_forceinline bool WidenString(string_buffer<wchar_t, CapacityT>& result, std::string_view value) noexcept
    {
        int const length = static_cast<int>(value.length());

        if (length > 0)
        {
            int const required = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0);

            if (required == 0)
            {
                result.trim(0);
                return false;
            }

            result.resize_for_override(static_cast<size_t>(required) + 1);

            std::span<wchar_t> const writable = result.as_buffer_span();

            int const processed = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()));

            if (processed == required)
            {
                result.trim(static_cast<size_t>(processed));
                return true;
            }

            return false;
        }

        result.trim(0);
        return true;
    }

    anemone_forceinline bool NarrowString(std::string& result, std::wstring_view value) noexcept
    {
        if (not value.empty())
        {
            int const required = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                static_cast<int>(value.size()),
                nullptr,
                0,
                nullptr,
                nullptr);

            if (required == 0)
            {
                result.clear();
                return false;
            }

            result.resize_and_overwrite(static_cast<size_t>(required), [&](char* ptr, size_t size) -> size_t
            {
                int const processed = WideCharToMultiByte(
                    CP_UTF8,
                    0,
                    value.data(),
                    static_cast<int>(value.size()),
                    ptr,
                    static_cast<int>(size),
                    nullptr,
                    nullptr);

                assert(required == processed);
                return static_cast<size_t>(processed);
            });
            return true;
        }

        result.clear();
        return true;
    }

    template <size_t CapacityT>
    anemone_forceinline bool NarrowString(string_buffer<char, CapacityT>& result, std::wstring_view value) noexcept
    {
        int const length = static_cast<int>(value.length());

        if (length > 0)
        {
            int const required = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0,
                nullptr,
                nullptr);

            if (required == 0)
            {
                result.trim(0);
                return false;
            }

            result.resize_for_override(static_cast<size_t>(required) + 1);

            std::span<char> const writable = result.as_buffer_span();

            int const processed = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()),
                nullptr,
                nullptr);

            if (processed == required)
            {
                result.trim(static_cast<size_t>(processed));
                return true;
            }

            return false;
        }

        result.trim(0);
        return true;
    }

}
