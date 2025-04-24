target_sources(AnemoneRuntime
    PRIVATE
        "HashAlgorithm.hxx"
        "SecureMemory.hxx"
        "SHA256.hxx"
        "SHA512.hxx"
)

add_subdirectory("Internal")
