#include "AnemoneBase/Version.hxx"

namespace Anemone
{
    std::to_chars_result ToChars(char* first, char* last, Version const& version)
    {
        if (first >= last)
        {
            return {first, std::errc::value_too_large};
        }

        auto res = std::to_chars(first, last, version.Major);

        if (res.ec != std::errc())
        {
            return res;
        }

        if (++res.ptr >= last)
        {
            return {res.ptr, std::errc::value_too_large};
        }

        *res.ptr++ = '.';

        res = std::to_chars(res.ptr, last, version.Minor);

        if (res.ec != std::errc())
        {
            return res;
        }

        if (++res.ptr >= last)
        {
            return {res.ptr, std::errc::value_too_large};
        }

        *res.ptr++ = '.';

        res = std::to_chars(res.ptr, last, version.Patch);

        if (res.ec != std::errc())
        {
            return res;
        }

        if (++res.ptr >= last)
        {
            return {res.ptr, std::errc::value_too_large};
        }

        *res.ptr++ = '.';

        return std::to_chars(res.ptr, last, version.Tweak);
    }

    std::from_chars_result FromChars(char const* first, char const* last, Version& version)
    {
        if (first >= last)
        {
            return {first, std::errc::invalid_argument};
        }

        auto res = std::from_chars(first, last, version.Major);

        if (res.ec != std::errc())
        {
            return res;
        }

        if (res.ptr >= last || *res.ptr++ != '.')
        {
            return {res.ptr, std::errc::invalid_argument};
        }

        res = std::from_chars(res.ptr, last, version.Minor);

        if (res.ec != std::errc())
        {
            return res;
        }

        if (res.ptr >= last || *res.ptr++ != '.')
        {
            return {res.ptr, std::errc::invalid_argument};
        }

        res = std::from_chars(res.ptr, last, version.Patch);

        if (res.ec != std::errc())
        {
            return res;
        }

        if (res.ptr >= last || *res.ptr++ != '.')
        {
            return {res.ptr, std::errc::invalid_argument};
        }

        return std::from_chars(res.ptr, last, version.Tweak);
    }
}
