#include "AnemoneRuntime/System/FileSystem.hxx"
#include "AnemoneRuntime/String.hxx"
#include "AnemoneRuntime/System/Path.hxx"

namespace Anemone::System
{
    std::expected<std::vector<DirectoryEntry>, ErrorCode> EnumerateDirectoryRecursively(std::string root, std::string_view pattern)
    {
        std::vector<DirectoryEntry> result;
        std::vector<DirectoryEnumerator> stack;

        stack.emplace_back(std::move(root));

        while (not stack.empty())
        {
            DirectoryEnumerator& enumerator = stack.back();
            if (std::optional<std::expected<DirectoryEntry, ErrorCode>> const& next = enumerator.Next())
            {
                if (std::expected<DirectoryEntry, ErrorCode> const& item = *next)
                {
                    DirectoryEntry const& entry = *item;

                    std::string_view const filename = Path::GetFilename(entry.Path);

                    if (entry.Type == FileType::Directory)
                    {
                        if ((filename == ".") or (filename == ".."))
                        {
                            continue;
                        }

                        stack.emplace_back(entry.Path);
                    }

                    if (MatchWildcard(pattern, filename))
                    {
                        result.push_back(entry);
                    }
                }
                else
                {
                    stack.pop_back();
                    // Happily ignore errors here
                }
            }
            else
            {
                stack.pop_back();
            }
        }

        return result;
    }
}
