#include "AnemoneRuntime/Storage/FileSystem.hxx"
#include "AnemoneRuntime/Storage/FileHandleReader.hxx"

namespace Anemone::Internal
{
    extern void InitializeFileSystem()
    {
        extern void PlatformInitializeFileSystem();
        PlatformInitializeFileSystem();
    }

    extern void FinalizeFileSystem()
    {
        extern void PlatformFinalizeFileSystem();
        PlatformFinalizeFileSystem();
    }
}

namespace Anemone
{
    auto FileSystem::ReadTextFile(std::string_view path) -> std::expected<std::string, Status>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            if (auto length = handle->GetLength())
            {
                std::string result;

                result.resize(*length);

                std::span view{reinterpret_cast<std::byte*>(result.data()), result.size()};

                do
                {
                    if (auto processed = handle->Read(view))
                    {
                        if (*processed == 0)
                        {
                            // End of file.
                            break;
                        }

                        // Update view to the remaining part.
                        view = view.subspan(*processed);
                    }
                    else
                    {
                        return std::unexpected(processed.error());
                    }

                } while (true);

                return result;
            }
        }

        return std::unexpected(Status::FileNotFound);
    }

    auto FileSystem::ReadBinaryFile(std::string_view path) -> std::expected<std::vector<std::byte>, Status>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            if (auto length = handle->GetLength())
            {
                std::vector<std::byte> result;
                result.resize(*length);

                std::span view{result.data(), result.size()};

                do
                {
                    if (auto processed = handle->Read(view))
                    {
                        if (*processed == 0)
                        {
                            // End of file.
                            break;
                        }

                        // Update view to the remaining part.
                        view = view.subspan(*processed);
                    }
                    else
                    {
                        return std::unexpected(processed.error());
                    }

                } while (true);

                return result;
            }
        }

        return std::unexpected(Status::FileNotFound);
    }

    auto FileSystem::WriteTextFile(std::string_view path, std::string_view content) -> std::expected<void, Status>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            std::span view = std::as_bytes(std::span{content});

            if (not handle->Truncate(static_cast<int64_t>(view.size())))
            {
                return std::unexpected(Status::IoError);
            }

            while (not view.empty())
            {
                if (auto processed = handle->Write(view))
                {
                    // Update view to the remaining part.
                    view = view.subspan(*processed);
                }
                else
                {
                    return std::unexpected(processed.error());
                }
            }

            return {};
        }

        return std::unexpected(Status::FileNotFound);
    }

    auto FileSystem::WriteBinaryFile(std::string_view path, std::span<std::byte const> content) -> std::expected<void, Status>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            if (not handle->Truncate(static_cast<int64_t>(content.size())))
            {
                return std::unexpected(Status::IoError);
            }

            while (not content.empty())
            {
                if (auto processed = handle->Write(content))
                {
                    // Update content to the remaining part.
                    content = content.subspan(*processed);
                }
                else
                {
                    return std::unexpected(processed.error());
                }
            }

            return {};
        }

        return std::unexpected(Status::FileNotFound);
    }
}
