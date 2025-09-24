if (WIN32)

target_sources(AnemoneRuntime
    PRIVATE
        "StackTrace.cxx"

        "WindowsDebug.cxx"
        "WindowsDebugTraceListener.cxx"
        "WindowsEtwTraceListener.cxx"
        "WindowsTrace.cxx"
    PUBLIC FILE_SET HEADERS FILES

        "WindowsDebug.hxx"
        "WindowsDebugTraceListener.hxx"
        "WindowsEtwTraceListener.hxx"
        "WindowsTrace.hxx"
)

endif()
