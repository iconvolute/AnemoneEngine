#include "AnemoneRuntime.Storage/FilePath.hxx"

namespace Anemone
{
    // TODO: move this to platform-specific constants
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

    constexpr std::string_view::size_type FindNextDirectorySeparator(std::string_view path, std::string_view::size_type pos)
    {
#if ANEMONE_PLATFORM_WINDOWS
        return path.find_first_of(AllDirectorySeparatorsString, pos);
#else
        return path.find_first_of(DirectorySeparatorChar, pos);
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

    void FilePath::AddDirectorySeparator(std::string& path)
    {
        if (path.empty() or not IsDirectorySeparator(path.back()))
        {
            path.push_back(DirectorySeparatorChar);
        }
    }

    void FilePath::RemoveDirectorySeparator(std::string& path)
    {
        if (not path.empty() and IsDirectorySeparator(path.back()))
        {
            path.pop_back();
        }
    }

    void FilePath::NormalizeDirectorySeparators(std::string& path)
    {
        auto it = path.begin();
        auto const end = path.end();

        auto out = it;

        while (it != end)
        {
            if (IsDirectorySeparator(*it))
            {
                // Skip all consecutive directory separators
                *out++ = DirectorySeparatorChar;

                while ((it != end) and IsDirectorySeparator(*it))
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

    void FilePath::Normalize(std::string& path)
    {
        // Replace '//' with '/'

        size_t pos = 0;

        while (true)
        {
            pos = path.find("//", pos);

            if (pos == std::string::npos)
            {
                break;
            }

            path.erase(pos, 1);
        }

        // Replace '/./' with '/'
        pos = 0;

        while (true)
        {
            pos = path.find("/./", pos);

            if (pos == std::string::npos)
            {
                break;
            }

            path.erase(pos, 2);
        }

        // Replace '/<fragment>/../' with '/'
        while (true)
        {
            pos = path.find("/../");

            if (pos == std::string::npos)
            {
                // No more parent directory references
                break;
            }

            // Find previous path fragment.
            size_t const prev = path.rfind('/', pos - 1);

            if (prev == std::string::npos)
            {
                // No previous fragment found
                break;
            }

            path.erase(prev + 1, pos - prev + 3);
        }

        if (path.ends_with("/."))
        {
            path.resize(path.size() - 2uz);
        }
    }

    void FilePath::PushFragment(std::string& path, std::string_view part)
    {
        if (path.empty())
        {
            path = part;
        }
        else
        {
            FilePath::AddDirectorySeparator(path);
            path.append(part);
        }
    }

    bool FilePath::PopFragment(std::string& path)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = FindLastDirectorySeparator(path);

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

    bool FilePath::PopFragment(std::string& path, std::string& tail)
    {
        if (not path.empty())
        {
            // Find directory separator and remove everything after it
            auto const separator = FindLastDirectorySeparator(path);

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

    std::string_view FilePath::GetFileName(std::string_view path)
    {
        std::string_view::size_type const separator = FindLastDirectorySeparator(path);

        if (separator != std::string_view::npos)
        {
            return path.substr(separator + 1);
        }

        return {};
    }

    void FilePath::SetFileName(std::string& path, std::string_view filename)
    {
        FilePath::PopFragment(path);
        FilePath::PushFragment(path, filename);
    }

    std::string_view FilePath::GetBaseName(std::string_view path)
    {
        std::string_view filename = FilePath::GetFileName(path);

        std::string_view::size_type const separator = FindExtensionSeparator(filename);

        if (separator != std::string_view::npos)
        {
            filename = filename.substr(0, separator);
        }

        // ReSharper disable once CppDFALocalValueEscapesFunction
        return filename;
    }

    void FilePath::SetBaseName(std::string& path, std::string_view baseName)
    {
        (void)path;
        (void)baseName;
        std::string_view::size_type name_start = FindLastDirectorySeparator(path);

        if (name_start == std::string_view::npos)
        {
            // No directory separator found, so start at the beginning
            name_start = 0;
        }
        else
        {
            // Skip the directory separator
            ++name_start;
        }

        std::string_view::size_type name_end = FindExtensionSeparator(path);

        if (name_end == std::string_view::npos)
        {
            // No extension separator found, so end at the end
            name_end = path.length();
        }

        // Remove the old base name
        path.replace(name_start, name_end - name_start, baseName);
    }

    std::string_view FilePath::GetExtension(std::string_view path)
    {
        std::string_view::size_type const separator = FindExtensionSeparator(path);

        if (separator != std::string_view::npos)
        {
            return path.substr(separator);
        }

        return {};
    }

    void FilePath::SetExtension(std::string& path, std::string_view extension)
    {
        // Strip any leading dots
        while (extension.starts_with('.'))
        {
            extension.remove_prefix(1);
        }

        std::string_view::size_type const separator = FindExtensionSeparator(path);

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
                path.push_back(ExtensionSeparatorChar);
            }
            else
            {
                // Extension separator found, so remove everything after it
                path.resize(separator + 1);
            }

            path.append(extension);
        }
    }

    void FilePath::MakeFileNameSafe(std::string& path)
    {
        for (char& c : path)
        {
            switch (c)
            {
            case '/':
            case '\\':
            case '<':
            case '>':
            case ':':
            case ';':
            case '"':
            case '|':
            case '?':
            case '*':
            case ',':
            case '.':
                c = '_';
                break;

            default:
                break;
            }
        }
    }

    bool FilePath::Relative(std::string& outRelativePath, std::string_view fromPath, std::string_view toPath)
    {
        // Normalize paths.
        std::string normalizedFromPath{fromPath};
        std::string normalizedToPath{toPath};

        Normalize(normalizedFromPath);
        Normalize(normalizedToPath);

        // Check if both paths are absolute or relative.
        bool const absoluteFromPath = !normalizedFromPath.empty() && IsDirectorySeparator(normalizedFromPath[0]);
        bool const absoluteToPath = !normalizedToPath.empty() && IsDirectorySeparator(normalizedToPath[0]);

        if (absoluteFromPath != absoluteToPath)
        {
            outRelativePath = normalizedToPath;
            return true;
        }

        std::string_view viewFromPath{normalizedFromPath};
        std::string_view viewToPath{normalizedToPath};

        while (true)
        {
            size_t nextFrom = FindNextDirectorySeparator(viewFromPath, 0);
            size_t nextTo = FindNextDirectorySeparator(viewToPath, 0);

            if ((nextFrom == std::string_view::npos) || (nextTo == std::string_view::npos))
            {
                // Reached the end of one of the paths.
                break;
            }

            std::string_view partFrom = viewFromPath.substr(0, nextFrom);
            std::string_view partTo = viewToPath.substr(0, nextTo);

            if (partFrom != partTo)
            {
                // Found a differing directory component.
                break;
            }

            viewFromPath.remove_prefix(nextFrom + 1);
            viewToPath.remove_prefix(nextTo + 1);
        }

        // Count path fragments remaining in 'fromPath'.

        size_t commonParts = 0;

        while (FindNextDirectorySeparator(viewFromPath, 0) != std::string_view::npos)
        {
            ++commonParts;
            viewFromPath.remove_prefix(FindNextDirectorySeparator(viewFromPath, 0) + 1);
        }

        // Prepare final path.
        outRelativePath.clear();

        // First, move up from 'fromPath' to common prefix
        for (size_t i = 0; i < commonParts; ++i)
        {
            outRelativePath += "../";
        }

        // Move into 'toPath' from common prefix
        outRelativePath += viewToPath;
        return true;
    }
}
