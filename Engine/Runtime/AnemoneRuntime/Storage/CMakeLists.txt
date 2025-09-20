add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PRIVATE
        "BinaryFormat.cxx"
        "DataReader.cxx"
        "DataWriter.cxx"
        "FileHandle.cxx"
        "FileHandleReader.cxx"
        "FileHandleWriter.cxx"
        "FileSystem.cxx"
        "MemoryReader.cxx"
        "MemoryWriter.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "BinaryFormat.hxx"
        "DataReader.hxx"
        "DataWriter.hxx"
        "FileHandle.hxx"
        "FileHandleReader.hxx"
        "FileHandleWriter.hxx"
        "FileSystem.hxx"
        "MemoryReader.hxx"
        "MemoryWriter.hxx"
)
