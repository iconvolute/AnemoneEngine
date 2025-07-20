add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PRIVATE
        "BinaryFormat.cxx"
        "BinaryFormat.hxx"
        "DataReader.cxx"
        "DataReader.hxx"
        "DataWriter.cxx"
        "DataWriter.hxx"
        "FileHandleReader.cxx"
        "FileHandleReader.hxx"
        "FileHandleWriter.cxx"
        "FileHandleWriter.hxx"

        "FileHandle.hxx"
        "FileHandle.cxx"
        
        "FileSystem.hxx"
        "FileSystem.cxx"

        "MemoryReader.cxx"
        "MemoryReader.hxx"
        "MemoryWriter.cxx"
        "MemoryWriter.hxx"
)
