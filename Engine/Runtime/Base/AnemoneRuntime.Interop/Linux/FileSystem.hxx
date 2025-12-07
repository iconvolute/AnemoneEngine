#pragma once
#include "AnemoneRuntime.Interop/Linux/Headers.hxx"
#include "AnemoneRuntime.Interop/StringBuffer.hxx"

namespace Anemone::Interop::Linux
{
    // Note: PATH_MAX is typically defined as 4 KiB.
    using FilePath = string_buffer<char, 256>;

    //
    // On Linux, read() (and similar system calls) will transfer at most 0x7ffff000 (2,147,479,552) bytes,
    // returning the number of bytes actually transferred.
    //

    inline constexpr size_t MaxIoRequestLength = 0x7ffff000uz;

    [[nodiscard]] constexpr int ValidateIoRequestLength(size_t value)
    {
        return static_cast<int>(std::min(value, MaxIoRequestLength));
    }

    template <size_t StaticCapacityT>
    inline bool LoadFile(string_buffer<char, StaticCapacityT>& buffer, const char* path)
    {
        bool result = false;

        int const fd = open(path, O_RDONLY);

        if (fd != -1)
        {
            struct stat fs;
            if (fstat(fd, &fs) == 0)
            {
                buffer.resize(fs.st_size);

                ssize_t const processed = read(fd, buffer.data(), buffer.size());

                if (processed >= 0)
                {
                    buffer.trim(static_cast<size_t>(processed));
                    result = true;
                }
                else
                {
                    buffer.trim(0);
                }
            }

            close(fd);
        }

        return result;
    }
}
