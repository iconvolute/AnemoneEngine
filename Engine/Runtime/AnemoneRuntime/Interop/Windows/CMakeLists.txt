if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "DateTime.hxx"
        "Debugger.hxx"
        "Dwm.hxx"
        "Environment.hxx"
        "FileSystem.hxx"
        "Graphics.hxx"
        "Headers.hxx"
        "MemoryBuffer.hxx"
        "MemoryMappedFile.hxx"
        "Process.hxx"
        "Registry.hxx"
        "SafeHandle.hxx"
        "Sdk.hxx"
        "StringBuffer.hxx"
        "System.hxx"
        "Text.hxx"
        "Threading.hxx"
        "UI.hxx"
)

endif()
