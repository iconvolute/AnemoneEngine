add_subdirectory("Platform")

target_sources(AnemoneRuntime
    PUBLIC FILE_SET HEADERS FILES
        "Types.hxx"
)
