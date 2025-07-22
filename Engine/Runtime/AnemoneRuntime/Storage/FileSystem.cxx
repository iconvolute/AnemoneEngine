#include "AnemoneRuntime/Storage/FileSystem.hxx"

#include "AnemoneRuntime/Diagnostics/Debug.hxx"
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
            auto length = handle->GetLength();
            std::string result;

            result.resize(length);

            std::span view{reinterpret_cast<std::byte*>(result.data()), result.size()};

            do
            {
                size_t processed = handle->Read(view);

                if (processed == 0)
                {
                    // End of file.
                    break;
                }

                view = view.subspan(processed);
            } while (true);

            return result;
        }

        return std::unexpected(Status::FileNotFound);
    }

    auto FileSystem::ReadBinaryFile(std::string_view path) -> std::expected<std::vector<std::byte>, Status>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            auto length = handle->GetLength();
            std::vector<std::byte> result;
            result.resize(length);

            std::span view{result.data(), result.size()};

            do
            {
                auto processed = handle->Read(view);
                if (processed == 0)
                {
                    // End of file.
                    break;
                }

                // Update view to the remaining part.
                view = view.subspan(processed);
            } while (true);

            return result;
        }

        return std::unexpected(Status::FileNotFound);
    }

    auto FileSystem::WriteTextFile(std::string_view path, std::string_view content) -> std::expected<void, Status>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            AE_ASSERT(handle->GetLength() == 0);

            std::span view = std::as_bytes(std::span{content});

            while (not view.empty())
            {
                size_t processed = handle->Write(view);
                // Update view to the remaining part.
                view = view.subspan(processed);
            }

            return {};
        }

        return std::unexpected(Status::FileNotFound);
    }

    auto FileSystem::WriteBinaryFile(std::string_view path, std::span<std::byte const> content) -> std::expected<void, Status>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            AE_ASSERT(handle->GetLength() == 0);

            while (not content.empty())
            {
                size_t processed = handle->Write(content);
                // Update content to the remaining part.
                content = content.subspan(processed);
            }

            return {};
        }

        return std::unexpected(Status::FileNotFound);
    }
}
