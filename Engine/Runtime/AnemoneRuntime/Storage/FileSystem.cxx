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
    auto FileSystem::CreateFileReader(
        std::string_view path)
        -> std::expected<std::unique_ptr<FileHandle>, ErrorCode>
    {
        return this->CreateFile(
            path,
            FileMode::OpenExisting,
            FileAccess::Read,
            FileOption::None);
    }

    auto FileSystem::CreateFileWriter(
        std::string_view path)
        -> std::expected<std::unique_ptr<FileHandle>, ErrorCode>
    {
        return this->CreateFile(
            path,
            FileMode::CreateAlways,
            FileAccess::ReadWrite,
            FileOption::None);
    }

    auto FileSystem::ReadTextFile(
        std::string_view path)
        -> std::expected<std::string, ErrorCode>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            auto length = (*handle)->GetLength();
            std::string result;

            result.resize(length);

            std::span view{reinterpret_cast<std::byte*>(result.data()), result.size()};

            do
            {
                size_t processed = (*handle)->Read(view);

                if (processed == 0)
                {
                    // End of file.
                    break;
                }

                view = view.subspan(processed);
            } while (true);

            return result;
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    auto FileSystem::ReadBinaryFile(
        std::string_view path)
        -> std::expected<std::vector<std::byte>, ErrorCode>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            auto length = (*handle)->GetLength();
            std::vector<std::byte> result;
            result.resize(length);

            std::span view{result.data(), result.size()};

            do
            {
                auto processed = (*handle)->Read(view);
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

        return std::unexpected(ErrorCode::NotFound);
    }

    auto FileSystem::WriteTextFile(
        std::string_view path,
        std::string_view content)
        -> std::expected<void, ErrorCode>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            AE_ASSERT((*handle)->GetLength() == 0);

            std::span view = std::as_bytes(std::span{content});

            while (not view.empty())
            {
                size_t processed = (*handle)->Write(view);
                // Update view to the remaining part.
                view = view.subspan(processed);
            }

            return {};
        }

        return std::unexpected(ErrorCode::NotFound);
    }

    auto FileSystem::WriteBinaryFile(
        std::string_view path,
        std::span<std::byte const> content)
        -> std::expected<void, ErrorCode>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            AE_ASSERT((*handle)->GetLength() == 0);

            while (not content.empty())
            {
                size_t processed = (*handle)->Write(content);
                // Update content to the remaining part.
                content = content.subspan(processed);
            }

            return {};
        }

        return std::unexpected(ErrorCode::NotFound);
    }

    auto FileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve)
        -> std::expected<void, ErrorCode>
    {
        auto reader = this->CreateFileReader(source);

        if (not reader)
        {
            return std::unexpected(reader.error());
        }

        if (this->FileExists(destination))
        {
            if (nameCollisionResolve == NameCollisionResolve::Fail)
            {
                return std::unexpected(ErrorCode::AlreadyExists);
            }
        }

        auto writer = this->CreateFileWriter(destination);

        if (not writer)
        {
            return std::unexpected(writer.error());
        }

        constexpr size_t DefaultBufferSize = 1u << 20u;

        std::unique_ptr<std::byte[]> buffer = std::make_unique_for_overwrite<std::byte[]>(DefaultBufferSize);
        std::span bufferView{buffer.get(), DefaultBufferSize};

        while (true)
        {
            size_t const readerProcessed = (*reader)->Read(bufferView);

            if (readerProcessed == 0)
            {
                // End of file.
                return {};
            }

            // Get part of buffer to write.
            std::span toWrite = bufferView.subspan(0, readerProcessed);

            // Write view back to destination.
            while (not toWrite.empty())
            {
                size_t const writerProcessed = (*writer)->Write(bufferView.subspan(0, readerProcessed));

                if (!writerProcessed)
                {
                    // Failed to write
                    return std::unexpected(ErrorCode::IoError);
                }

                toWrite = toWrite.subspan(writerProcessed);
            }
        }
    }
}
