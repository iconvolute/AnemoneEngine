#include "AnemoneRuntime/Storage/Platform/Windows/WindowsMemoryMappedFile.hxx"
#include "AnemoneRuntime/Base/Bitwise.hxx"
#include "AnemoneRuntime/Diagnostics/Platform/Windows/WindowsDebug.hxx"
#include "AnemoneRuntime/Interop/Windows/Text.hxx"

namespace Anemone
{
    namespace
    {
        size_t GetSystemPageSize()
        {
            SYSTEM_INFO si;
            GetSystemInfo(&si);
            return si.dwAllocationGranularity;
        }

        constexpr DWORD TranslateCreationDisposition(FileMode mode)
        {
            switch (mode)
            {
            case FileMode::CreateNew:
                return CREATE_NEW;

            case FileMode::Create:
                return CREATE_ALWAYS;

            case FileMode::Open:
                return OPEN_EXISTING;

            case FileMode::Append:
            case FileMode::OpenOrCreate:
                return OPEN_ALWAYS;

            case FileMode::Truncate:
                return TRUNCATE_EXISTING;
            }

            return 0;
        }

        constexpr DWORD GetFileShare(MemoryMappedFileAccess access)
        {
            switch (access)
            {
            case MemoryMappedFileAccess::Read:
            case MemoryMappedFileAccess::ReadExecute:
                return FILE_SHARE_READ;

            case MemoryMappedFileAccess::ReadWrite:
            case MemoryMappedFileAccess::CopyOnWrite:
            case MemoryMappedFileAccess::ReadWriteExecute:
            case MemoryMappedFileAccess::Write:
                return FILE_SHARE_READ | FILE_SHARE_WRITE;
            }

            return FILE_SHARE_READ | FILE_SHARE_WRITE;
        }

        constexpr DWORD GetFileAccess(MemoryMappedFileAccess access)
        {
            switch (access)
            {
            case MemoryMappedFileAccess::Read:
            case MemoryMappedFileAccess::ReadExecute:
                return GENERIC_READ;

            case MemoryMappedFileAccess::ReadWrite:
            case MemoryMappedFileAccess::CopyOnWrite:
            case MemoryMappedFileAccess::ReadWriteExecute:
                return GENERIC_READ | GENERIC_WRITE;

            case MemoryMappedFileAccess::Write:
                return GENERIC_WRITE;
            }

            return GENERIC_WRITE;
        }

        constexpr DWORD GetPageAccess(MemoryMappedFileAccess access)
        {
            switch (access)
            {
            case MemoryMappedFileAccess::Read:
                return PAGE_READONLY;

            case MemoryMappedFileAccess::ReadWrite:
                return PAGE_READWRITE;

            case MemoryMappedFileAccess::CopyOnWrite:
                return PAGE_WRITECOPY;

            case MemoryMappedFileAccess::ReadExecute:
                return PAGE_EXECUTE_READ;

            case MemoryMappedFileAccess::ReadWriteExecute:
                return PAGE_EXECUTE_READWRITE;

            case MemoryMappedFileAccess::Write:
                return PAGE_READWRITE;
            }

            return PAGE_READONLY;
        }

        constexpr DWORD GetFileMapAccess(MemoryMappedFileAccess access)
        {
            switch (access)
            {
            case MemoryMappedFileAccess::Read:
                return FILE_MAP_READ;
            case MemoryMappedFileAccess::Write:
                return FILE_MAP_WRITE;
            case MemoryMappedFileAccess::ReadWrite:
                return FILE_MAP_READ | FILE_MAP_WRITE;
            case MemoryMappedFileAccess::CopyOnWrite:
                return FILE_MAP_COPY;
            case MemoryMappedFileAccess::ReadExecute:
                return FILE_MAP_EXECUTE | FILE_MAP_READ;
            case MemoryMappedFileAccess::ReadWriteExecute:
                return FILE_MAP_EXECUTE | FILE_MAP_READ | FILE_MAP_WRITE;
            }

            return FILE_MAP_READ;
        }

    }

    WindowsMemoryMappedFile::~WindowsMemoryMappedFile() = default;

    Reference<WindowsMemoryMappedFile> WindowsMemoryMappedFile::Create(std::string_view path, FileMode mode, uint64_t capacity, MemoryMappedFileAccess access)
    {
        Interop::Windows::FilePathW wPath{};
        Interop::Windows::WidenString(wPath, path);

        if (access == MemoryMappedFileAccess::Write)
        {
            // Write access is not supported for memory mapped files.
            return {};
        }

        if ((mode == FileMode::Append) or (mode == FileMode::Truncate))
        {
            // Not useful for memory mapped files
            return {};
        }

        bool existed = false;

        if (mode == FileMode::Open)
        {
            // Well, if we opened it, it existed.
            existed = true;
        }
        else if (mode == FileMode::CreateNew)
        {
            // If file creation failed, we assume it did not exist.
            existed = false;
        }
        else
        {
            // Check if the file exists.
            DWORD const dwAttributes = GetFileAttributesW(wPath.c_str());
            existed = (dwAttributes != INVALID_FILE_ATTRIBUTES) and (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
        }

        DWORD const dwCreationDisposition = TranslateCreationDisposition(mode);
        DWORD const dwAccess = GetFileAccess(access);
        DWORD const dwShare = GetFileShare(access);

        Interop::Windows::SafeFileHandle hFile{
            CreateFileW(
                wPath.c_str(),
                dwAccess,
                dwShare,
                nullptr,
                dwCreationDisposition,
                FILE_ATTRIBUTE_NORMAL,
                nullptr),
        };

        if (not hFile)
        {
            // Failed to open the file.
            return {};
        }

        uint64_t fileSize = 0;

        if ((mode != FileMode::CreateNew) and (mode != FileMode::Create))
        {
            if (hFile)
            {
                LARGE_INTEGER liFileSize{};
                if (GetFileSizeEx(hFile.Get(), &liFileSize))
                {
                    fileSize = std::bit_cast<uint64_t>(liFileSize.QuadPart);
                }
            }
        }

        if ((capacity == 0) and (fileSize == 0))
        {
            hFile = {};

            if (not existed)
            {
                // If the file did not exist, we delete it.
                DeleteFileW(wPath.c_str());
            }

            return {};
        }

        if (capacity == 0)
        {
            // If capacity is not specified, we use the file size.
            capacity = fileSize;
        }

        Interop::Windows::SafeMemoryMappedFileHandle hMapping{
            CreateFileMappingW(
                hFile.Get(),
                nullptr,
                GetPageAccess(access),
                static_cast<DWORD>(capacity >> 32u),
                static_cast<DWORD>(capacity),
                nullptr),
        };

        if (not hMapping)
        {
            hFile = {};

            if (not existed)
            {
                // If the file did not exist, we delete it.
                DeleteFileW(wPath.c_str());
            }

            return {};
        }

        return MakeReference<WindowsMemoryMappedFile>(
            std::move(hFile),
            std::move(hMapping));
    }

    void WindowsMemoryMappedFile::Flush()
    {
        FlushFileBuffers(this->_fileHandle.Get());
    }

    Reference<MemoryMappedFileView> WindowsMemoryMappedFile::CreateView(MemoryMappedFileAccess access, size_t offset, size_t length)
    {
        size_t const alignment = GetSystemPageSize();

        if (length == 0)
        {
            // If length is not specified, we use the file size.
            LARGE_INTEGER liFileSize{};
            if (not GetFileSizeEx(this->_fileHandle.Get(), &liFileSize))
            {
                return {};
            }

            // Include offset in the length calculation.
            length = std::bit_cast<size_t>(liFileSize.QuadPart) - offset;
        }

        size_t const mapBase = Bitwise::AlignDown(offset, alignment);
        size_t const viewDelta = offset - mapBase;
        size_t const mapSize = viewDelta + length;

        Interop::Windows::SafeMemoryMappedViewHandle mapView{
            MapViewOfFile(
                this->_mapHandle.Get(),
                GetFileMapAccess(access),
                static_cast<DWORD>(mapBase >> 32u),
                static_cast<DWORD>(mapBase),
                mapSize),
            mapSize,
        };

        if (not mapView)
        {
            return {};
        }

        return MakeReference<WindowsMemoryMappedFileView>(
            Reference{this},
            std::move(mapView),
            viewDelta,
            length);
    }
}

namespace Anemone
{
    std::span<std::byte const> WindowsMemoryMappedFileView::GetData() const
    {
        return this->_mapData.AsSpan().subspan(this->_offset, this->_size);
    }

    std::span<std::byte> WindowsMemoryMappedFileView::GetData()
    {
        return this->_mapData.AsSpan().subspan(this->_offset, this->_size);
    }

    void WindowsMemoryMappedFileView::Flush()
    {
        FlushViewOfFile(this->_mapData.GetData(), this->_mapData.GetSize());
    }
}

namespace Anemone
{
    Reference<MemoryMappedFile> MemoryMappedFile::Create(std::string_view path, FileMode mode, uint64_t capacity, MemoryMappedFileAccess access)
    {
        return WindowsMemoryMappedFile::Create(path, mode, capacity, access);
    }
}
