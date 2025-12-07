#include "AnemoneRuntime.Storage/FileSystem.hxx"
#include "AnemoneRuntime.Diagnostics/Debug.hxx"

namespace Anemone
{
    auto FileSystem::ReadTextFile(
        std::string_view path)
        -> std::expected<std::string, Error>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            if (auto length = (*handle)->GetLength())
            {
                std::string result;

                result.resize(*length);

                std::span view{reinterpret_cast<std::byte*>(result.data()), result.size()};

                do
                {
                    if (auto processed = (*handle)->Read(view))
                    {
                        if (*processed == 0)
                        {
                            // End of file.
                            break;
                        }

                        view = view.subspan(*processed);
                    }
                    else
                    {
                        return std::unexpected(processed.error());
                    }
                } while (true);

                return result;
            }
            else
            {
                return std::unexpected(length.error());
            }
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    auto FileSystem::ReadBinaryFile(
        std::string_view path)
        -> std::expected<Reference<MemoryBuffer>, Error>
    {
        if (auto handle = this->CreateFileReader(path))
        {
            if (auto length = (*handle)->GetLength())
            {
                auto result = MemoryBuffer::Create(*length);

                if (result)
                {
                    std::span<std::byte> bufferView = (*result)->GetView();

                    do
                    {
                        if (auto processed = (*handle)->Read(bufferView))
                        {
                            if (*processed == 0)
                            {
                                // End of file.
                                break;
                            }

                            bufferView = bufferView.subspan(*processed);
                        }
                        else
                        {
                            return std::unexpected(processed.error());
                        }
                    } while (true);

                }

                return result;
            }
            else
            {
                return std::unexpected(length.error());
            }
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    auto FileSystem::WriteTextFile(
        std::string_view path,
        std::string_view content)
        -> std::expected<void, Error>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            std::span view = std::as_bytes(std::span{content});

            while (not view.empty())
            {
                if (auto processed = (*handle)->Write(view))
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
        else
        {
            return std::unexpected(handle.error());
        }
    }

    auto FileSystem::WriteBinaryFile(
        std::string_view path,
        MemoryBuffer const& content)
        -> std::expected<void, Error>
    {
        if (auto handle = this->CreateFileWriter(path))
        {
            std::span contentView = content.GetView();

            while (not contentView.empty())
            {
                if (auto processed = (*handle)->Write(contentView))
                {
                    // Update content to the remaining part.
                    contentView = contentView.subspan(*processed);
                }
                else
                {
                    return std::unexpected(processed.error());
                }
            }

            return {};
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    auto FileSystem::FileCopy(
        std::string_view source,
        std::string_view destination,
        NameCollisionResolve nameCollisionResolve)
        -> std::expected<void, Error>
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
                return std::unexpected(Error::AlreadyExists);
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
            if (auto readerProcessed = (*reader)->Read(bufferView))
            {
                if (*readerProcessed == 0)
                {
                    // End of file.
                    return {};
                }

                // Get part of buffer to write.
                std::span toWrite = bufferView.subspan(0, *readerProcessed);

                // Write view back to destination.
                while (not toWrite.empty())
                {
                    if (auto writerProcessed = (*writer)->Write(bufferView.subspan(0, *readerProcessed)))
                    {
                        toWrite = toWrite.subspan(*writerProcessed);
                    }
                    else
                    {
                        return std::unexpected(writerProcessed.error());
                    }
                }
            }
            else
            {
                return std::unexpected(readerProcessed.error());
            }
        }
    }
}
