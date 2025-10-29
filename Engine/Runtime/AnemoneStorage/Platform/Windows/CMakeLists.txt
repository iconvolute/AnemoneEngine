if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "WindowsFileHandle.cxx"
        "WindowsFileSystem.cxx"
        "WindowsMemoryMappedFile.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "WindowsFileHandle.hxx"
        "WindowsFileSystem.hxx"
        "WindowsMemoryMappedFile.hxx"
)

endif()
