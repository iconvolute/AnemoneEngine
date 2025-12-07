target_sources(AnemoneRuntime.Base
    PRIVATE
        "SimdDouble.cxx"
        "SimdFloat.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "SimdDouble.hxx"
        "SimdDouble.inl"
        "SimdFloat.hxx"
        "SimdFloat.inl"
)
