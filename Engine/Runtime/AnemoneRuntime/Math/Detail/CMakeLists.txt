target_sources(AnemoneRuntime
    PRIVATE
        "SimdDouble.cxx"
        "SimdFloat.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "SimdDouble.hxx"
        "SimdDouble.inl"
        "SimdFloat.inl"
        "SimdFloat.hxx"
)
