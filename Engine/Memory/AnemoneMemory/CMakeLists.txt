target_sources(AnemoneMemory
    PRIVATE
        "MemoryArena.cxx"
        "Module.cxx"
        "RawMemoryArena.cxx"

    PUBLIC FILE_SET HEADERS FILES
        "MemoryArena.hxx"
        "Module.hxx"
        "RawMemoryArena.hxx"
)
