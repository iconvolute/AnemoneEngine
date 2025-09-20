target_sources(AnemoneRuntime
    PRIVATE
        "AES.cxx"
        "HashAlgorithm.cxx"
        "SHA256.cxx"
        "SHA512.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "AES.hxx"
        "BlockCipherPaddingMode.hxx"
        "HashAlgorithm.hxx"
        "SecureMemory.hxx"
        "SHA256.hxx"
        "SHA512.hxx"
)
