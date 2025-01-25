target_sources(AnemoneRuntime
    PRIVATE
        "DirectoryEnumerator.cxx"
        "Environment.hxx"
        "ErrorCode.cxx"
        "FileHandle.hxx"
        "FileSystem.hxx"
        "NamedMutex.hxx"
        "Path.cxx"
        "Path.hxx"
        "Process.cxx"
        "Process.hxx"
        "SharedLibrary.hxx"
)

if(WIN32)
    add_subdirectory("Windows")
elseif(ANDROID OR LINUX)
    add_subdirectory("Posix")
endif()
