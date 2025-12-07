target_sources(AnemoneRuntime.Base
    PRIVATE
        "MemoryArena.cxx"
        "RawMemoryArena.cxx"

    PUBLIC FILE_SET HEADERS FILES
        "MemoryArena.hxx"
        "RawMemoryArena.hxx"
)
