#include "AnemoneRuntime/System/Path.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::System::Private
{
#if ANEMONE_PLATFORM_WINDOWS
    inline constexpr char DirectorySeparatorChar = '/';
    inline constexpr char AlternativeDirectorySeparatorChar = '\\';
    inline constexpr char VolumeSeparatorChar = ':';
    inline constexpr char PathSeparatorChar = ';';
    inline constexpr char ExtensionSeparatorChar = '.';
    inline constexpr std::string_view AllDirectorySeparatorsString = "/\\";
#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
    inline constexpr char DirectorySeparatorChar = '/';
    inline constexpr char AlternativeDirectorySeparatorChar = '/';
    inline constexpr char VolumeSeparatorChar = '/';
    inline constexpr char PathSeparatorChar = ':';
    inline constexpr char ExtensionSeparatorChar = '.';
    inline constexpr char AllDirectorySeparatorsString = '/';
#else
#error "Unsupported platform"
#endif

    constexpr bool IsDirectorySeparator(char c)
    {
#if ANEMONE_PLATFORM_WINDOWS
        return c == DirectorySeparatorChar or c == AlternativeDirectorySeparatorChar;
#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
        return c == DirectorySeparatorChar;
#else
#error "Unsupported platform"
#endif
    }

    constexpr std::string_view::size_type FindLastDirectorySeparator(std::string_view path)
    {
#if ANEMONE_PLATFORM_WINDOWS
        return path.find_last_of(AllDirectorySeparatorsString);
#elif ANEMONE_PLATFORM_LINUX || ANEMONE_PLATFORM_ANDROID
        return path.find_last_of(DirectorySeparatorChar);
#else
#error "Unsupported platform"
#endif
    }

    constexpr std::string_view::size_type FindExtensionSeparator(std::string_view path)
    {
        std::string_view::size_type const length = path.length();

        for (std::string_view::size_type i = length; i > 0; --i)
        {
            char const c = path[i - 1];

            if (c == ExtensionSeparatorChar)
            {
                return i - 1;
            }

            if (IsDirectorySeparator(c))
            {
                break;
            }
        }

        return std::string_view::npos;
    }
}

namespace Anemone::System
{
    void Path::AddDirectorySeparator(std::string& path)
    {
        if (path.empty() or not Private::IsDirectorySeparator(path.back()))
        {
            path.push_back(Private::DirectorySeparatorChar);
        }
    }

    void Path::RemoveDirectorySeparator(std::string& path)
    {
        if (not path.empty() and Private::IsDirectorySeparator(path.back()))
        {
            path.pop_back();
        }
    }

    void Path::NormalizeDirectorySeparators(std::string& path)
    {
        auto it = path.begin();
        auto const end = path.end();

        auto out = it;

        while (it != end)
        {
            if (Private::IsDirectorySeparator(*it))
            {
                // Skip all consecutive directory separators
                *out++ = Private::DirectorySeparatorChar;

                while ((it != end) and Private::IsDirectorySeparator(*it))
                {
                    ++it;
                }
            }
            else
            {
                // Copy character
                *out++ = *it++;
            }
        }

        // Trim path
        path.resize(out - path.begin());
    }

    void Path::Normalize(std::string& path)
    {
        NormalizeDirectorySeparators(path);

        static constexpr std::string_view parent{"/.."};

        while (true)
        {
            if (path.empty())
            {
                break;
            }

            // if ((path.compare(0, 2, "..") == 0) or (path.compare(0, parent.length(), parent) == 0))
            //{
            //     return;
            // }

            std::string::size_type const index = path.find(parent);

            if (index == std::string::npos)
            {
                break;
            }

            std::string::size_type previous = index;

            while (true)
            {
                previous = path.rfind(Private::DirectorySeparatorChar, previous - 1);

                if ((previous == 0) or (previous == std::string::npos))
                {
                    break;
                }

                if (((index - previous) > 1) and ((path[previous + 1] != '.') or (path[previous + 2] != Private::DirectorySeparatorChar)))
                {
                    break;
                }
            }

            std::string::size_type const colon = path.find(Private::VolumeSeparatorChar, previous);

            if ((colon != 0) and (colon < index))
            {
                break;
            }

            path.erase(previous, index - previous + parent.length());
        }

        // remove single dots
        while (true)
        {
            std::string::size_type const index = path.find("/.");

            if (index == std::string::npos)
            {
                break;
            }

            path.erase(index, 2);
        }
    }

    void Path::Push(std::string& path, std::string_view part)
    {
        if (path.empty())
        {
            path = part;
        }
        else
        {
            AddDirectorySeparator(path);
            path.append(part);
        }
    }

    bool Path::Pop(std::string& path)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = Private::FindLastDirectorySeparator(path);

            if (separator != std::string_view::npos)
            {
                path.resize(separator);
            }
            else
            {
                path.clear();
            }

            return true;
        }

        return false;
    }

    bool Path::Pop(std::string& path, std::string& tail)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = Private::FindLastDirectorySeparator(path);

            if (separator != std::string_view::npos)
            {
                // Tail is everything after the separator
                tail = path.substr(separator + 1);
                path.resize(separator);
            }
            else
            {
                // Path don't contain any directory separators, so the whole path is the tail
                tail = std::move(path);
                path.clear();
            }

            return true;
        }

        tail.clear();
        return false;
    }

    void Path::SetExtension(std::string& path, std::string_view extension)
    {
        // Strip any leading dots
        while (extension.starts_with('.'))
        {
            extension.remove_prefix(1);
        }

        std::string_view::size_type const separator = Private::FindExtensionSeparator(path);

        if (extension.empty())
        {
            // Remove extension
            if (separator != std::string_view::npos)
            {
                path.resize(separator);
            }
        }
        else
        {
            // Append extension
            if (separator == std::string_view::npos)
            {
                // Extension separator not found, so append it
                path.push_back(Private::ExtensionSeparatorChar);
            }
            else
            {
                // Extension separator found, so remove everything after it
                path.resize(separator + 1);
            }

            path.append(extension);
        }
    }

    void Path::SetFilename(std::string& path, std::string_view filename)
    {
        Pop(path);
        Push(path, filename);
    }

    std::string_view Path::GetExtension(std::string_view path)
    {
        std::string_view::size_type const separator = Private::FindExtensionSeparator(path);

        if (separator != std::string_view::npos)
        {
            return path.substr(separator);
        }

        return {};
    }

    std::string_view Path::GetFilename(std::string_view path)
    {
        std::string_view::size_type const separator = Private::FindLastDirectorySeparator(path);

        if (separator != std::string_view::npos)
        {
            return path.substr(separator + 1);
        }

        return {};
    }

    std::string_view Path::GetFilenameWithoutExtension(std::string_view path)
    {
        std::string_view filename = GetFilename(path);

        std::string_view::size_type const separator = Private::FindExtensionSeparator(filename);

        if (separator != std::string_view::npos)
        {
            filename = filename.substr(0, separator);
        }

        // ReSharper disable once CppDFALocalValueEscapesFunction
        return filename;
    }
}
