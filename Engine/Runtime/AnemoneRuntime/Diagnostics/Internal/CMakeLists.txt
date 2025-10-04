target_sources(AnemoneRuntime
    PRIVATE
        "ConsoleTraceListener.cxx"
        "FileTraceListener.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "ConsoleTraceListener.hxx"
        "FileTraceListener.hxx"
)
